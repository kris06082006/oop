#ifndef UNORDERED_MAP_H
#define UNORDERED_MAP_H

#include <vector>
#include <utility> 
#include <iterator>
#include <algorithm>
#include <functional>
#include <cmath>

/**
 * @class UnorderedMap
 * @brief Кастомная реализация хеш-таблицы с открытой адресацией.
 * * Данный класс реализует ассоциативный контейнер, использующий квадратичное пробирование
 * для разрешения коллизий. Поддерживает интерфейс, максимально приближенный к std::unordered_map.
 * * @tparam Key Тип ключа
 * @tparam Value Тип значения
 * @tparam Hash Функтор хеширования
 * @tparam KeyEqual Функтор для сравнения ключей на равенство
 */
template<typename Key, 
        typename Value, 
        typename Hash = std::hash<Key>,
        typename KeyEqual = std::equal_to<Key>>
class UnorderedMap;

/**
 * @struct HashMapNode
 * @brief Внутренняя структура ячейки хеш-таблицы.
 * * Использует std::aligned_storage для отложенной инициализации объектов pair,
 * что позволяет избежать вызова конструкторов в пустых ячейках.
 */
template<typename Key, typename Value>
struct HashMapNode {
    /** @enum State Состояние ячейки */
    enum State { EMPTY, OCCUPIED, DELETED };
    typedef std::pair<const Key, Value> value_type;

    /// Выровненное хранилище для данных (ключ и значение)
    std::aligned_storage_t<sizeof(value_type), alignof(value_type)> storage = {};
    State state = EMPTY; ///< Текущий статус ячейки

    /** @brief Безопасное получение ссылки на данные через std::launder */
    value_type& get_value() noexcept {
        return *std::launder(reinterpret_cast<value_type*>(&storage));
    }
    const value_type&  get_value() const noexcept{
        return *std::launder(reinterpret_cast<const value_type*>(&storage));
    }

    /** @brief Получение ссылки на ключ текущего узла */
    const Key& get_key() const noexcept {
        return get_value().first;
    }
};

/**
 * @class HashMapIterator
 * @brief Двунаправленный итератор для обхода занятых ячеек UnorderedMap.
 */
template<typename Key, typename Value, bool IsConst>
class HashMapIterator {
public:
    typedef HashMapNode<Key, Value> node_t;
    typedef typename node_t::value_type value_type;
    typedef ptrdiff_t difference_type;
    typedef std::conditional_t<IsConst, const value_type*, value_type*> pointer;
    typedef std::conditional_t<IsConst, const value_type&, value_type&> reference;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::conditional_t<IsConst, const node_t*, node_t*> bucket_ptr_t;

    template<typename K, typename V, bool C> friend class HashMapIterator;

private:
    bucket_ptr_t buckets = nullptr; ///< Массив ячеек
    size_t current_index = 0;       ///< Текущая позиция
    size_t capacity = 0;            ///< Общий размер массива

    template<typename K, typename V, typename H, typename KE>
    friend class UnorderedMap;

    HashMapIterator(bucket_ptr_t arr, size_t index, size_t cap) noexcept
        : buckets(arr), current_index(index), capacity(cap) {}

public:
    HashMapIterator() noexcept = default;

    /** @brief Конструктор приведения (non-const -> const) */
    template<bool OtherConst>
    HashMapIterator(const HashMapIterator<Key, Value, OtherConst>& other) noexcept
        requires (IsConst >= OtherConst)
        : buckets(other.buckets), current_index(other.current_index), capacity(other.capacity) {}

    /** @brief Разыменование итератора */
    reference operator * () const noexcept {
        return buckets[current_index].get_value();
    }

    /** @brief Доступ к полям value_type (first/second) */
    pointer operator -> () const noexcept {
        return &buckets[current_index].get_value();
    }

    /** @brief Оператор сравнения итераторов */
    template<bool OtherConst>
    bool operator == (const HashMapIterator<Key, Value, OtherConst>& other) const noexcept {
        return current_index == other.current_index && buckets == other.buckets;
    }

    /** @brief Инкремент: поиск следующей ячейки в состоянии OCCUPIED */
    HashMapIterator& operator++() noexcept{
        size_t size = capacity; 
        current_index++;
        while (current_index < size) {
            if (buckets[current_index].state == node_t::OCCUPIED) {
                return *this;
            }
            current_index++;
        }
        current_index = size;
        return *this;
    }

    HashMapIterator operator++(int) noexcept {
        HashMapIterator temp = *this;
        ++(*this);
        return temp;
    }

    /** @brief Декремент: поиск предыдущей ячейки в состоянии OCCUPIED */
    HashMapIterator& operator--() noexcept {
        --current_index;
        while (current_index > 0 && buckets[current_index].state != node_t::OCCUPIED) {
            --current_index;
        }
        return *this;
    }

    HashMapIterator operator--(int) noexcept {
        HashMapIterator temp = *this;
        --(*this);
        return temp;
    }
};

template<typename Key, 
         typename Value, 
         typename Hash, 
         typename KeyEqual>
class UnorderedMap {
public:
    typedef HashMapNode<Key, Value> node_t;
    typedef Key key_type;
    typedef Value mapped_type;
    typedef typename node_t::value_type value_type; 
    typedef size_t size_type;

    typedef HashMapIterator<Key, Value, false> iterator;
    typedef HashMapIterator<Key, Value, true> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

private:
    node_t* buckets = nullptr;        ///< Массив бакетов (динамический)
    size_t buckets_capacity = 0;      ///< Вместимость таблицы
    size_t map_size = 0;              ///< Количество элементов (OCCUPIED)
    float max_load_factor_ = 0.5f;    ///< Коэффициент заполнения для рехеширования

    Hash hasher;            ///< Объект хеш-функции
    KeyEqual key_equal;     ///< Объект сравнения ключей

    /** @brief Размещение объекта pair в выделенной памяти ячейки */
    template<typename... Args>
    void construct_node(node_t* node, Args&&... args);
    
    /** @brief Реаллокация и перенос элементов в новую таблицу */
    void rehash(size_type new_count);
    
    /** @brief Алгоритм поиска индекса (квадратичное пробирование) */
    size_t find_slot(const key_type& key) const;

    /** @brief Обмен содержимым между двумя объектами */
    void swap(UnorderedMap& other) noexcept {
        std::swap(buckets, other.buckets);
        std::swap(buckets_capacity, other.buckets_capacity);
        std::swap(map_size, other.map_size);
        std::swap(hasher, other.hasher); 
        std::swap(key_equal, other.key_equal);
    }

public:
    /** @brief Создает пустую таблицу с заданным начальным размером */
    explicit UnorderedMap(size_type initial_capacity = 10);

    /** @brief Глубокое копирование таблицы */
    UnorderedMap(const UnorderedMap& other);
    UnorderedMap& operator=(const UnorderedMap& other);
    /** @brief Перемещение ресурсов (no-copy) */
    UnorderedMap(UnorderedMap&& other) noexcept;
    UnorderedMap& operator=(UnorderedMap&& other) noexcept;

    /** @brief Деструктор: ручное уничтожение OCCUPIED ячеек и освобождение памяти */
    ~UnorderedMap();

    /** @brief Возвращает итератор на первый занятый элемент */
    iterator begin() noexcept {
        size_t i = 0;
        size_t size = buckets_capacity;
        while (i < size) {
            if (buckets[i].state == node_t::OCCUPIED) {
                return iterator(buckets, i, buckets_capacity);
            }
            i++;
        }
        return end();
    }

    /** @brief Возвращает итератор на "за-конец" таблицы */
    iterator end() noexcept{
        return iterator(buckets, buckets_capacity, buckets_capacity);
    }

    const_iterator begin() const noexcept {
        size_t i = 0;
        size_t size = buckets_capacity;
        while (i < size) {
            if (buckets[i].state == node_t::OCCUPIED) {
                return const_iterator(buckets, i, buckets_capacity); 
            }
            i++;
        }
        return end();
    }

    const_iterator end() const noexcept {
        return const_iterator(buckets, buckets_capacity, buckets_capacity);
    }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    /** @brief Вставка элемента. Если ключ существует, вставки не происходит.
     * @return Пара {итератор на элемент, флаг успеха}
     */
    std::pair<iterator, bool> insert(const value_type& value);
    template<typename P>
    std::pair<iterator, bool> insert(P&& value);

    /** @brief Поиск элемента по ключу */
    iterator find(const key_type& key) noexcept;
    const_iterator find(const key_type& key) const noexcept;

    /** @brief Доступ к значению по ключу. Если ключа нет, создается новый элемент. */
    mapped_type& operator[](const key_type& key);

    /** @brief Доступ к значению с проверкой. Бросает исключение std::out_of_range. */
    mapped_type& at(const key_type& key);
    const mapped_type& at(const key_type& key) const; 

    /** @brief Резервирование памяти под ожидаемое количество элементов */
    void reserve(size_type count);
    /** @brief Текущий коэффициент заполнения таблицы */
    float load_factor() const noexcept;
    /** @brief Получение максимально допустимого коэффициента заполнения */
    float max_load_factor() const noexcept;
    /** @brief Установка нового порога для рехеширования */
    void max_load_factor(float ml);
    /** @brief Полная очистка контейнера */
    void clear() noexcept;

    /** @brief Количество элементов в таблице */
    size_type size() const noexcept { return map_size; }
    /** @brief Общее количество выделенных ячеек */
    size_type bucket_count() const noexcept { return buckets_capacity; }
    /** @brief Проверка на отсутствие элементов */
    bool empty() const noexcept { return map_size == 0; }

    /** @brief Удаление элемента по итератору */
    iterator erase(const_iterator position); 
    /** @brief Удаление элемента по ключу. Возвращает количество удаленных элементов (0 или 1). */
    size_type erase(const key_type& key);
    /** @brief Проверка наличия ключа в таблице */
    size_type count(const key_type& key) const noexcept;

    Hash hash_function() const;
    KeyEqual key_eq() const;
};

template<typename K, typename V, typename H, typename KE>
UnorderedMap<K, V, H, KE>::UnorderedMap(size_type initial_capacity) {
    if (initial_capacity == 0) initial_capacity = 10;

    buckets_capacity = initial_capacity;
    buckets = new node_t[initial_capacity];

    std::for_each(buckets, buckets + buckets_capacity, 
                    [](node_t& node) {
                    node.state = node_t::EMPTY;
                  });
    map_size = 0;
}


template<typename K, typename V, typename H, typename KE>
UnorderedMap<K, V, H, KE>::~UnorderedMap() {
    for (size_t i = 0; i < buckets_capacity; ++i) {
        if (buckets[i].state == node_t::OCCUPIED) {
            std::destroy_at(&buckets[i].get_value());
        }
    }

    delete[] buckets;
    buckets = nullptr;
    buckets_capacity = 0;
}

template<typename K, typename V, typename H, typename KE>
template<typename... Args>
void UnorderedMap<K, V, H, KE>::construct_node(node_t* node, Args&&... args) {
    ::new (const_cast<void*>(reinterpret_cast<const void*>(&node->get_value()))) 
        typename node_t::value_type(std::forward<Args>(args)...);
    node->state = node_t::OCCUPIED;
}

template<typename K, typename V, typename H, typename KE>
UnorderedMap<K, V, H, KE>::UnorderedMap(const UnorderedMap& other)
    : map_size(0), hasher(other.hasher), key_equal(other.key_equal) {
    
    buckets_capacity = other.buckets_capacity;
    buckets = new node_t[buckets_capacity];

    std::for_each(buckets, buckets + buckets_capacity,
                    [](node_t& node) {
                        node.state = node_t::EMPTY;
                    });
    for (size_type i = 0; i < other.buckets_capacity; ++i) {
        const node_t& other_node = other.buckets[i];

        if (other_node.state == node_t::OCCUPIED) {
            insert(other_node.get_value());
        }
    }
}

template<typename K, typename V, typename H, typename KE>
UnorderedMap<K, V, H, KE>& UnorderedMap<K, V, H, KE>::operator=(const UnorderedMap& other) {
    UnorderedMap temp(other);

    std::swap(*this, temp);

    return *this;
}

template<typename K, typename V, typename H, typename KE>
UnorderedMap<K, V, H, KE>::UnorderedMap(UnorderedMap&& other) noexcept
    : buckets(other.buckets), 
      buckets_capacity(other.buckets_capacity), 
      map_size(other.map_size),
      hasher(std::move(other.hasher)), 
      key_equal(std::move(other.key_equal)) {

        other.buckets = nullptr;
        other.buckets_capacity = 0;
        other.map_size = 0;
    }

template<typename K, typename V, typename H, typename KE>
UnorderedMap<K, V, H, KE>& UnorderedMap<K, V, H, KE>::operator=(UnorderedMap&& other) noexcept {
    if (this != &other) {
        swap(other);
    }
    return *this;
}


template<typename K, typename V, typename H, typename KE>
size_t UnorderedMap<K, V, H, KE>::find_slot(const key_type& key) const {
    size_t size = buckets_capacity;
    if (size == 0) return 0;

    size_t start_index = hasher(key) % size;
    size_t first_deleted_slot = size;

    for (size_t step = 0; step < size; ++step) {
        size_t current_index = (start_index + step * step) % size;
        typename node_t::State state = buckets[current_index].state;

        if (state == node_t::OCCUPIED){
            if(key_equal(buckets[current_index].get_key(), key)) {
                return current_index;
            }
        }
        else if (state == node_t::EMPTY) {
            return (first_deleted_slot != size) ? first_deleted_slot : current_index;
        }
        else {
            if (first_deleted_slot == size) {
                first_deleted_slot = current_index;
            }
        }
    }
    return first_deleted_slot;
}

template<typename K, typename V, typename H, typename KE>
std::pair<typename UnorderedMap<K, V, H, KE>::iterator, bool>
UnorderedMap<K, V, H, KE>::insert(const value_type& value) {
    return insert(value_type(value));
}

template<typename K, typename V, typename H, typename KE>
template<typename P>
std::pair<typename UnorderedMap<K, V, H, KE>::iterator, bool>
UnorderedMap<K, V, H, KE>::insert(P&& value) {
    
    const key_type& key = value.first;
    
    if (buckets_capacity == 0 || (float)(map_size + 1) / buckets_capacity > max_load_factor_) {
        size_type min_cap = 10;
        size_type new_capacity = std::max(buckets_capacity * 2, min_cap);
        rehash(new_capacity);
    }

    size_type slot_index = find_slot(key);
    size_type size = buckets_capacity; 

    if (slot_index == size) {
        return {end(), false}; 
    }
    
    node_t* slot = &buckets[slot_index];

    if (slot->state == node_t::OCCUPIED) {
        return {iterator(buckets, slot_index, buckets_capacity), false}; 
    }

    construct_node(slot, std::forward<P>(value));

    map_size++; 

    return {iterator(buckets, slot_index, buckets_capacity), true};
}

template<typename K, typename V, typename H, typename KE>
void UnorderedMap<K, V, H, KE>::rehash(size_type new_capacity) {
    if (new_capacity <= buckets_capacity) {
        return;
    }

    node_t* new_buckets = new node_t[new_capacity];

    std::for_each(new_buckets, new_buckets + new_capacity, 
                    [](node_t& node) {
                    node.state = node_t::EMPTY;
                  }); 

    node_t* old_buckets = buckets;
    size_type old_capacity = buckets_capacity;
    size_type current_map_size = map_size;

    buckets = new_buckets;
    buckets_capacity = new_capacity;

    for (size_type i = 0; i < old_capacity; ++i) {
        node_t& old_node = old_buckets[i];
        if (old_node.state == node_t::OCCUPIED) {
            const key_type& key = old_node.get_key();
            size_type new_slot_index = find_slot(key);
            node_t* new_slot = &buckets[new_slot_index];
            construct_node(new_slot, std::move(old_node.get_value()));
            std::destroy_at(&old_node.get_value());
        }
    }

    delete[] old_buckets;
}

template<typename K, typename V, typename H, typename KE>
typename UnorderedMap<K, V, H, KE>::iterator
UnorderedMap<K, V, H, KE>::find(const key_type& key) noexcept {
    size_type index = find_slot(key);
    if (index < buckets_capacity && buckets[index].state == node_t::OCCUPIED) {
        return iterator(buckets, index, buckets_capacity);
    }
    return end();
}

template<typename K, typename V, typename H, typename KE>
typename UnorderedMap<K, V, H, KE>::const_iterator 
UnorderedMap<K, V, H, KE>::find(const key_type& key) const noexcept {
    
    size_type index = find_slot(key);

    if (index < buckets_capacity && buckets[index].state == node_t::OCCUPIED) {
        return const_iterator(buckets, index, buckets_capacity);
    }
    
    return end(); 
}

template<typename K, typename V, typename H, typename KE>
typename UnorderedMap<K, V, H, KE>::mapped_type&
UnorderedMap<K, V, H, KE>::operator[](const key_type& key) {

    size_type index = find_slot(key);
    node_t* slot = &buckets[index];
    if (slot->state == node_t::OCCUPIED) {
        return slot->get_value().second;
    }

    if (buckets_capacity == 0 || (float)(map_size + 1)/buckets_capacity > max_load_factor_) {
        size_type min_cap = 10;
        size_type new_capacity = std::max(buckets_capacity * 2, min_cap);
        rehash(new_capacity);

        index = find_slot(key);
        slot = &buckets[index];
    }

    construct_node(slot, value_type(key, mapped_type()));
    map_size++;
    return slot->get_value().second;

}

template<typename K, typename V, typename H, typename KE>
typename UnorderedMap<K, V, H, KE>::size_type
UnorderedMap<K, V, H, KE>::erase(const key_type& key) {
    
    size_type index = find_slot(key);
    
    if (index < buckets_capacity && buckets[index].state == node_t::OCCUPIED) {
        node_t* slot = &buckets[index];
        std::destroy_at(&slot->get_value());
        slot->state = node_t::DELETED;
        map_size--;    
        return 1;
    }
    
    return 0; 
}

template<typename K, typename V, typename H, typename KE>
typename UnorderedMap<K, V, H, KE>::iterator
UnorderedMap<K, V, H, KE>::erase(const_iterator position) {

    if (position == end()) {
        return end();
    }

    size_type index = position.current_index;
    
    node_t* slot = &buckets[index];
    std::destroy_at(&slot->get_value());
    slot->state = node_t::DELETED;
    map_size--;
    
    size_type next_index = index + 1;
    size_type size = buckets_capacity;
    
    while (next_index < size) {
        if (buckets[next_index].state == node_t::OCCUPIED) {
            return iterator(buckets, next_index, buckets_capacity);
        }
        next_index++;
    }

    return end();
}

template<typename K, typename V, typename H, typename KE>
void UnorderedMap<K, V, H, KE>::clear() noexcept {
    for (size_t i = 0; i < buckets_capacity; ++i) {
        node_t& slot = buckets[i];
        if (slot.state == node_t::OCCUPIED) {
            std::destroy_at(&slot.get_value());
            slot.state = node_t::EMPTY;
        } else if (slot.state == node_t::DELETED) {
            slot.state = node_t::EMPTY; 
        }
    }
    map_size = 0;
}

template<typename K, typename V, typename H, typename KE>
typename UnorderedMap<K, V, H, KE>::mapped_type&
UnorderedMap<K, V, H, KE>::at(const key_type& key) {
    size_type index = find_slot(key);
    if (index < buckets_capacity && buckets[index].state == node_t::OCCUPIED){
        return buckets[index].get_value().second;
    }
    throw std::out_of_range("Key not found in UnorderedMap::at");
}

template<typename K, typename V, typename H, typename KE>
const typename UnorderedMap<K, V, H, KE>::mapped_type&
UnorderedMap<K, V, H, KE>::at(const key_type& key) const {
    size_type index = find_slot(key);

    if (index < buckets_capacity && buckets[index].state == node_t::OCCUPIED) {
        return buckets[index].get_value().second;
    }

    throw std::out_of_range("Key not found in UnorderedMap::at");
}

template<typename K, typename V, typename H, typename KE>
float UnorderedMap<K, V, H, KE>::load_factor() const noexcept {
    if (buckets_capacity == 0) {
        return 0.0f; 
    }
    return (float)map_size / buckets_capacity;
}

template<typename K, typename V, typename H, typename KE>
void UnorderedMap<K, V, H, KE>::max_load_factor(float ml) {
    if (ml <= 0) {
        return; 
    }  
    max_load_factor_ = ml;
    float current_lf = load_factor(); 
    if (current_lf > max_load_factor_) {
        size_type new_capacity = (size_type)std::ceil((float)map_size / max_load_factor_);
        rehash(new_capacity);
    }
}

template<typename K, typename V, typename H, typename KE>
float UnorderedMap<K, V, H, KE>::max_load_factor() const noexcept {
    return max_load_factor_; 
}

template<typename K, typename V, typename H, typename KE>
void UnorderedMap<K, V, H, KE>::reserve(size_type count) {
    if (count == 0) return;
    
    size_type min_capacity = (size_type)std::ceil((float)count / max_load_factor_);

    if (min_capacity > buckets_capacity) {
        rehash(min_capacity);
    }
}

template<typename K, typename V, typename H, typename KE>
H UnorderedMap<K, V, H, KE>::hash_function() const {
    return hasher;
}

template<typename K, typename V, typename H, typename KE>
KE UnorderedMap<K, V, H, KE>::key_eq() const {
    return key_equal;
}

template<typename K, typename V, typename H, typename KE>
typename UnorderedMap<K, V, H, KE>::size_type
UnorderedMap<K, V, H, KE>::count(const key_type& key) const noexcept {
    size_type index = find_slot(key);
    
    if (index < buckets_capacity && buckets[index].state == node_t::OCCUPIED) {
        return 1;
    }
    return 0;
}

#endif
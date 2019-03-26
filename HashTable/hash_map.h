#include <functional>
#include <list>
#include <stdexcept>
#include <utility>
#include <vector>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
public:
    HashMap(const Hash& custom_hasher = std::hash<KeyType>())
        : elements()
        , buckets(START_COUNT_BUCKETS, std::make_pair(elements.end(), elements.end()))
        , count_elements()
        , hasher(custom_hasher)
    {}

    template<typename ForwardIt>
    HashMap(ForwardIt first, ForwardIt last, const Hash& custom_hasher = Hash())
        : HashMap(custom_hasher)
    {
        while (first != last) {
            insert(*first++);
        }
    }

    HashMap(const std::initializer_list<std::pair<const KeyType, ValueType>>& list_of_elements,
            const Hash& custom_hasher = Hash())
        : HashMap(custom_hasher)
    {
        for (const auto& element : list_of_elements) {
            insert(element);
        }
    }

    HashMap(const HashMap& other)
        : HashMap(other.hasher)
    {
        *this = other;
    }

    HashMap& operator = (const HashMap& other) {
        if (this == &other) {
            return *this;
        }
        count_elements = other.count_elements;
        elements.clear();
        buckets.assign(other.buckets.size(), std::make_pair(elements.end(), elements.end()));
        for (size_t curr_hash = 0; curr_hash < buckets.size(); ++curr_hash) {
            auto it = other.buckets[curr_hash].first;
            if (it != other.elements.end()) {
                elements.push_front(*it);
                buckets[curr_hash].first = elements.begin();
                while (it != other.buckets[curr_hash].second) {
                    ++it;
                    elements.push_front(*it);
                }
                buckets[curr_hash].second = elements.begin();
            }
        }
        return *this;
    }

    size_t size() const {
        return count_elements;
    }

    bool empty() const {
        return count_elements == 0;
    }

    Hash hash_function() const {
        return hasher;
    }

    void clear() {
        count_elements = 0;
        elements.clear();
        buckets.assign(START_COUNT_BUCKETS, std::make_pair(elements.end(), elements.end()));
    }

    void erase(const KeyType& key) {
        size_t num = hasher(key) % buckets.size();
        if (buckets[num].first == elements.end()) {
            return;
        }
        auto it = buckets[num].first;
        while (it != buckets[num].second && !(it->first == key)) {
            ++it;
        }
        if (it->first == key) {
            if (it == buckets[num].first) {
                if (it == buckets[num].second) {
                    buckets[num].first = elements.end();
                    buckets[num].second = elements.end();
                } else {
                    ++buckets[num].first;
                }
            } else if (it == buckets[num].second) {
                --buckets[num].second;
            }
            --count_elements;
            elements.erase(it);
        }
    }

    void increase_size(size_t new_size) {
        decltype(elements) old_elements;
        std::swap(elements, old_elements);
        buckets.assign(new_size, std::make_pair(elements.end(), elements.end()));
        for (const std::pair<const KeyType, ValueType>& element : old_elements) {
            size_t num = hasher(element.first) % new_size;
            if (buckets[num].first == elements.end()) {
                elements.push_front(element);
                buckets[num].second = elements.begin();
                buckets[num].first = elements.begin();
            } else {
                buckets[num].first = elements.insert(buckets[num].first, element);
            }
        }
    }

    class iterator {
    public:
        iterator(const iterator& other)
            : it(other.it)
        {}

        iterator()
            : it()
        {}

        bool operator == (iterator other) const {
            return it == other.it;
        }

        bool operator != (iterator other) const {
            return it != other.it;
        }

        iterator& operator ++ () {
            ++it;
            return *this;
        }

        iterator operator ++ (int) {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        typename std::list<std::pair<const KeyType, ValueType>>::iterator operator -> () const {
            return it;
        }

        iterator& operator -- () {
            --it;
            return *this;
        }

        iterator operator -- (int) {
            iterator tmp = *this;
            --it;
            return tmp;
        }

        std::pair<const KeyType, ValueType>& operator * () const {
            return *it;
        }

        iterator(typename std::list<std::pair<const KeyType, ValueType>>::iterator it_)
            : it(it_)
        {}
    private:
        typename std::list<std::pair<const KeyType, ValueType>>::iterator it;
    };

    class const_iterator {
    public:
        const_iterator(const const_iterator& other)
            : it(other.it)
        {}

        const_iterator()
            : it()
        {}

        bool operator == (const_iterator other) const {
            return it == other.it;
        }

        bool operator != (const_iterator other) const {
            return it != other.it;
        }

        const_iterator& operator ++ () {
            ++it;
            return *this;
        }

        const_iterator operator ++ (int) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        const_iterator& operator -- () {
            --it;
            return *this;
        }

        const_iterator operator -- (int) {
            const_iterator tmp = *this;
            --it;
            return tmp;
        }

        typename std::list<std::pair<const KeyType, ValueType>>::const_iterator operator -> () const {
            return it;
        }

        const std::pair<const KeyType, ValueType>& operator * () const {
            return *it;
        }

        const_iterator(typename std::list<std::pair<const KeyType, ValueType>>::const_iterator it_)
            : it(it_)
        {}

    private:
        typename std::list<std::pair<const KeyType, ValueType>>::const_iterator it;
    };

    iterator find(const KeyType& key) {
        size_t num = hasher(key) % buckets.size();
        auto it = buckets[num].first;
        while (it != buckets[num].second && !(it->first == key)) {
            ++it;
        }
        if (it != elements.end() && it->first == key) {
            return it;
        }
        return end();
    }

    const_iterator find(const KeyType& key) const {
        size_t num = hasher(key) % buckets.size();
        auto it = buckets[num].first;
        while (it != buckets[num].second && !(it->first == key)) {
            ++it;
        }
        if (it != elements.end() && it->first == key) {
            return const_iterator(it);
        }
        return end();
    }

    ValueType& operator [](const KeyType& key) {
        auto it = find(key);
        if (it == end()) {
            return insert_skipping_check(std::pair<const KeyType, ValueType>(key, ValueType()))->second;
        } else {
            return it->second;
        }
    }

    const ValueType& at(const KeyType& key) const {
        auto it = find(key);
        if (it == end()) {
            throw std::out_of_range("Key not found");
        }
        return it->second;
    }

    iterator insert(const std::pair<const KeyType, ValueType>& element) {
        auto it = find(element.first);
        if (it == end()) {
            return insert_skipping_check(element);
        } else {
            return it;
        }
    }

    iterator begin() {
        return iterator(elements.begin());
    }

    const_iterator begin() const {
        return const_iterator(elements.begin());
    }

    iterator end() {
        return iterator(elements.end());
    }

    const_iterator end() const {
        return const_iterator(elements.end());
    }

private:
    std::list<std::pair<const KeyType, ValueType>> elements;
    std::vector<std::pair<typename decltype(elements)::iterator, typename decltype(elements)::iterator>> buckets;
    size_t count_elements;
    Hash hasher;
    static constexpr float maxLoadFactor = 0.75;
    static const int START_COUNT_BUCKETS = 1;

    iterator insert_skipping_check(const std::pair<const KeyType, ValueType>& element) {
        ++count_elements;
        if (count_elements > maxLoadFactor * buckets.size()) {
            increase_size(2 * buckets.size());
        }
        size_t num = hasher(element.first) % buckets.size();
        if (buckets[num].first == elements.end()) {
            elements.push_front(element);
            buckets[num].second = elements.begin();
            buckets[num].first = elements.begin();
            return buckets[num].first;
        } else {
            return buckets[num].first = elements.insert(buckets[num].first, element);
        }
    }
};

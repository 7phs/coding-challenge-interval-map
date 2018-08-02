#include <assert.h>
#include <map>
#include <limits>

template<class K, class V>
class interval_map {
    friend void IntervalMapTest();

private:
    std::map<K, V> m_map;

public:
    interval_map(V const &val) {
        m_map.insert(m_map.begin(), std::make_pair(std::numeric_limits<K>::lowest(), val));
    }

    void assign(K const &keyBegin, K const &keyEnd, V const &val) {
        if (!(keyBegin < keyEnd)) {
            return;
        }

        auto start = [&]() -> auto {
            if (keyBegin == std::numeric_limits<K>::lowest()) {
                return begin(m_map);
            } else {
                return m_map.upper_bound(keyBegin);
            }
        }();

        bool is_start_begin = (start == begin(m_map));
        auto insert_it = start;
        insert_it--;

        // do not change values outside this interval
        // but the previous interval will contains an equal value
        if (!is_start_begin && insert_it->first < keyBegin && val == insert_it->second) {
            return;
        }

        auto finish = [&]() -> auto {
            if (keyEnd == std::numeric_limits<K>::max()) {
                return end(m_map);
            } else {
                auto it = this->m_map.upper_bound(keyEnd - 1);
                if (it != end(m_map)) {
                    --it;
                }

                return it;
            }
        }();

        auto finish_key = finish->first;
        auto finish_value = finish->second;

        if (finish != end(m_map)) {
            // do not change values outside this interval
            // but the next interval will contains an equal value
            if (val == finish->second) {
                return;
            }
        } else {
            --finish;
            finish_key = finish->first;
            finish_value = finish->second;
            ++finish;
        }

        // erase all values inside the interval
        if (start == begin(m_map) && finish == end(m_map)) {
            m_map.clear();
        } else if (start == finish && start == end(m_map)) {
            // skip it
        } else if (start == begin(m_map) || finish == end(m_map) ||
                   start->first < finish->first ||
                   start->first == finish->first) {
            if (finish != end(m_map)) {
                ++finish;
            }

            m_map.erase(start, finish);
        }

        // insert a new interval if not need to extend existing
        if (is_start_begin) {
            insert_it = begin(m_map);
        }

        auto new_it = m_map.insert(insert_it, std::pair<K, V>(keyBegin, val));

        // reduce the last intersection interval
        if (keyEnd < std::numeric_limits<K>::max() && finish_value != val && finish_key > keyBegin &&
            finish_key < keyEnd) {
            m_map.insert(new_it, std::pair<K, V>(keyEnd, finish_value));
        }
    }

    // look-up of the value associated with key
    V const &operator[](K const &key) const {
        return (--m_map.upper_bound(key))->second;
    }

    size_t size() const {
        return m_map.size();
    }
};

#pragma once

// std
#include <functional>
#include <optional>
#include <map>
#include <assert.h>

namespace CGE
{
	namespace RHI
	{
        struct Interval
        {
            Interval() = default;
            Interval(uint32_t min, uint32_t max) : m_min{ min }, m_max{ max } {}

            uint32_t m_min = 0;
            uint32_t m_max = 0;

            bool operator==(const Interval& rhs) const { return m_min == rhs.m_min && m_max == rhs.m_max; }
            bool operator!=(const Interval& rhs) const { return m_min != rhs.m_min || m_max != rhs.m_max; }
        };

		// This class is used for tracking image subresource states. (Check ImageProperty.h)
		template<typename Key, typename T, typename Compare = std::less<Key>>
        class interval_map
        {
            using this_type = interval_map<Key, T, Compare>;

        public:
            template<class T1, class T2>
            using pair = std::pair<T1, T2>;
            using mapped_type = std::optional<T>;
            using Allocator = std::allocator<std::pair<const Key, mapped_type>>;
            using container_type = std::map<Key, mapped_type, Compare, Allocator>;
            using container_iterator = typename container_type::iterator;
            using container_const_iterator = typename container_type::const_iterator;
            using key_compare = typename container_type::key_compare;
            using value_compare = typename container_type::value_compare;
            using key_type = typename container_type::key_type;
            using size_type = std::size_t;

        public:
            class const_iterator
            {
                using this_type = const_iterator;
                friend class interval_map<Key, T, Compare>;

            public:
                using reference_value = const T&;
                using interval_type = pair<key_type, key_type>;

                const_iterator() {}
                const_iterator(const container_type& container, container_const_iterator iter) : m_container(&container), m_iterator(iter) {}

                bool operator==(const const_iterator& other) const
                {
                    return m_container == other.m_container && m_iterator == other.m_iterator;
                }

                bool operator!=(const const_iterator& other) const
                {
                    return m_container != other.m_container || m_iterator != other.m_iterator;
                }

                inline key_type interval_begin() const
                {
                    assert(m_iterator != m_container->end(), "Invalid begin interval");
                    return m_iterator->first;
                }

                inline key_type interval_end() const
                {
                    assert(m_iterator != m_container->end(), "Invalid begin interval");
                    auto next = std::next(m_iterator);
                    assert(next != m_container->end(), "Invalid end interval");
                    return next->first;
                }

                inline reference_value value() const
                {
                    assert(m_iterator != m_container->end() && m_iterator->second.has_value(), "Invalid begin interval");
                    return m_iterator->second.value();
                }

                inline interval_type interval() const
                {
                    return std::make_pair(interval_begin(), interval_end());
                }

                pair<interval_type, reference_value> operator*() const
                {
                    return std::pair<interval_type, reference_value>(interval(), value());
                }

                // pre-increment
                inline this_type& operator++()
                {
                    assert(m_iterator != m_container->end(), "Invalid begin interval");
                    while (++m_iterator != m_container->end() && !m_iterator->second.has_value()) {};
                    return *this;
                }

                // post-increment
                inline this_type operator++(int)
                {
                    this_type tmp = *this;
                    ++(*this);
                    return tmp;
                }

                inline this_type& operator--()
                {
                    assert(m_iterator != m_container->begin(), "Interval is at the beginning");
                    while (--m_iterator != m_container->begin() && !m_iterator->second.has_value()) {};
                    return *this;
                }

                inline this_type operator--(int)
                {
                    this_type tmp = *this;
                    --(*this);
                    return tmp;
                }

            protected:
                const container_type* m_container = nullptr;
                container_const_iterator m_iterator;
            };

            class iterator : public const_iterator
            {
                using this_type = iterator;
                using base_type = const_iterator;

            public:
                using reference_value = T&;

                iterator() {}
                iterator(const container_type& container, container_iterator iter) : const_iterator(container, iter) {}

                inline this_type& operator++()
                {
                    base_type::operator++();
                    return *this;
                }

                inline this_type operator++(int)
                {
                    this_type temp = *this;
                    ++(*this);
                    return temp;
                }

                inline this_type& operator--()
                {
                    base_type::operator--();
                    return *this;
                }

                inline this_type operator--(int)
                {
                    this_type temp = *this;
                    --(*this);
                    return temp;
                }

                inline reference_value value() const
                {
                    assert(this->m_iterator != this->m_container->end() && this->m_iterator->second.has_value(), "Invalid begin interval");
                    return const_cast<reference_value>(this->m_iterator->second.value());
                }
            };

            inline explicit interval_map(const Compare& comp = Compare(), const Allocator& alloc = Allocator()) : m_container(comp, alloc) {}
            template <class InputIterator>
            inline interval_map(InputIterator first, InputIterator last, const Compare& comp = Compare(), const Allocator& alloc = Allocator()) : m_container(first, last, comp, alloc) {}
            inline interval_map(const this_type& rhs) : m_container(rhs.m_container) {}
            inline interval_map(const this_type& rhs, const Allocator& alloc) : m_container(rhs.m_container, alloc) {}

            inline this_type& operator=(const this_type& rhs)
            {
                m_container = rhs.m_container;
                return *this;
            }
            inline key_compare key_comp() const { return m_container.key_comp(); }
            inline value_compare value_comp() const { return m_container.value_comp(); }

            inline iterator begin() { return iterator(m_container, m_container.begin()); }
            inline iterator end() { return iterator(m_container, m_container.end()); }
            inline const_iterator begin() const { return const_iterator(m_container, m_container.begin()); }
            inline const_iterator end() const { return const_iterator(m_container, m_container.end()); }
            inline bool empty() const { return m_container.empty(); }
            inline void swap(this_type& rhs) { m_container.swap(rhs.m_container); }

            inline const_iterator at(const key_type& key) const
            {
                // We get upper bound the pverious members value will be our resource state.
                auto upper_bound = m_container.upper_bound(key);
                if (upper_bound == m_container.begin() || !has_value(std::prev(upper_bound)))
                {
                    return end();
                }
                return const_iterator(m_container, std::prev(upper_bound));
            }

            // Returns a begin and end iterator for the intervals in the map that overlaps with the [lower, upper) interval
            // that was passed. In this function the end_range will be the first range that it does NOT overlap with.
            inline pair<const_iterator, const_iterator> overlap(const key_type& lower, const key_type& upper) const
            {
                // lower bound should be bigger than upper bound.
                if (!key_comp()(lower, upper))
                {
                    return pair<const_iterator, const_iterator>(end(), end());
                }

                auto lower_bound = m_container.upper_bound(lower);
                auto upper_bound = m_container.upper_bound(upper);
                if (lower_bound == m_container.end() || upper_bound == m_container.begin())
                {
                    return pair<const_iterator, const_iterator>(end(), end());
                }

                const_iterator begin_range;
                const_iterator end_range;
                if (lower_bound == m_container.begin())
                {
                    begin_range = const_iterator(m_container, lower_bound);
                }
                else
                {
                    auto prev = std::prev(lower_bound);
                    begin_range = const_iterator(m_container, has_value(prev) ? prev : lower_bound);
                }

                if (upper_bound == m_container.end())
                {
                    end_range = const_iterator(m_container, upper_bound);
                }
                else
                {
                    auto next = std::next(upper_bound);
                    end_range = const_iterator(m_container, has_value(upper_bound) ? upper_bound : next);
                }

                return pair<const_iterator, const_iterator>(begin_range, end_range);
            }

            // Assign the interval [lower, upper) with a new value. All overlapping intervals will
            // have their value overwritten with the new value. All adjacent intervals with the same value
            // will be merge together.
            inline iterator assign(const key_type& lower, const key_type& upper, const T& value)
            {
                if (!key_comp()(lower, upper))
                {
                    return end();
                }

                auto last = insert_upper_bound(upper);
                auto first = m_container.lower_bound(lower);
                m_container.erase(first, last);
                first = m_container.insert(last, std::make_pair(lower, value));

                // Merge intervals with the same value.
                if (first != m_container.begin())
                {
                    auto prev = std::prev(first);
                    if (has_value(prev) && prev->second == value)
                    {
                        m_container.erase(first);
                        first = prev;
                    }
                }

                if (has_value(last) &&
                    last->second == value)
                {
                    last = m_container.erase(last);
                }

                return iterator(m_container, first);
            }

            inline const_iterator erase(const_iterator erasePos)
            {
                if (erasePos == end())
                {
                    return end();
                }

                container_const_iterator eraseIt;
                auto lower = erasePos.m_iterator;
                auto upper = std::next(erasePos.m_iterator);
                assert(lower != m_container.end() && upper != m_container.end(), "Invalid erase iterator");

                if (!has_value(upper))
                {
                    m_container.erase(upper);
                }

                if (lower == m_container.begin() || !has_value(std::prev(lower)))
                {
                    eraseIt = m_container.erase(lower);
                }
                else
                {
                    const_cast<mapped_type&>(lower->second).reset();
                    eraseIt = std::next(lower);
                }

                return const_iterator(m_container, eraseIt);
            }

            inline void clear() { m_container.clear(); }

        private:
            bool has_value(container_const_iterator iterator) const
            {
                return iterator->second.has_value();
            }

            // Inserts the upper bound of an interval
            auto insert_upper_bound(const key_type& upper)
            {
                auto last = m_container.upper_bound(upper);
                if (last == m_container.begin())
                {
                    return m_container.insert(last, { upper, mapped_type() });
                }

                auto&& value_before = std::prev(last)->second;
                return m_container.insert(last, std::make_pair(upper, value_before));
            }

            container_type m_container;
        };
	}
}
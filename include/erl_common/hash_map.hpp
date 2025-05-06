#pragma once

#include <unordered_map>
#include <vector>

namespace erl::common {

    template<typename KeyType, typename ValueType>
    class HashMap {

        bool m_use_vector_;
        std::vector<ValueType> m_vector_;
        std::unordered_map<KeyType, ValueType> m_map_;

    public:
        explicit HashMap(bool use_vector, std::size_t capacity = 1e7) {
            // if ValueType is a pointer, memory for 1E7 elements is 76MB in a 64-bit system.
            // memory for 1E9 elements is 7.6GB in a 64-bit system, too much.
            if (capacity > static_cast<std::size_t>(1e9)) { use_vector = false; }
            if (use_vector) { m_vector_.resize(capacity); }
            m_use_vector_ = use_vector;
        }

        [[nodiscard]] bool
        UseVector() const {
            return m_use_vector_;
        }

        [[nodiscard]] std::size_t
        Size() const {
            if (m_use_vector_) { return m_vector_.size(); }
            return m_map_.size();
        }

        [[nodiscard]] bool
        Contains(const KeyType& key) const {
            if (m_use_vector_) { return true; }
            return m_map_.contains(key);
        }

        ValueType&
        operator[](const KeyType& key) {
            if (m_use_vector_) { return m_vector_[key]; }
            return m_map_[key];
        }

        const ValueType&
        operator[](const KeyType& key) const {
            if (m_use_vector_) { return m_vector_[key]; }
            return m_map_.at(key);
        }

        typename std::vector<ValueType>::iterator
        VectorBegin() {
            return m_vector_.begin();
        }

        typename std::vector<ValueType>::const_iterator
        VectorBegin() const {
            return m_vector_.begin();
        }

        typename std::vector<ValueType>::iterator
        VectorEnd() {
            return m_vector_.end();
        }

        typename std::vector<ValueType>::const_iterator
        VectorEnd() const {
            return m_vector_.end();
        }

        typename std::unordered_map<KeyType, ValueType>::iterator
        MapBegin() {
            return m_map_.begin();
        }

        typename std::unordered_map<KeyType, ValueType>::const_iterator
        MapBegin() const {
            return m_map_.begin();
        }

        typename std::unordered_map<KeyType, ValueType>::iterator
        MapEnd() {
            return m_map_.end();
        }

        typename std::unordered_map<KeyType, ValueType>::const_iterator
        MapEnd() const {
            return m_map_.end();
        }
    };

}  // namespace erl::common

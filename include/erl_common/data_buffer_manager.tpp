#pragma once

#include "data_buffer_manager.hpp"
#include "serialization.hpp"

#include <unordered_set>

namespace erl::common {

    template<typename T, class Buffer>
    template<typename T0, typename T1>
    bool
    DataBufferManager<T, Buffer>::Writer<T0, T1>::Run(const T *entry, std::ostream &stream) {
        stream.write(reinterpret_cast<const char *>(entry), sizeof(T));
        return stream.good();
    }

    template<typename T, class Buffer>
    template<typename C>
    bool
    DataBufferManager<T, Buffer>::Writer<C, std::void_t<decltype(std::declval<C>().Write())>>::Run(
        const T *entry,
        std::ostream &stream) {
        return entry->Write(stream);
    }

    template<typename T, class Buffer>
    template<typename T0, typename T1>
    bool
    DataBufferManager<T, Buffer>::Reader<T0, T1>::Run(T *entry, std::istream &stream) {
        stream.read(reinterpret_cast<char *>(entry), sizeof(T));
        return stream.good();
    }

    template<typename T, class Buffer>
    template<typename C>
    bool
    DataBufferManager<T, Buffer>::Reader<C, std::void_t<decltype(std::declval<C>().Read())>>::Run(
        T *entry,
        std::istream &stream) {
        return entry->Read(stream);
    }

    template<typename T, class Buffer>
    DataBufferManager<T, Buffer>::Iterator::Iterator(DataBufferManager *manager)
        : m_manager_(manager) {
        if (m_manager_ == nullptr) { return; }
        if (m_manager_->m_entries_.empty()) {
            m_manager_ = nullptr;
            return;
        }
        m_available_indices_.insert(
            m_manager_->m_available_indices_.begin(),
            m_manager_->m_available_indices_.end());
        m_index_ = 0;
        while (m_index_ < m_manager_->m_entries_.size() && m_available_indices_.count(m_index_)) {
            ++m_index_;
        }
        if (m_index_ >= m_manager_->m_entries_.size()) {
            m_manager_ = nullptr;
            return;
        }
    }

    template<typename T, class Buffer>
    bool
    DataBufferManager<T, Buffer>::Iterator::operator==(const Iterator &other) const {
        if (m_manager_ != other.m_manager_) { return false; }
        if (m_manager_ == nullptr) { return true; }
        return m_index_ == other.m_index_;
    }

    template<typename T, class Buffer>
    bool
    DataBufferManager<T, Buffer>::Iterator::operator!=(const Iterator &other) const {
        return !(*this == other);
    }

    template<typename T, class Buffer>
    T &
    DataBufferManager<T, Buffer>::Iterator::operator*() {
        return m_manager_->m_entries_[m_index_];
    }

    template<typename T, class Buffer>
    T *
    DataBufferManager<T, Buffer>::Iterator::operator->() {
        return &operator*();
    }

    template<typename T, class Buffer>
    typename DataBufferManager<T, Buffer>::Iterator &
    DataBufferManager<T, Buffer>::Iterator::operator++() {
        ++m_index_;
        while (m_index_ < m_manager_->m_entries_.size() && m_available_indices_.count(m_index_)) {
            ++m_index_;
        }
        if (m_index_ >= m_manager_->m_entries_.size()) { m_manager_ = nullptr; }
        return *this;
    }

    template<typename T, class Buffer>
    typename DataBufferManager<T, Buffer>::Iterator
    DataBufferManager<T, Buffer>::Iterator::operator++(int) {
        Iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    template<typename T, class Buffer>
    DataBufferManager<T, Buffer>::ConstIterator::ConstIterator(const DataBufferManager *manager)
        : m_manager_(manager) {
        if (m_manager_ == nullptr) { return; }
        if (m_manager_->m_entries_.empty()) {
            m_manager_ = nullptr;
            return;
        }
        m_available_indices_.insert(
            m_manager_->m_available_indices_.begin(),
            m_manager_->m_available_indices_.end());
        m_index_ = 0;
        while (m_index_ < m_manager_->m_entries_.size() && m_available_indices_.count(m_index_)) {
            ++m_index_;
        }
        if (m_index_ >= m_manager_->m_entries_.size()) {
            m_manager_ = nullptr;
            return;
        }
    }

    template<typename T, class Buffer>
    bool
    DataBufferManager<T, Buffer>::ConstIterator::operator==(const ConstIterator &other) const {
        if (m_manager_ != other.m_manager_) { return false; }
        if (m_manager_ == nullptr) { return true; }
        return m_index_ == other.m_index_;
    }

    template<typename T, class Buffer>
    bool
    DataBufferManager<T, Buffer>::ConstIterator::operator!=(const ConstIterator &other) const {
        return !(*this == other);
    }

    template<typename T, class Buffer>
    const T &
    DataBufferManager<T, Buffer>::ConstIterator::operator*() {
        return m_manager_->m_entries_[m_index_];
    }

    template<typename T, class Buffer>
    const T *
    DataBufferManager<T, Buffer>::ConstIterator::operator->() {
        return &operator*();
    }

    template<typename T, class Buffer>
    typename DataBufferManager<T, Buffer>::ConstIterator &
    DataBufferManager<T, Buffer>::ConstIterator::operator++() {
        ++m_index_;
        while (m_index_ < m_manager_->m_entries_.size() && m_available_indices_.count(m_index_)) {
            ++m_index_;
        }
        if (m_index_ >= m_manager_->m_entries_.size()) { m_manager_ = nullptr; }
        return *this;
    }

    template<typename T, class Buffer>
    typename DataBufferManager<T, Buffer>::ConstIterator
    DataBufferManager<T, Buffer>::ConstIterator::operator++(int) {
        Iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    template<typename T, class Buffer>
    std::size_t
    DataBufferManager<T, Buffer>::Size() const {
        return m_entries_.size() - m_available_indices_.size();
    }

    template<typename T, class Buffer>
    void
    DataBufferManager<T, Buffer>::Reserve(const std::size_t size) {
        m_entries_.reserve(size);
    }

    template<typename T, class Buffer>
    std::size_t
    DataBufferManager<T, Buffer>::AddEntry(T &&entry) {
        if (m_available_indices_.empty()) {
            m_entries_.emplace_back(entry);
            return m_entries_.size() - 1;
        }

        std::size_t index = m_available_indices_.back();
        m_available_indices_.pop_back();
        ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
        m_entries_[index] = std::move(entry);
        return index;
    }

    template<typename T, class Buffer>
    std::size_t
    DataBufferManager<T, Buffer>::AddEntry(const T &entry) {
        return AddEntry(T(entry));
    }

    template<typename T, class Buffer>
    template<typename... Args>
    std::size_t
    DataBufferManager<T, Buffer>::AddEntry(Args &&...args) {
        return AddEntry(T(std::forward<Args>(args)...));
    }

    // enable if T has a default constructor
    template<typename T, class Buffer>
    std::enable_if_t<std::is_default_constructible_v<T>, std::pair<std::size_t, T &>>
    DataBufferManager<T, Buffer>::AllocateEntry() {
        if (m_available_indices_.empty()) {
            m_entries_.emplace_back(T());  // default construct a new entry
            return m_entries_.size() - 1;
        }
        std::size_t index = m_available_indices_.back();
        m_available_indices_.pop_back();
        ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
        return {index, m_entries_[index]};
    }

    template<typename T, class Buffer>
    void
    DataBufferManager<T, Buffer>::RemoveEntry(const std::size_t index) {
        ERL_DEBUG_ASSERT(index != static_cast<std::size_t>(-1), "Index is invalid.");
        ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
        m_available_indices_.push_back(index);
        ERL_DEBUG_ASSERT(
            std::unordered_set(m_available_indices_.begin(), m_available_indices_.end()).size() ==
                m_available_indices_.size(),
            "Redundant indices");
    }

    template<typename T, class Buffer>
    T &
    DataBufferManager<T, Buffer>::operator[](const std::size_t index) {
        ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
        return m_entries_[index];
    }

    template<typename T, class Buffer>
    const T &
    DataBufferManager<T, Buffer>::operator[](const std::size_t index) const {
        ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
        return m_entries_[index];
    }

    template<typename T, class Buffer>
    bool
    DataBufferManager<T, Buffer>::operator==(const DataBufferManager &other) const {
        if (m_entries_.size() - m_available_indices_.size() !=
            other.m_entries_.size() - other.m_available_indices_.size()) {
            return false;
        }
        const std::unordered_set available_indices(
            m_available_indices_.begin(),
            m_available_indices_.end());
        for (std::size_t i = 0; i < m_entries_.size(); ++i) {
            if (!available_indices.count(i)) { continue; }
            if (i >= other.m_entries_.size() || m_entries_[i] != other.m_entries_[i]) {
                return false;
            }
        }
        return true;
    }

    template<typename T, class Buffer>
    bool
    DataBufferManager<T, Buffer>::operator!=(const DataBufferManager &other) const {
        return !(*this == other);
    }

    template<typename T, class Buffer>
    const Buffer &
    DataBufferManager<T, Buffer>::GetBuffer() const {
        return m_entries_;
    }

    template<typename T, class Buffer>
    const std::vector<std::size_t> &
    DataBufferManager<T, Buffer>::GetAvailableIndices() const {
        return m_available_indices_;
    }

    template<typename T, class Buffer>
    void
    DataBufferManager<T, Buffer>::Clear() {
        m_entries_.clear();
        m_available_indices_.clear();
    }

    template<typename T, class Buffer>
    std::unordered_map<std::size_t, std::size_t>
    DataBufferManager<T, Buffer>::Compact() {
        std::unordered_map<std::size_t, std::size_t> index_mapping;
        if (m_available_indices_.empty()) {
            for (std::size_t i = 0; i < m_entries_.size(); ++i) { index_mapping[i] = i; }
            return index_mapping;
        }
        std::sort(m_available_indices_.begin(), m_available_indices_.end());
        std::size_t remove_idx = 0;
        std::size_t write_idx = 0;
        for (std::size_t read_idx = 0; read_idx < m_entries_.size(); ++read_idx) {
            if (remove_idx < m_available_indices_.size() &&
                read_idx == m_available_indices_[remove_idx]) {
                ++remove_idx;
                continue;
            }
            index_mapping[read_idx] = write_idx;
            m_entries_[write_idx++] = std::move(m_entries_[read_idx]);
        }
        m_entries_.resize(write_idx);
        m_available_indices_.clear();
        return index_mapping;
    }

    template<typename T, class Buffer>
    bool
    DataBufferManager<T, Buffer>::Write(std::ostream &s) const {
        static const TokenWriteFunctionPairs<DataBufferManager> token_function_pairs = {
            {
                "entries",
                [](const DataBufferManager *self, std::ostream &stream) {
                    const std::size_t size = self->m_entries_.size();
                    stream.write(reinterpret_cast<const char *>(&size), sizeof(size));
                    const T *entry = self->m_entries_.data();
                    for (std::size_t i = 0; i < size; ++i, ++entry) {
                        if (!Writer<T>::Run(entry, stream)) {
                            ERL_WARN("Failed to write entry {}.", i);
                            return false;
                        }
                    }
                    return true;
                },
            },
            {
                "available_indices",
                [](const DataBufferManager *self, std::ostream &stream) {
                    const std::size_t size = self->m_available_indices_.size();
                    stream.write(reinterpret_cast<const char *>(&size), sizeof(size));
                    for (const auto &index: self->m_available_indices_) {
                        stream.write(reinterpret_cast<const char *>(&index), sizeof(index));
                    }
                    return true;
                },
            },
        };
        return common::WriteTokens(s, this, token_function_pairs);
    }

    template<typename T, class Buffer>
    bool
    DataBufferManager<T, Buffer>::Read(std::istream &s) {
        static const TokenReadFunctionPairs<DataBufferManager> token_function_pairs = {
            {
                "entries",
                [](DataBufferManager *self, std::istream &stream) {
                    std::size_t size;
                    stream.read(reinterpret_cast<char *>(&size), sizeof(size));
                    self->m_entries_.resize(size);
                    T *entry = self->m_entries_.data();
                    for (std::size_t i = 0; i < size; ++i, ++entry) {
                        if (!Reader<T>::Run(entry, stream)) {
                            ERL_WARN("Failed to read entry {}.", i);
                            return false;
                        }
                    }
                    return true;
                },
            },
            {
                "available_indices",
                [](DataBufferManager *self, std::istream &stream) {
                    std::size_t size;
                    stream.read(reinterpret_cast<char *>(&size), sizeof(size));
                    self->m_available_indices_.resize(size);
                    for (std::size_t i = 0; i < size; ++i) {
                        stream.read(
                            reinterpret_cast<char *>(&self->m_available_indices_[i]),
                            sizeof(std::size_t));
                    }
                    return true;
                },
            },
        };
        return common::ReadTokens(s, this, token_function_pairs);
    }

    template<typename T, class Buffer>
    typename DataBufferManager<T, Buffer>::Iterator
    DataBufferManager<T, Buffer>::begin() {
        return Iterator(this);
    }

    template<typename T, class Buffer>
    typename DataBufferManager<T, Buffer>::Iterator
    DataBufferManager<T, Buffer>::end() {
        return Iterator(nullptr);
    }

    template<typename T, class Buffer>
    typename DataBufferManager<T, Buffer>::ConstIterator
    DataBufferManager<T, Buffer>::begin() const {
        return ConstIterator(this);
    }

    template<typename T, class Buffer>
    typename DataBufferManager<T, Buffer>::ConstIterator
    DataBufferManager<T, Buffer>::end() const {
        return ConstIterator(nullptr);
    }

}  // namespace erl::common

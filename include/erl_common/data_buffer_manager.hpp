#pragma once

#include "erl_common/logging.hpp"

#include <unordered_set>
#include <vector>

namespace erl::common {

    template<typename T, class Buffer = std::vector<T>>
    class DataBufferManager {
    public:
        using DataBuffer = Buffer;

    private:
        DataBuffer m_entries_;
        std::vector<std::size_t> m_available_indices_;

    public:
        DataBufferManager() = default;

        DataBufferManager(const DataBufferManager &) = default;
        DataBufferManager &
        operator=(const DataBufferManager &) = default;
        DataBufferManager(DataBufferManager &&) = default;
        DataBufferManager &
        operator=(DataBufferManager &&) = default;

        [[nodiscard]] std::size_t
        Size() const {
            return m_entries_.size() - m_available_indices_.size();
        }

        void
        Reserve(const std::size_t size) {
            m_entries_.reserve(size);
        }

        [[nodiscard]] std::size_t
        AddEntry(T &&entry) {
            if (m_available_indices_.empty()) {
                m_entries_.emplace_back(entry);
                return m_entries_.size() - 1;
            }

            std::size_t index = m_available_indices_.back();
            m_available_indices_.pop_back();
            ERL_ASSERTM(index < m_entries_.size(), "Index {} is out of range.", index);
            m_entries_[index] = std::move(entry);
            return index;
        }

        std::size_t
        AddEntry(const T &entry) {
            return AddEntry(T(entry));
        }

        template<typename... Args>
        [[nodiscard]] std::size_t
        AddEntry(Args &&...args) {
            return AddEntry(T(std::forward<Args>(args)...));
        }

        void
        RemoveEntry(const std::size_t index) {
            ERL_ASSERTM(index != static_cast<std::size_t>(-1), "Index is invalid.");
            ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
            m_available_indices_.push_back(index);
            ERL_DEBUG_ASSERT(
                std::unordered_set(m_available_indices_.begin(), m_available_indices_.end()).size() == m_available_indices_.size(),
                "Redundant indices");
        }

        T &
        operator[](const std::size_t index) {
            ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
            return m_entries_[index];
        }

        const T &
        operator[](const std::size_t index) const {
            ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
            return m_entries_[index];
        }

        const Buffer &
        GetEntries() const {
            return m_entries_;
        }

        void
        Clear() {
            m_entries_.clear();
            m_available_indices_.clear();
        }

        std::unordered_map<std::size_t, std::size_t>
        Compact() {
            std::unordered_map<std::size_t, std::size_t> index_mapping;
            std::size_t new_index = 0;
            const std::unordered_set available_indices(m_available_indices_.begin(), m_available_indices_.end());
            for (std::size_t i = 0; i < m_entries_.size(); ++i) {
                if (!available_indices.count(i)) {
                    index_mapping[i] = new_index;
                    m_entries_[new_index] = m_entries_[i];
                    ++new_index;
                }
            }

            m_entries_.resize(new_index);
            m_available_indices_.clear();
            return index_mapping;
        }
    };
}  // namespace erl::common

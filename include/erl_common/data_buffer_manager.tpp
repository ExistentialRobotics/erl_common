#pragma once

#include "serialization.hpp"

namespace erl::common {
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
        ERL_ASSERTM(index < m_entries_.size(), "Index {} is out of range.", index);
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
        const std::set available_indices(m_available_indices_.begin(), m_available_indices_.end());
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
    DataBufferManager<T, Buffer>::GetEntries() const {
        return m_entries_;
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
        std::size_t new_index = 0;
        const std::unordered_set available_indices(
            m_available_indices_.begin(),
            m_available_indices_.end());
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

    template<typename T, class Buffer>
    bool
    DataBufferManager<T, Buffer>::Write(std::ostream &s) const {
        static const std::vector<
            std::pair<const char *, std::function<bool(const DataBufferManager *, std::ostream &)>>>
            token_function_pairs = {
                {
                    "entries",
                    [](const DataBufferManager *self, std::ostream &stream) {
                        const std::size_t size = self->m_entries_.size();
                        stream.write(reinterpret_cast<const char *>(&size), sizeof(size));
                        for (const auto &entry: self->m_entries_) {
                            if (!entry.Write(stream)) {
                                ERL_WARN("Failed to write entry.");
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
        static const std::vector<
            std::pair<const char *, std::function<bool(DataBufferManager *, std::istream &)>>>
            token_function_pairs = {
                {
                    "entries",
                    [](DataBufferManager *self, std::istream &stream) {
                        std::size_t size;
                        stream.read(reinterpret_cast<char *>(&size), sizeof(size));
                        self->m_entries_.resize(size);
                        for (std::size_t i = 0; i < size; ++i) {
                            if (!self->m_entries_[i].Read(stream)) {
                                ERL_WARN("Failed to read entry.");
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

}  // namespace erl::common

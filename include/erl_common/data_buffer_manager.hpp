#pragma once

#include "logging.hpp"
#include "serialization.hpp"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace erl::common {

    template<typename T, class Buffer = std::vector<T>>
    class DataBufferManager {
    public:
        using DataBuffer = Buffer;

        class Iterator {
            DataBufferManager *m_manager_;
            std::unordered_set<std::size_t> m_available_indices_;
            std::size_t m_index_ = 0;

        public:
            explicit Iterator(DataBufferManager *manager)
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
                while (m_index_ < m_manager_->m_entries_.size() &&
                       m_available_indices_.count(m_index_)) {
                    ++m_index_;
                }
                if (m_index_ >= m_manager_->m_entries_.size()) {
                    m_manager_ = nullptr;
                    return;
                }
            }

            Iterator(const Iterator &other) = default;
            Iterator &
            operator=(const Iterator &other) = default;
            Iterator(Iterator &&other) = default;
            Iterator &
            operator=(Iterator &&other) = default;

            [[nodiscard]] bool
            operator==(const Iterator &other) const {
                if (m_manager_ != other.m_manager_) { return false; }
                if (m_manager_ == nullptr) { return true; }
                return m_index_ == other.m_index_;
            }

            [[nodiscard]] bool
            operator!=(const Iterator &other) const {
                return !(*this == other);
            }

            T &
            operator*() {
                return m_manager_->m_entries_[m_index_];
            }

            T *
            operator->() {
                return &operator*();
            }

            Iterator &
            operator++() {
                ++m_index_;
                while (m_index_ < m_manager_->m_entries_.size() &&
                       m_available_indices_.count(m_index_)) {
                    ++m_index_;
                }
                if (m_index_ >= m_manager_->m_entries_.size()) { m_manager_ = nullptr; }
                return *this;
            }

            Iterator
            operator++(int) {
                Iterator tmp = *this;
                ++(*this);
                return tmp;
            }
        };

        class ConstIterator {
            const DataBufferManager *m_manager_;
            std::unordered_set<std::size_t> m_available_indices_;
            std::size_t m_index_ = 0;

        public:
            explicit ConstIterator(const DataBufferManager *manager)
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
                while (m_index_ < m_manager_->m_entries_.size() &&
                       m_available_indices_.count(m_index_)) {
                    ++m_index_;
                }
                if (m_index_ >= m_manager_->m_entries_.size()) {
                    m_manager_ = nullptr;
                    return;
                }
            }

            ConstIterator(const ConstIterator &other) = default;
            ConstIterator &
            operator=(const ConstIterator &other) = default;
            ConstIterator(ConstIterator &&other) = default;
            ConstIterator &
            operator=(ConstIterator &&other) = default;

            [[nodiscard]] bool
            operator==(const ConstIterator &other) const {
                if (m_manager_ != other.m_manager_) { return false; }
                if (m_manager_ == nullptr) { return true; }
                return m_index_ == other.m_index_;
            }

            [[nodiscard]] bool
            operator!=(const ConstIterator &other) const {
                return !(*this == other);
            }

            const T &
            operator*() {
                return m_manager_->m_entries_[m_index_];
            }

            const T *
            operator->() {
                return &operator*();
            }

            ConstIterator &
            operator++() {
                ++m_index_;
                while (m_index_ < m_manager_->m_entries_.size() &&
                       m_available_indices_.count(m_index_)) {
                    ++m_index_;
                }
                if (m_index_ >= m_manager_->m_entries_.size()) { m_manager_ = nullptr; }
                return *this;
            }

            ConstIterator
            operator++(int) {
                Iterator tmp = *this;
                ++(*this);
                return tmp;
            }
        };

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
        Reserve(std::size_t size) {
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
            ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
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

        std::enable_if_t<std::is_default_constructible_v<T>, std::pair<std::size_t, T &>>
        AllocateEntry() {
            if (m_available_indices_.empty()) {
                m_entries_.emplace_back(T());  // default construct a new entry
                return m_entries_.size() - 1;
            }
            std::size_t index = m_available_indices_.back();
            m_available_indices_.pop_back();
            ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
            return {index, m_entries_[index]};
        }

        void
        RemoveEntry(const std::size_t index) {
            ERL_DEBUG_ASSERT(index != static_cast<std::size_t>(-1), "Index is invalid.");
            ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
            m_available_indices_.push_back(index);
            ERL_DEBUG_ASSERT(
                std::unordered_set(m_available_indices_.begin(), m_available_indices_.end())
                        .size() == m_available_indices_.size(),
                "Redundant indices");
        }

        T &
        operator[](std::size_t index) {
            ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
            return m_entries_[index];
        }

        const T &
        operator[](std::size_t index) const {
            ERL_DEBUG_ASSERT(index < m_entries_.size(), "Index {} is out of range.", index);
            return m_entries_[index];
        }

        [[nodiscard]] bool
        operator==(const DataBufferManager &other) const {
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

        [[nodiscard]] bool
        operator!=(const DataBufferManager &other) const {
            return !(*this == other);
        }

        const Buffer &
        GetBuffer() const {
            return m_entries_;
        }

        [[nodiscard]] const std::vector<std::size_t> &
        GetAvailableIndices() const {
            return m_available_indices_;
        }

        void
        Clear() {
            m_entries_.clear();
            m_available_indices_.clear();
        }

        std::unordered_map<std::size_t, std::size_t>
        Compact() {
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

        [[nodiscard]] bool
        Write(std::ostream &s) const {
            using namespace serialization;
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
            return WriteTokens(s, this, token_function_pairs);
        }

        [[nodiscard]] bool
        Read(std::istream &s) {
            using namespace serialization;
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
            return ReadTokens(s, this, token_function_pairs);
        }

        Iterator
        begin() {
            return Iterator(this);
        }

        Iterator
        end() {
            return Iterator(nullptr);
        }

        ConstIterator
        begin() const {
            return ConstIterator(this);
        }

        ConstIterator
        end() const {
            return ConstIterator(nullptr);
        }
    };
}  // namespace erl::common

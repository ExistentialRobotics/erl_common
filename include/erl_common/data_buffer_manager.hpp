#pragma once

#include "erl_common/logging.hpp"

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
        Size() const;

        void
        Reserve(std::size_t size);

        [[nodiscard]] std::size_t
        AddEntry(T &&entry);

        std::size_t
        AddEntry(const T &entry);

        template<typename... Args>
        [[nodiscard]] std::size_t
        AddEntry(Args &&...args);

        void
        RemoveEntry(std::size_t index);

        T &
        operator[](std::size_t index);

        const T &
        operator[](std::size_t index) const;

        [[nodiscard]] bool
        operator==(const DataBufferManager &other) const;

        [[nodiscard]] bool
        operator!=(const DataBufferManager &other) const;

        const Buffer &
        GetEntries() const;

        void
        Clear();

        std::unordered_map<std::size_t, std::size_t>
        Compact();

        [[nodiscard]] bool
        Write(std::ostream &s) const;

        [[nodiscard]] bool
        Read(std::istream &s);
    };
}  // namespace erl::common

#include "data_buffer_manager.tpp"

#pragma once

#include "logging.hpp"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace erl::common {

    template<typename T, class Buffer = std::vector<T>>
    class DataBufferManager {
    public:
        template<typename, typename = void>
        struct Writer {
            static bool
            Run(const T *entry, std::ostream &stream);
        };

        template<typename C>
        struct Writer<C, std::void_t<decltype(std::declval<C>().Write())>> {
            static bool
            Run(const T *entry, std::ostream &stream);
        };

        template<typename, typename = void>
        struct Reader {
            static bool
            Run(T *entry, std::istream &stream);
        };

        template<typename C>
        struct Reader<C, std::void_t<decltype(std::declval<C>().Read())>> {
            static bool
            Run(T *entry, std::istream &stream);
        };

        using DataBuffer = Buffer;

        class Iterator {
            DataBufferManager *m_manager_;
            std::unordered_set<std::size_t> m_available_indices_;
            std::size_t m_index_ = 0;

        public:
            explicit Iterator(DataBufferManager *manager);

            Iterator(const Iterator &other) = default;
            Iterator &
            operator=(const Iterator &other) = default;
            Iterator(Iterator &&other) = default;
            Iterator &
            operator=(Iterator &&other) = default;

            [[nodiscard]] bool
            operator==(const Iterator &other) const;

            [[nodiscard]] bool
            operator!=(const Iterator &other) const;

            T &
            operator*();

            T *
            operator->();

            Iterator &
            operator++();

            Iterator
            operator++(int);
        };

        class ConstIterator {
            const DataBufferManager *m_manager_;
            std::unordered_set<std::size_t> m_available_indices_;
            std::size_t m_index_ = 0;

        public:
            explicit ConstIterator(const DataBufferManager *manager);

            ConstIterator(const ConstIterator &other) = default;
            ConstIterator &
            operator=(const ConstIterator &other) = default;
            ConstIterator(ConstIterator &&other) = default;
            ConstIterator &
            operator=(ConstIterator &&other) = default;

            [[nodiscard]] bool
            operator==(const ConstIterator &other) const;

            [[nodiscard]] bool
            operator!=(const ConstIterator &other) const;

            const T &
            operator*();

            const T *
            operator->();

            ConstIterator &
            operator++();

            ConstIterator
            operator++(int);
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

        std::enable_if_t<std::is_default_constructible_v<T>, std::pair<std::size_t, T &>>
        AllocateEntry();

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
        GetBuffer() const;

        [[nodiscard]] const std::vector<std::size_t> &
        GetAvailableIndices() const;

        void
        Clear();

        std::unordered_map<std::size_t, std::size_t>
        Compact();

        [[nodiscard]] bool
        Write(std::ostream &s) const;

        [[nodiscard]] bool
        Read(std::istream &s);

        Iterator
        begin();

        Iterator
        end();

        ConstIterator
        begin() const;

        ConstIterator
        end() const;
    };
}  // namespace erl::common

#include "data_buffer_manager.tpp"

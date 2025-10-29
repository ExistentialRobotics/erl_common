#pragma once

#include "serialization.hpp"

#include <cstddef>
#include <initializer_list>
#include <optional>
#include <vector>

namespace erl::common {

    template<typename T, int N = -1, class Allocator = std::allocator<T>>
    class RingBuffer {
        static_assert(N != 0, "N cannot be 0");

        using Storage = std::conditional_t<
            (N > 0),
            T[(N > 0 ? N : 1)],        // static array if N > 0
            std::vector<T, Allocator>  // dynamic vector if N == -1
            >;

        Storage m_buffer_;                          // static array or dynamic vector
        bool m_reject_on_full_ = false;             // whether to reject new data when full
        std::size_t m_write_ = 0;                   // write position
        std::size_t m_read_ = 0;                    // read position
        std::size_t m_size_ = 0;                    // current size
        std::size_t m_capacity_ = (N > 0 ? N : 0);  // capacity

    public:
        /**
         * Constructs a RingBuffer with static Capacity N.
         */
        RingBuffer() {
            static_assert(N > 0, "Dynamic RingBuffer requires Capacity in constructor");
            m_capacity_ = N;
        }

        /**
         * Constructs a RingBuffer with dynamic Capacity.
         * @param capacity The capacity of the RingBuffer.
         */
        explicit RingBuffer(std::size_t capacity)
            : m_buffer_(capacity), m_capacity_(capacity) {
            static_assert(N == -1, "Static RingBuffer does not take Capacity in constructor");
        }

        [[nodiscard]] bool
        RejectOnFull() const {
            return m_reject_on_full_;
        }

        void
        RejectOnFull(const bool reject) {
            m_reject_on_full_ = reject;
        }

        const Storage &
        GetBuffer() const {
            return m_buffer_;
        }

        /**
         *
         * @param val Value to push into the ring buffer.
         */
        void
        Push(const T &val) {
            if (m_reject_on_full_ && IsFull()) { return; }  // reject new data when full
            m_buffer_[m_write_] = val;
            if (IsFull()) {
                m_read_ = (m_read_ + 1) % m_capacity_;
            } else {
                ++m_size_;
            }
            m_write_ = (m_write_ + 1) % m_capacity_;
        }

        /**
         *
         * @param val Value to push into the ring buffer.
         */
        void
        Push(T &&val) {
            if (m_reject_on_full_ && IsFull()) { return; }  // reject new data when full
            m_buffer_[m_write_] = std::move(val);
            if (IsFull()) {
                m_read_ = (m_read_ + 1) % m_capacity_;
            } else {
                ++m_size_;
            }
            m_write_ = (m_write_ + 1) % m_capacity_;
        }

        /**
         *
         * @tparam It Iterator type.
         * @param first The first iterator of the range to push.
         * @param last The last iterator (excluded) of the range to push.
         * @return The number of elements pushed.
         */
        template<typename It>
        std::size_t
        PushRange(It first, It last) {
            if (m_reject_on_full_ && IsFull()) { return 0; }  // reject new data when full

            size_t n = std::distance(first, last);
            if (n == 0) { return 0; }

            // The new data exceeds capacity, keep only the last 'm_capacity_' elements
            if (n >= m_capacity_) {
                std::advance(first, n - m_capacity_);  // in-place advance
                n = m_capacity_;
                if (m_reject_on_full_) {  // skip some elements
                    std::advance(first, m_size_);
                    n -= m_size_;
                }
                std::copy(first, last, GetDataPointer());
                m_write_ = 0;
                m_read_ = 0;
                m_size_ = m_capacity_;
                return n;
            }

            // Calculate how many elements will be overwritten.
            const std::size_t free_space = AvailableSpace();  // m_capacity_ - m_size_
            const std::size_t overwrite_count = (n > free_space) ? (n - free_space) : 0;

            // Check for wrap-around
            std::size_t available_at_end = m_capacity_ - m_write_;
            if (n <= available_at_end) {  // No wrap-around
                std::copy(first, last, GetDataPointer() + m_write_);
            } else {
                // Wrap-around occurs
                It mid = std::next(first, available_at_end);
                std::copy(first, mid, GetDataPointer() + m_write_);
                std::copy(mid, last, GetDataPointer());
            }

            // Update write and read indices and size
            m_write_ = (m_write_ + n) % m_capacity_;
            if (overwrite_count > 0) {
                m_read_ = (m_read_ + overwrite_count) % m_capacity_;
                m_size_ = m_capacity_;  // buffer is full
            } else {
                m_size_ += n;
            }

            return n;
        }

        /**
         * Push multiple values into the ring buffer.
         * @param values list of values to push into the ring buffer
         */
        void
        PushRange(std::initializer_list<T> values) {
            for (const auto &v: values) { Push(v); }
        }

        /**
         *
         * @return An element from the ring buffer. std::nullopt if the buffer is empty.
         */
        std::optional<T>
        Pop() {
            if (IsEmpty()) { return std::nullopt; }
            T item = std::move(m_buffer_[m_read_]);
            m_read_ = (m_read_ + 1) % m_capacity_;
            --m_size_;
            return item;
        }

        std::size_t
        PopAll(std::vector<T> &output) {
            return PopRange(output, m_size_);
        }

        std::size_t
        PopRange(std::vector<T> &output, const std::size_t n) {
            output.reserve(output.size() + m_size_);
            auto dest = std::back_inserter(output);
            return PopRange<decltype(dest)>(dest, n);
        }

        template<typename OutputIt>
        std::size_t
        PopRange(OutputIt dest, const std::size_t n) {
            const std::size_t actual_count = std::min(n, m_size_);
            if (actual_count == 0) { return 0; }

            // Check for wrap-around
            const std::size_t available_to_end = m_capacity_ - m_read_;
            std::size_t part1_count = std::min(actual_count, available_to_end);
            std::size_t part2_count = actual_count - part1_count;
            T *data_ptr = GetDataPointer() + m_read_;
            dest = std::copy(data_ptr, data_ptr + part1_count, dest);
            if (part2_count > 0) {
                // Wrap-around occurs
                data_ptr = GetDataPointer();
                std::copy(data_ptr, data_ptr + part2_count, dest);
            }

            // Update read index and size
            m_read_ = (m_read_ + actual_count) % m_capacity_;
            m_size_ -= actual_count;
            return actual_count;
        }

        void
        Resize(std::size_t new_capacity) {
            static_assert(N == -1, "Resize is only available for dynamic RingBuffer");
            if (new_capacity == m_capacity_) { return; }
            if (new_capacity > m_capacity_) {
                Storage new_buffer(new_capacity);
                PopRange(new_buffer.data(), m_size_);
                m_buffer_ = std::move(new_buffer);
                m_write_ = m_size_;
                m_read_ = 0;
                m_capacity_ = new_capacity;
                return;
            }
            // new_capacity < m_capacity_
            Storage new_buffer(new_capacity);
            std::size_t elements_to_copy = std::min(m_size_, new_capacity);
            if (m_size_ > elements_to_copy) {  // need to drop some elements
                PopRange(new_buffer.data(), m_size_ - elements_to_copy);
            }
            PopRange(new_buffer.data(), elements_to_copy);
            m_buffer_ = std::move(new_buffer);
            m_write_ = elements_to_copy % new_capacity;
            m_read_ = 0;
            m_size_ = elements_to_copy;
            m_capacity_ = new_capacity;
        }

        void
        Reset() {
            m_write_ = 0;
            m_read_ = 0;
            m_size_ = 0;
        }

        T *
        Front() {
            if (IsEmpty()) { return nullptr; }
            return &m_buffer_[m_read_];
        }

        [[nodiscard]] const T *
        Front() const {
            if (IsEmpty()) { return nullptr; }
            return &m_buffer_[m_read_];
        }

        [[nodiscard]] bool
        IsEmpty() const {
            return m_size_ == 0;
        }

        [[nodiscard]] bool
        IsFull() const {
            return m_size_ == m_capacity_;
        }

        [[nodiscard]] std::size_t
        Size() const {
            return m_size_;
        }

        [[nodiscard]] std::size_t
        Capacity() const {
            return m_capacity_;
        }

        [[nodiscard]] std::size_t
        AvailableSpace() const {
            return m_capacity_ - m_size_;
        }

        [[nodiscard]] bool
        operator==(const RingBuffer &other) const {
            if (m_size_ != other.m_size_) { return false; }
            for (std::size_t i = 0; i < m_size_; ++i) {
                std::size_t index1 = (m_read_ + i) % m_capacity_;
                std::size_t index2 = (other.m_read_ + i) % other.m_capacity_;
                if (m_buffer_[index1] != other.m_buffer_[index2]) { return false; }
            }
            return true;
        }

        [[nodiscard]] bool
        operator!=(const RingBuffer &other) const {
            return !(*this == other);
        }

        [[nodiscard]] bool
        Write(std::ostream &stream) const {
            using namespace common::serialization;
            static const TokenWriteFunctionPairs<RingBuffer> token_function_pairs = {
                {
                    "info",
                    [](const RingBuffer *self, std::ostream &s) {
                        const bool &rej_on_full = self->m_reject_on_full_;
                        const std::size_t &write = self->m_write_;
                        const std::size_t &read = self->m_read_;
                        const std::size_t &size = self->m_size_;
                        const std::size_t &capacity = self->m_capacity_;
                        s.write(reinterpret_cast<const char *>(&rej_on_full), sizeof(rej_on_full));
                        s.write(reinterpret_cast<const char *>(&write), sizeof(write));
                        s.write(reinterpret_cast<const char *>(&read), sizeof(read));
                        s.write(reinterpret_cast<const char *>(&size), sizeof(size));
                        s.write(reinterpret_cast<const char *>(&capacity), sizeof(capacity));
                        return s.good();
                    },
                },
                {
                    "buffer",
                    [](const RingBuffer *self, std::ostream &s) {
                        for (std::size_t i = 0; i < self->m_size_; ++i) {
                            std::size_t index = (self->m_read_ + i) % self->m_capacity_;
                            if (!Writer<T>::Run(&self->m_buffer_[index], s)) {
                                ERL_WARN("Failed to write item {}.", i);
                                return false;
                            }
                        }
                        return s.good();
                    },
                },
            };
            return WriteTokens(stream, this, token_function_pairs);
        }

        [[nodiscard]] bool
        Read(std::istream &stream) {
            using namespace common::serialization;
            static const TokenReadFunctionPairs<RingBuffer> token_function_pairs = {
                {
                    "info",
                    [](RingBuffer *self, std::istream &s) {
                        bool &rej_on_full = self->m_reject_on_full_;
                        std::size_t &write = self->m_write_;
                        std::size_t &read = self->m_read_;
                        std::size_t &size = self->m_size_;
                        std::size_t &capacity = self->m_capacity_;
                        s.read(reinterpret_cast<char *>(&rej_on_full), sizeof(rej_on_full));
                        s.read(reinterpret_cast<char *>(&write), sizeof(write));
                        s.read(reinterpret_cast<char *>(&read), sizeof(read));
                        s.read(reinterpret_cast<char *>(&size), sizeof(size));
                        s.read(reinterpret_cast<char *>(&capacity), sizeof(capacity));
                        return s.good();
                    },
                },
                {
                    "buffer",
                    [](RingBuffer *self, std::istream &s) {
                        self->m_buffer_.clear();
                        self->m_buffer_.resize(self->m_capacity_);
                        for (std::size_t i = 0; i < self->m_size_; ++i) {
                            std::size_t index = (self->m_read_ + i) % self->m_capacity_;
                            if (!Reader<T>::Run(&self->m_buffer_[index], s)) {
                                ERL_WARN("Failed to read item {}.", i);
                                return false;
                            }
                        }
                        return s.good();
                    },
                },
            };
            return ReadTokens(stream, this, token_function_pairs);
        }

    private:
        T *
        GetDataPointer() {
            if constexpr (N > 0) {
                return m_buffer_;
            } else {
                return m_buffer_.data();
            }
        }
    };
}  // namespace erl::common

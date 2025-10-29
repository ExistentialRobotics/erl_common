#include "erl_common/ring_buffer.hpp"
#include "erl_common/test_helper.hpp"

#include <iostream>
#include <numeric>  // For std::iota
#include <vector>

// Helper function to print the buffer's state
template<typename T, int N>
void
PrintBufferState(const erl::common::RingBuffer<T, N> &buffer) {
    std::cout << "Buffer (Size: " << buffer.Size()                  //
              << ", Capacity: " << buffer.Capacity()                //
              << ", Empty: " << std::boolalpha << buffer.IsEmpty()  //
              << ", Full: " << std::boolalpha << buffer.IsFull() << ")" << std::endl;
}

// Helper function to pop and print all elements using PopRange
template<typename T, int N>
void
DrainBuffer(erl::common::RingBuffer<T, N> &buffer) {
    std::cout << "Draining buffer: ";
    std::vector<int> popped_data(buffer.Size());
    std::size_t popped_size = buffer.PopRange(popped_data.begin(), popped_data.size());

    ASSERT_EQ(popped_size, popped_data.size());

    for (size_t i = 0; i < popped_data.size(); ++i) {
        std::cout << popped_data[i] << ((i == popped_data.size() - 1) ? "" : ", ");
    }
    std::cout << std::endl;
}

TEST(RingBufferTest, BasicOperations) {
    using namespace erl::common;

    RingBuffer<int, 5> buffer;
    PrintBufferState(buffer);

    // Push elements
    for (int i = 1; i <= 3; ++i) {
        buffer.Push(i);
        PrintBufferState(buffer);
    }
    std::cout << fmt::format("{}", buffer.GetBuffer()) << std::endl;

    // Pop elements
    for (int i = 0; i < 2; ++i) {
        const int val = buffer.Pop().value();
        std::cout << "Popped: " << val << std::endl;
        PrintBufferState(buffer);
    }

    // Push more elements
    for (int i = 4; i <= 6; ++i) {
        buffer.Push(i);
        PrintBufferState(buffer);
    }
    std::cout << fmt::format("{}", buffer.GetBuffer()) << std::endl;

    // Drain remaining elements
    DrainBuffer(buffer);

    buffer.RejectOnFull(true);
    // Try to push when full
    for (int i = 1; i <= 5; ++i) { buffer.Push(i); }
    PrintBufferState(buffer);
    std::cout << fmt::format("{}", buffer.GetBuffer()) << std::endl;
    buffer.Push(6);  // This should be rejected
    PrintBufferState(buffer);
    DrainBuffer(buffer);
}

TEST(RingBufferTest, Overwrite) {
    using namespace erl::common;

    RingBuffer<int, -1> buffer(5);
    PrintBufferState(buffer);
    // Fill the buffer
    for (int i = 1; i <= 5; ++i) { buffer.Push(i); }
    PrintBufferState(buffer);
    DrainBuffer(buffer);  // Should print: 1, 2, 3, 4, 5
}

TEST(RingBufferTest, PushMany) {
    using namespace erl::common;

    RingBuffer<int, -1> buffer(5);
    PrintBufferState(buffer);

    buffer.PushRange({1, 2, 3});
    PrintBufferState(buffer);  // Size 3
    DrainBuffer(buffer);       // Should print: 1, 2, 3

    buffer.PushRange({1, 2, 3, 4, 5, 6});
    PrintBufferState(buffer);
    DrainBuffer(buffer);  // Should print: 2, 3, 4, 5, 6
}

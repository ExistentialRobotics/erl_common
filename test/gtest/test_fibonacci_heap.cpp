#include "erl_common/fibonacci_heap.hpp"
#include "erl_common/test_helper.hpp"
#include "erl_common/random.hpp"
#include <vector>

TEST(ERL_COMMON, FibonacciHeapSeq) {
    using namespace erl::common;

    int num_of_nodes = 100000;
    std::vector<int> keys(num_of_nodes);
    std::iota(keys.begin(), keys.end(), num_of_nodes);
    std::shuffle(keys.begin(), keys.end(), g_random_engine);
    // test constructor
    FibonacciHeap<int, std::less<int>> heap;
    // test insert
    std::vector<FibonacciHeap<int, std::less<int>>::Node::Ptr> nodes;
    nodes.reserve(num_of_nodes);
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int &key: keys) { nodes.push_back(heap.Insert(key)); }
    auto t1 = std::chrono::high_resolution_clock::now();
    double dt = std::chrono::duration<double, std::micro>(t1 - t0).count() / double(num_of_nodes);
    std::cout << "insert: " << dt << " us per operation." << std::endl;
    // test decrease key
    std::shuffle(nodes.begin(), nodes.end(), g_random_engine);
    t0 = std::chrono::high_resolution_clock::now();
    for (auto &node: nodes) {
        node->key -= num_of_nodes;
        heap.DecreaseKey(node);
    }
    t1 = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration<double, std::micro>(t1 - t0).count() / double(num_of_nodes);
    std::cout << "decrease key: " << dt << " us per operation" << std::endl;
    // test extract min
    int cnt = 0;
    double t = 0;
    while (!heap.IsEmpty()) {
        t0 = std::chrono::high_resolution_clock::now();
        int min_key = heap.ExtractMin();
        t1 = std::chrono::high_resolution_clock::now();
        t += std::chrono::duration<double, std::micro>(t1 - t0).count();
        std::cout << std::setw(8) << std::setfill(' ') << min_key << std::flush;
        cnt++;
        if (cnt % 10 == 0) { std::cout << std::endl; }
    }
    dt = t / double(num_of_nodes);
    std::cout << "extract min: " << dt << " us per operation" << std::endl;
}

TEST(ERL_COMMON, FibonacciHeapRandom) {
    using namespace erl::common;

    constexpr int kMaxInsertionsPerIter = 10;
    constexpr int kMaxDecreaseKeysPerIter = 5;
    constexpr int kNumIters = 20000;
    constexpr int kMaxKeyVal = 1000000;

    double t_insert = 0;
    double t_extract_min = 0;
    double t_decrease_key = 0;
    long n_insert = 0;
    long n_extract_min = 0;
    long n_decrease_key = 0;

    std::uniform_int_distribution<int> num_insertions_rng(0, kMaxInsertionsPerIter);
    std::uniform_int_distribution<int> num_decrease_keys_rng(0, kMaxDecreaseKeysPerIter);
    std::uniform_int_distribution<int> num_key_val_rng(0, kMaxKeyVal);

    g_random_engine.seed(21);
    using Heap = FibonacciHeap<int, std::less<int>>;
    Heap heap;
    std::vector<Heap::Node::Ptr> nodes;
    for (int i = 0; i < kNumIters; ++i) {
        int num_insertions = num_insertions_rng(g_random_engine);
        int num_decrease_keys = num_decrease_keys_rng(g_random_engine);
        std::vector<bool> ops(num_insertions + num_decrease_keys, false);
        for (int j = 0; j < num_insertions; ++j) { ops[j] = true; }
        std::shuffle(ops.begin(), ops.end(), g_random_engine);
        for (bool op: ops) {
            if (op || heap.IsEmpty()) {
                int key = num_key_val_rng(g_random_engine);
                auto t0 = std::chrono::high_resolution_clock::now();
                auto node = heap.Insert(key);
                auto t1 = std::chrono::high_resolution_clock::now();
                t_insert += std::chrono::duration<double, std::micro>(t1 - t0).count();
                n_insert++;
                nodes.push_back(node);
            } else {
                std::shuffle(nodes.begin(), nodes.end(), g_random_engine);
                auto node = nodes.back();
                node->key -= 1;
                auto t0 = std::chrono::high_resolution_clock::now();
                heap.DecreaseKey(node);
                auto t1 = std::chrono::high_resolution_clock::now();
                t_decrease_key += std::chrono::duration<double, std::micro>(t1 - t0).count();
                n_decrease_key++;
            }
        }

        if (!heap.IsEmpty()) {
            auto itr = std::find(nodes.begin(), nodes.end(), heap.MinNode());
            ERL_ASSERTM(itr != nodes.end(), "Min node not found in nodes.");
            std::swap(*itr, nodes.back());
            nodes.pop_back();

            auto t0 = std::chrono::high_resolution_clock::now();
            heap.ExtractMin();
            auto t1 = std::chrono::high_resolution_clock::now();
            t_extract_min += std::chrono::duration<double, std::micro>(t1 - t0).count();
            n_extract_min++;
        }

        // std::cout << "iter: " << i << std::endl
        //           << "insert: " << t_insert / double(n_insert) << " us per operation" << std::endl
        //           << "decrease key: " << t_decrease_key / double(n_decrease_key) << " us per operation" << std::endl
        //           << "extract min: " << t_extract_min / double(n_extract_min) << " us per operation" << std::endl
        //           << "n_insert: " << n_insert << std::endl
        //           << "n_decrease_key: " << n_decrease_key << std::endl
        //           << "n_extract_min: " << n_extract_min << std::endl
        //           << "heap size: " << heap.Size() << std::endl
        //           << std::endl;
    }
    std::cout << "insert: " << t_insert / double(n_insert) << " us per operation" << std::endl
              << "decrease key: " << t_decrease_key / double(n_decrease_key) << " us per operation" << std::endl
              << "extract min: " << t_extract_min / double(n_extract_min) << " us per operation" << std::endl
              << "n_insert: " << n_insert << std::endl
              << "n_decrease_key: " << n_decrease_key << std::endl
              << "n_extract_min: " << n_extract_min << std::endl
              << "heap size: " << heap.Size() << std::endl
              << std::endl;
}

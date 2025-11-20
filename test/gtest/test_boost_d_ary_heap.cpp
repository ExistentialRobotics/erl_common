#include "erl_common/random.hpp"
#include "erl_common/test_helper.hpp"

#include <boost/heap/d_ary_heap.hpp>

#include <vector>

TEST(BoostDaryHeap, Seq) {
    using namespace erl::common;
    using Heap = boost::heap::d_ary_heap<
        int,
        boost::heap::mutable_<true>,
        boost::heap::arity<8>,
        boost::heap::compare<std::greater<>>>;

    constexpr int num_of_nodes = 100000;
    std::vector<int> keys(num_of_nodes);
    std::iota(keys.begin(), keys.end(), num_of_nodes);
    std::shuffle(keys.begin(), keys.end(), g_random_engine);
    // test constructor
    Heap heap;
    // test insert
    std::vector<Heap::handle_type> handles;
    handles.reserve(num_of_nodes);
    auto t0 = std::chrono::high_resolution_clock::now();
    std::transform(keys.begin(), keys.end(), std::back_inserter(handles), [&heap](const int key) {
        return heap.push(key);
    });
    auto t1 = std::chrono::high_resolution_clock::now();
    double dt = std::chrono::duration<double, std::micro>(t1 - t0).count() /
                static_cast<double>(num_of_nodes);
    std::cout << "insert: " << dt << " us per operation." << std::endl;
    // test decrease key
    std::shuffle(handles.begin(), handles.end(), g_random_engine);
    t0 = std::chrono::high_resolution_clock::now();
    for (auto &handle: handles) {
        *handle -= num_of_nodes;
        heap.decrease(handle);
    }
    t1 = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration<double, std::micro>(t1 - t0).count() /
         static_cast<double>(num_of_nodes);
    std::cout << "decrease key: " << dt << " us per operation" << std::endl;
    // test extract min
    int cnt = 0;
    double t = 0;
    while (!heap.empty()) {
        t0 = std::chrono::high_resolution_clock::now();
        const int min_key = heap.top();
        heap.pop();
        t1 = std::chrono::high_resolution_clock::now();
        t += std::chrono::duration<double, std::micro>(t1 - t0).count();
        std::cout << std::setw(8) << std::setfill(' ') << min_key << std::flush;
        cnt++;
        if (cnt % 10 == 0) { std::cout << std::endl; }
    }
    dt = t / static_cast<double>(num_of_nodes);
    std::cout << "extract min: " << dt << " us per operation" << std::endl;
}

TEST(BoostDaryHeap, Random) {
    using namespace erl::common;

    constexpr int max_insertions_per_iter = 10;
    constexpr int max_decrease_keys_per_iter = 5;
    constexpr int num_iters = 2000;
    constexpr int max_key_val = 10000;

    double t_insert = 0;
    double t_extract_min = 0;
    double t_decrease_key = 0;
    long n_insert = 0;
    long n_extract_min = 0;
    long n_decrease_key = 0;

    std::uniform_int_distribution num_insertions_rng(0, max_insertions_per_iter);
    std::uniform_int_distribution num_decrease_keys_rng(0, max_decrease_keys_per_iter);
    std::uniform_int_distribution key_val_rng(0, max_key_val);

    struct Greater {
        bool
        operator()(const std::shared_ptr<int> &i1, const std::shared_ptr<int> &i2) const {
            return *i1 > *i2;
        }
    };

    g_random_engine.seed(21);  // NOLINT(*-msc51-cpp)
    using Heap = boost::heap::d_ary_heap<
        std::shared_ptr<int>,
        boost::heap::mutable_<true>,
        boost::heap::arity<8>,
        boost::heap::compare<Greater>>;

    Heap heap;
    std::vector<Heap::handle_type> handles;
    for (int i = 0; i < num_iters; ++i) {
        const int num_insertions = num_insertions_rng(g_random_engine);
        const int num_decrease_keys = num_decrease_keys_rng(g_random_engine);
        std::vector<bool> ops(num_insertions + num_decrease_keys, false);
        for (int j = 0; j < num_insertions; ++j) { ops[j] = true; }
        std::shuffle(ops.begin(), ops.end(), g_random_engine);
        for (const bool op: ops) {
            if (op || heap.empty()) {
                int key = key_val_rng(g_random_engine);
                auto item = std::make_shared<int>(key);

                auto t0 = std::chrono::high_resolution_clock::now();
                auto handle = heap.push(item);
                auto t1 = std::chrono::high_resolution_clock::now();

                t_insert += std::chrono::duration<double, std::micro>(t1 - t0).count();
                n_insert++;
                handles.push_back(handle);
            } else {
                std::shuffle(handles.begin(), handles.end(), g_random_engine);
                auto handle = handles.back();
                **handle -= 1;

                auto t0 = std::chrono::high_resolution_clock::now();
                heap.decrease(handle);
                auto t1 = std::chrono::high_resolution_clock::now();

                t_decrease_key += std::chrono::duration<double, std::micro>(t1 - t0).count();
                n_decrease_key++;
            }
        }

        if (!heap.empty()) {
            auto itr = std::find_if(handles.begin(), handles.end(), [&heap](const auto &handle) {
                return heap.top() == *handle;
            });
            ERL_ASSERTM(itr != handles.end(), "Min node not found in nodes.");
            std::swap(*itr, handles.back());
            handles.pop_back();

            auto t0 = std::chrono::high_resolution_clock::now();
            heap.pop();
            auto t1 = std::chrono::high_resolution_clock::now();
            t_extract_min += std::chrono::duration<double, std::micro>(t1 - t0).count();
            n_extract_min++;
        }
    }
    std::cout << "insert: " << t_insert / static_cast<double>(n_insert) << " us per operation\n"
              << "decrease key: " << t_decrease_key / static_cast<double>(n_decrease_key)
              << " us per operation\n"
              << "extract min: " << t_extract_min / static_cast<double>(n_extract_min)
              << " us per operation\n"
              << "n_insert: " << n_insert << std::endl
              << "n_decrease_key: " << n_decrease_key << std::endl
              << "n_extract_min: " << n_extract_min << std::endl
              << "heap size: " << heap.size() << std::endl
              << std::endl;
}

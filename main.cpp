#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <chrono>
#include <iomanip>
#include <random>

// Utility: measure elapsed time of a lambda function
template <typename Func>
double measure_time(Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end - start).count();
}

// ===============================================================
// 1. Double-ended operations test (push_front/back, pop_front/back)
// ===============================================================
void test_double_ended_operations(size_t operations) {
    std::cout << "\n=== Double-ended operation test (" << operations << " operations) ===" << std::endl;
    
    // deque test
    std::deque<int> dq;
    double dq_time = measure_time([&]() {
        for (size_t i = 0; i < operations; ++i) {
            if (i % 2 == 0)
                dq.push_back(i);   // push at back
            else
                dq.push_front(i);  // push at front
        }
        for (size_t i = 0; i < operations / 2; ++i) {
            dq.pop_back();        // pop at back
            dq.pop_front();       // pop at front
        }
    });

    // vector test (inefficient for front insertions)
    std::vector<int> vec;
    double vec_time = measure_time([&]() {
        for (size_t i = 0; i < operations; ++i) {
            if (i % 2 == 0)
                vec.push_back(i);  // push back
            else
                vec.insert(vec.begin(), i);  // insert at front (slow)
        }
        for (size_t i = 0; i < operations / 2; ++i) {
            vec.pop_back();       // pop back
            vec.erase(vec.begin()); // erase front (slow)
        }
    });

    // list test
    std::list<int> lst;
    double lst_time = measure_time([&]() {
        for (size_t i = 0; i < operations; ++i) {
            if (i % 2 == 0)
                lst.push_back(i);  // push back
            else
                lst.push_front(i); // push front
        }
        for (size_t i = 0; i < operations / 2; ++i) {
            lst.pop_back();       // pop back
            lst.pop_front();      // pop front
        }
    });

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "deque:  " << dq_time << " s\n";
    std::cout << "vector: " << vec_time << " s\n";
    std::cout << "list:   " << lst_time << " s\n";
    std::cout << "vector is " << vec_time / dq_time << "x slower than deque\n";
    std::cout << "list   is " << lst_time / dq_time << "x slower than deque\n";
}

// ======================================
// 2. Random access performance test
// ======================================
void test_random_access(size_t size) {
    std::cout << "\n=== Random access test (" << size << " elements) ===" << std::endl;

    // Prepare random data and RNG
    std::vector<int> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, size - 1);

    // Initialize containers
    std::deque<int> dq(data.begin(), data.end());
    std::vector<int> vec(data.begin(), data.end());
    std::list<int> lst(data.begin(), data.end());

    // deque random access
    double dq_time = measure_time([&]() {
        for (size_t i = 0; i < size; ++i) {
            size_t idx = dist(gen);
            volatile int val = dq[idx];
        }
    });

    // vector random access
    double vec_time = measure_time([&]() {
        for (size_t i = 0; i < size; ++i) {
            size_t idx = dist(gen);
            volatile int val = vec[idx];
        }
    });

    // list random access (requires traversal)
    double lst_time = measure_time([&]() {
        for (size_t i = 0; i < size; ++i) {
            size_t idx = dist(gen);
            auto it = lst.begin();
            std::advance(it, idx);
            volatile int val = *it;
        }
    });

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "deque:  " << dq_time << " s\n";
    std::cout << "vector: " << vec_time << " s\n";
    std::cout << "list:   " << lst_time << " s\n";
    std::cout << "deque is " << dq_time / vec_time << "x slower than vector\n";
    std::cout << "list  is " << lst_time / dq_time << "x slower than deque\n";
}

// ========================================
// 3. Middle insertion/deletion test
// ========================================
void test_middle_operations(size_t size) {
    std::cout << "\n=== Middle insertion/deletion test (" << size << " operations) ===" << std::endl;

    std::deque<int> dq(size / 2);
    std::vector<int> vec(size / 2);
    std::list<int> lst(size / 2);

    // deque middle ops
    double dq_time = measure_time([&]() {
        auto mid = dq.begin() + dq.size() / 2;
        for (size_t i = 0; i < size; ++i) {
            mid = dq.insert(mid, i);
            if (i % 3 == 0)
                mid = dq.erase(mid);
        }
    });

    // vector middle ops
    double vec_time = measure_time([&]() {
        auto mid = vec.begin() + vec.size() / 2;
        for (size_t i = 0; i < size; ++i) {
            mid = vec.insert(mid, i);
            if (i % 3 == 0)
                mid = vec.erase(mid);
        }
    });

    // list middle ops
    double lst_time = measure_time([&]() {
        auto mid = lst.begin();
        std::advance(mid, lst.size() / 2);
        for (size_t i = 0; i < size; ++i) {
            mid = lst.insert(mid, i);
            if (i % 3 == 0)
                mid = lst.erase(mid);
        }
    });

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "deque:  " << dq_time << " s\n";
    std::cout << "vector: " << vec_time << " s\n";
    std::cout << "list:   " << lst_time << " s\n";
    std::cout << "deque is " << dq_time / lst_time << "x slower than list\n";
    std::cout << "vector is " << vec_time / dq_time << "x slower than deque\n";
}

// ========================================
// 4. Memory usage comparison (approximate)
// ========================================
void test_memory_usage(size_t size) {
    std::cout << "\n=== Approximate memory usage (" << size << " elements) ===" << std::endl;

    std::deque<int> dq(size);
    std::vector<int> vec(size);
    std::list<int> lst(size, 0);

    // Approximate memory usage
    size_t dq_memory = dq.size() * sizeof(int) +
                       dq.max_size() / sizeof(int) * sizeof(void*);
    size_t vec_memory = vec.capacity() * sizeof(int);
    size_t lst_memory = lst.size() * (sizeof(int) + 2 * sizeof(void*));

    std::cout << "deque:  ~" << dq_memory / 1024 << " KB\n";
    std::cout << "vector: ~" << vec_memory / 1024 << " KB\n";
    std::cout << "list:   ~" << lst_memory / 1024 << " KB\n";
}

// ========================================
// main
// ========================================
int main() {
    const size_t ops = 100000;
    const size_t size = 50000;

    test_double_ended_operations(ops);
    test_random_access(size);
    test_middle_operations(ops / 10);
    test_memory_usage(size);

    return 0;
}

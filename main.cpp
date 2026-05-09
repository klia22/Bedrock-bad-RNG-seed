#include <cstdint>
#include <climits>
#include <iostream>
#include <chrono>
#include <fstream>
#include <cassert>
#include <omp.h>
#include <string>

namespace MT {
    static const int N = 624;
    static const int M = 397;
    static const uint32_t MATRIX_A = 0x9908B0DFU;
    static const uint32_t UPPER_MASK = 0x80000000U;
    static const uint32_t LOWER_MASK = 0x7FFFFFFFU;

    static uint32_t mt[N];
    static int index = N + 1;

    static void twist() {
        for (int i = 0; i < N; ++i) {
            uint32_t x = (mt[i] & UPPER_MASK) | (mt[(i + 1) % N] & LOWER_MASK);
            uint32_t xA = x >> 1;
            if (x & 1U) {
                xA ^= MATRIX_A;
            }
            mt[i] = mt[(i + M) % N] ^ xA;
        }
        index = 0;
    }

    void init(uint32_t seed) {
        mt[0] = seed;
        for (int i = 1; i < N; ++i) {
            mt[i] = 1812433253U * (mt[i - 1] ^ (mt[i - 1] >> 30)) + static_cast<uint32_t>(i);
        }
        index = N;
    }

    uint32_t extract() {
        if (index >= N) {
            if (index > N) {
                init(5489U);
            }
            twist();
        }

        uint32_t y = mt[index++];
        y ^= (y >> 11);
        y ^= (y << 7) & 0x9D2C5680U;
        y ^= (y << 15) & 0xEFC60000U;
        y ^= (y >> 18);

        return y;
    }
}

int main() {
    int num_threads;
    std::cin >> num_threads;
    if (num_threads <= 0 || num_threads > omp_get_max_threads()) {
        num_threads = omp_get_max_threads();
    }
    omp_set_num_threads(num_threads);

    std::ofstream outFile("Results.txt");
    auto start = std::chrono::high_resolution_clock::now();
    // Test 1
    int testseed1 = 1000686894;
    MT::init(testseed1);
    uint32_t first = MT::extract();
    uint32_t second = MT::extract();
    std::cout << "Test1: first=" << first << ", second=" << second << std::endl;
    // assert(second == 1);
    outFile << "Starting search";
    //Test 2
    int testseed2 = 1669320484;
    MT::init(testseed2);
    uint32_t first2 = MT::extract();
    uint32_t second2 = MT::extract();
    std::cout << "Test2: first2=" << first2 << ", second2=" << second2 << std::endl;
    // assert(second2 == first2+1);
    #pragma omp parallel for  // Added OpenMP parallel for loop
    for (long long seed = INT_MIN; seed <= INT_MAX; seed++){
        MT::init(static_cast<uint32_t>(seed));
        uint32_t first = MT::extract();
        if (first <= 1 || first == INT_MAX) {
            #pragma omp critical  // Protect output
            {
                outFile << "Seed: " << seed << " produces first output: " << first << std::endl;
                std::cout << "Seed: " << seed << " produces first output: " << first << std::endl;
            }
        }
        uint32_t second = MT::extract();
        if (second <= 1 || second == INT_MAX) {
            #pragma omp critical
            {
                outFile << "Seed: " << seed << " produces second output: " << second << std::endl;
                std::cout << "Seed: " << seed << " produces second output: " << second << std::endl;
            }
        }
        if (std::abs(static_cast<int64_t>(second) - static_cast<int64_t>(first)) <= 1) {
            #pragma omp critical
            {
                outFile << "Seed: " << seed << " produces first output: " << first << " and second output: " << second << std::endl;
                std::cout << "Seed: " << seed << " produces first output: " << first << " and second output: " << second << std::endl;
            }
        }
        uint32_t third = MT::extract();
        if (third <= 1 || third == INT_MAX) {
            #pragma omp critical
            {
                outFile << "Seed: " << seed << " produces third output: " << third << std::endl;
                std::cout << "Seed: " << seed << " produces third output: " << third << std::endl;
            }
        }
        if (seed % 10000000 == 0) {
            #pragma omp critical
            {
                auto end = std::chrono::high_resolution_clock::now();
                std::cout << "Checked up to seed: " << seed << "   ";
                double totalSeeds = static_cast<double>(INT_MAX) - static_cast<double>(INT_MIN) + 1.0;
                double done = static_cast<double>(seed) - static_cast<double>(INT_MIN) + 1.0;
                double progress = done / totalSeeds;
                double elapsedSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
                double etaSeconds = (progress > 0.0) ? elapsedSeconds * (1.0 - progress) / progress : 0.0;
                std::cout << std::fixed << std::setprecision(3);
                std::cout << "Percentage: " << (progress * 100.0) << "%   ";
                std::cout << "eta: " << etaSeconds << " seconds" << std::endl;
            }
        }
    }
}
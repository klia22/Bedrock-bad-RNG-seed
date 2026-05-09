#include <cstdint>
#include <climits>
#include <iostream>
#include <chrono>
#include <fstream>
#include <cassert>
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
    MT::init(-1444255050);
    uint32_t first = MT::extract();
    uint32_t second = MT::extract();
    std::cout << "first=" << first << ", second=" << second << std::endl;
}
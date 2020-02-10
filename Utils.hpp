#ifndef UTILS_HPP
#define UTILS_HPP

#include "Config.hpp"

#include <iostream>
#include <string>
#include <tuple>

#define DEBUG_MSG(x) std::cout << (x) << std::endl

#define CESTL constexpr static thread_local

MoveIndex human_coords2int(const std::string& str);
std::string int2human_coords(const int m);
MoveIndex fromWthorCoords(const int c);

//Returns the number of 1 bits in the numbre x
int popcnt(const bs64 x);

template<typename Container>
void printCtn(const Container& c) {
	for (auto x : c)
		std::cout << x << ' ';
	std::cout << std::endl;
}

template <typename T, typename Callable>
void tupleForEach(T&& tuple, Callable&& c) {
    const auto c_with_return = [c](auto&& arg) -> char { c(arg); return 0;};
    const auto dummy = [](auto&& ...arg){;};
    const auto lambda = [c_with_return, dummy](auto&& ...args) {dummy(c_with_return(args)...);};
    std::apply(lambda, tuple);
}

#define SQR(x) ((x)*(x))

#define BITn_MASK(n) (bs64(1) << (n))
#define BIT0_MASK 0x1
#define BIT1_MASK 0x2
#define BIT2_MASK 0x4
#define BIT3_MASK 0x8
#define BIT4_MASK 0x10
#define BIT5_MASK 0x20
#define BIT6_MASK 0x40
#define BIT7_MASK 0x80
#define BIT8_MASK 0x100
#define BIT9_MASK 0x200
#define BIT10_MASK 0x400
#define BIT11_MASK 0x800
#define BIT12_MASK 0x1000
#define BIT13_MASK 0x2000
#define BIT14_MASK 0x4000
#define BIT15_MASK 0x8000
#define BIT16_MASK 0x10000
#define BIT17_MASK 0x20000
#define BIT18_MASK 0x40000
#define BIT19_MASK 0x80000
#define BIT20_MASK 0x100000
#define BIT21_MASK 0x200000
#define BIT22_MASK 0x400000
#define BIT23_MASK 0x800000
#define BIT24_MASK 0x1000000
#define BIT25_MASK 0x2000000
#define BIT26_MASK 0x4000000
#define BIT27_MASK 0x8000000
#define BIT28_MASK 0x10000000
#define BIT29_MASK 0x20000000
#define BIT30_MASK 0x40000000
#define BIT31_MASK 0x80000000
#define BIT32_MASK 0x100000000
#define BIT33_MASK 0x200000000
#define BIT34_MASK 0x400000000
#define BIT35_MASK 0x800000000
#define BIT36_MASK 0x1000000000
#define BIT37_MASK 0x2000000000
#define BIT38_MASK 0x4000000000
#define BIT39_MASK 0x8000000000
#define BIT40_MASK 0x10000000000
#define BIT41_MASK 0x20000000000
#define BIT42_MASK 0x40000000000
#define BIT43_MASK 0x80000000000
#define BIT44_MASK 0x100000000000
#define BIT45_MASK 0x200000000000
#define BIT46_MASK 0x400000000000
#define BIT47_MASK 0x800000000000
#define BIT48_MASK 0x1000000000000
#define BIT49_MASK 0x2000000000000
#define BIT50_MASK 0x4000000000000
#define BIT51_MASK 0x8000000000000
#define BIT52_MASK 0x10000000000000
#define BIT53_MASK 0x20000000000000
#define BIT54_MASK 0x40000000000000
#define BIT55_MASK 0x80000000000000
#define BIT56_MASK 0x100000000000000
#define BIT57_MASK 0x200000000000000
#define BIT58_MASK 0x400000000000000
#define BIT59_MASK 0x800000000000000
#define BIT60_MASK 0x1000000000000000
#define BIT61_MASK 0x2000000000000000
#define BIT62_MASK 0x4000000000000000
#define BIT63_MASK 0x8000000000000000

#endif
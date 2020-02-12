#include "Utils.hpp"

#include "Config.hpp"

#include <cassert>
#include <string>
//#include <intrin.h>

MoveIndex human_coords2int(const std::string& str) {
	assert(str.size() == 2);
	// using char operations on purpose
	// 64 - x because index 0 is the bottom right square
	return  63 - (8 * (str[1] - '1') + (str[0] - 'a'));
}

std::string int2human_coords(const int m) {
	uint8_t y = 7 - m / 8;
	uint8_t x = 7 - m % 8;
	std::string coords("00");
	coords[0] = 'a' + x;
	coords[1] = '1' + y;
	return coords;
}

MoveIndex fromWthorCoords(const int c) {
	// Wthor Coords : col + 10 * row
	int row = c / 10 - 1;
	int col = c % 10 - 1;
	if (row < 0 || row > 7) std::cout << "row " << row << std::endl;
	if (col < 0 || col > 7) std::cout << "col " << col << std::endl; 
	return row*8 + col;
}

int popcnt(const bs64 x) {
	/*int cnt = 0;
	for (unsigned i = 0; i < 64;++i)
		if (x >> i & 1)
			++cnt;
	return cnt;*/
	return __builtin_popcountll(x);
	//return __popcnt64(x);
}	
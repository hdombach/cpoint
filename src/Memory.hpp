#pragma once
#include <vector>

class Memory {
	public:
		Memory();
		Memory(uint32_t size): _m(size, 0) {}

		uint32_t &operator[](uint32_t index) {
			while (_m.size() < index) {
				_m.push_back(0);
			}
			return _m[index];
		}

	private:
		std::vector<uint32_t> _m;
};

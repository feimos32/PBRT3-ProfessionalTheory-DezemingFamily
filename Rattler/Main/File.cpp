#include "File.h"

#include <fstream>

bool FileCopy(std::string from, std::string to) {
	long long fsize;
	char* copyBuffer = new char;
	std::ifstream F(from, std::ios::in | std::ios::binary);
	if (!F) return false;

	std::ofstream O(to, std::ios::out | std::ios::binary);
	if (!O) {
		F.close();
		return false;
	}

	F.seekg(0, std::ios::end);
	fsize = F.tellg();
	F.seekg(0, std::ios::beg);
	while (fsize--) {
		F.read(copyBuffer, 1);
		O.write(copyBuffer, 1);
	}
	F.close();
	O.close();
	delete copyBuffer;
	return true;
}





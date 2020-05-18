#include <iostream>

#include <xtools/Uuid.h>

int main() {
	x::tool::UuidGenerator uuid;
	uuid.Init(1, 1);

	int i = 10;
	while (i--) {
		std::cout << uuid.Get() << std::endl;
	}
	return 0;
}
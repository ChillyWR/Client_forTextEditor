#include "ClientConnection.h"

int main() {
	try {
		ClientConnection client;
	}
	catch (std::string err) {
		std::cout << err << std::endl;
	}
	return 0;
}

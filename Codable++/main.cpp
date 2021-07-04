#include "JSON.h"
#include "Codable.h"
using namespace std;

CodingKey* keys = {};

class Contact {
	string phoneNumber;
	string name;
};

int main() {
	JSONEncoder encoder;
	JSONDecoder decoder;
	decoder.container(keys);
	//auto container = decoder.container(keys);
	return 0;
}
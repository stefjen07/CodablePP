#include <string>
#include "Codable.h"
using namespace std;

class JSONContainer {
public:
	string content() {
		return "";
	}

	template<class T>
	T decode(T type, CodingKey key) {
		return type;
	}

	JSONContainer() {

	}
};

class JSONEncoder {
public:
	JSONContainer container(CodingKey* keys) {
		return JSONContainer();
	}

	JSONEncoder() {

	}
};

class JSONDecoder {
public:
	JSONContainer container(CodingKey* keys) {
		return JSONContainer();
	}

	JSONDecoder() {

	}
};
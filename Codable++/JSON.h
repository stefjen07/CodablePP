#ifndef JSON_H
#define JSON_H
#include "Codable.h"
#include <string>

class JSONContainer {
public:
	std::string content();

	template<class T>
	T decode(T type, CodingKey key);
};
class JSONEncoder {
public:
	JSONContainer container(CodingKey* keys);
};
class JSONDecoder {
public:
	JSONContainer container(CodingKey* keys);
};

#endif
#ifndef JSON_H
#define JSON_H
#include "Codable.h"
#include <string>

class JSONContainer: CoderContainer {
public:
    std::string content();
};

class JSONEncodeContainer: JSONContainer {
public:
    template <typename T>
    void encode(T value, CodingKey key) {
        return;
    }

    JSONEncodeContainer();
};

class JSONDecodeContainer: JSONContainer {
public:
    JSONDecodeContainer* childs[];
    
    template <typename T>
    T decode(T type, CodingKey key) {
        return type;
    }

    JSONDecodeContainer(std::string content);
};

class JSONEncoder: Encoder {
public:
    JSONEncodeContainer container(CodingKey keys[], unsigned keysAmount);
};

class JSONDecoder: Decoder {
public:
    JSONDecodeContainer container(std::string content, CodingKey keys[], unsigned keysAmount);
};

#endif

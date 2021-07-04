#ifndef JSON_H
#define JSON_H
#include "Codable.h"
#include <string>
#include <vector>

using namespace std;

class JSONContainer: CoderContainer {
public:
    std::string content();
};

class JSONEncodeContainer: public JSONContainer {
public:
    template <typename T>
    void encode(T value, CodingKey key) {
        return;
    }

    JSONEncodeContainer();
};

enum class DecodeContainerType {
    closure,
    variable,
    array
};

class JSONDecodeContainer: JSONContainer {
public:
    vector<JSONDecodeContainer*> children = vector<JSONDecodeContainer*>(0);
    string key;
    string content;
    DecodeContainerType type;
    JSONDecodeContainer* nullContainer;
    
    template <typename T>
    T decode(T type, CodingKey key) {
        return type;
    }

    JSONDecodeContainer& operator [](string key) {
        for (int i = 0; i < children.size(); i++) {
            if (children[i]->key == key) {
                return *children[i];
            }
        }
        return *nullContainer;
    }

    JSONDecodeContainer(string content);
    JSONDecodeContainer(string key, string content);
};

class JSONEncoder: Encoder {
public:
    JSONEncodeContainer container(CodingKey keys[], unsigned keysAmount);
};

class JSONDecoder: Decoder {
public:
    JSONDecodeContainer container(std::string content);
};

#endif

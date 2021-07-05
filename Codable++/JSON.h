#ifndef JSON_H
#define JSON_H
#include "Codable.h"
#include <string>
#include <vector>

using namespace std;

class JSONContainer: public CoderContainer {
public:
    string content();
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

class JSONDecodeContainer: public JSONContainer {
public:
    vector<unsigned long> childrenIndexes;
    vector<JSONDecodeContainer>* containers;
    string key;
    string content;
    DecodeContainerType parsedType;
    JSONDecodeContainer* nullContainer;
    
    JSONDecodeContainer& getChild(string key) {
        for (int i = 0; i < childrenIndexes.size(); i++) {
            if (containers->data()[childrenIndexes[i]].key == key) {
                return containers->data()[childrenIndexes[i]];
            }
        }
        return *nullContainer;
    }
    
    JSONDecodeContainer& operator [](string key) {
        return getChild(key);
    }
    
    string decode(string type, CodingKey key) {
        string value = getChild(key).content;
        if(value.length()>1) {
            value.erase(value.begin());
            value.erase(value.begin()+value.length()-1);
        }
        return value;
    }
    
    template <typename T>
    T decode(T type, CodingKey key) {
        if (is_polymorphic<T>::value) {
            Codable* casted = dynamic_cast<Codable*>(&type);
            casted->decode(this);
        }
        return type;
    }

    JSONDecodeContainer(string content, vector<JSONDecodeContainer>* containers);
    JSONDecodeContainer(string key, string content, vector<JSONDecodeContainer>* containers);
};

class JSONEncoder: Encoder {
public:
    JSONEncodeContainer container(CodingKey keys[], unsigned keysAmount);
};

class JSONDecoder: Decoder {
private:
    vector<JSONDecodeContainer> containers;
public:
    JSONDecodeContainer container(string content);
};

#endif

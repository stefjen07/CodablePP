#ifndef JSON_H
#define JSON_H
#include "Codable.h"
#include <string>
#include <vector>

#define MAIN_CONTAINER_KEY ""

using namespace std;

class JSONContainer: public CoderContainer {
public:
    vector<unsigned long> childrenIndexes;
    string key;
    string content;
};

enum class JSONContainerType {
    closure,
    variable,
    array
};

class JSONEncodeContainer: public JSONContainer {
public:
    vector<JSONEncodeContainer>* containers;
    JSONContainerType encodingType;
    string value;

    void generateContent() {
        content = "";
        if (key.length()) {
            content = "\"" + key + "\": ";
        }
        if (encodingType == JSONContainerType::variable) {
            content += value;
            return;
        }
        if (encodingType == JSONContainerType::array) {
            content += "[";
        }
        else if (encodingType == JSONContainerType::closure) {
            content += "{";
        }
        for (int i = 0; i < childrenIndexes.size(); i++) {
            if (i != 0)
                content += ",";
            content += containers->data()[childrenIndexes[i]].content;
        }
        if (encodingType == JSONContainerType::array) {
            content += "]";
        }
        else if (encodingType == JSONContainerType::closure) {
            content += "}";
        }
    }

    template <typename T>
    void encode(vector<T> value, CodingKey key) {
        JSONEncodeContainer array(this->containers);
        array.key = key;
        array.encodingType = JSONContainerType::array;
        array.containers = containers;
        for (int i = 0; i < value.size(); i++) {
            JSONEncodeContainer valueContainer(containers);
            valueContainer.encode(value[i]);
            containers->push_back(valueContainer);
            array.childrenIndexes.push_back(containers->size()-1);
        }
        array.generateContent();
        containers->push_back(array);
        childrenIndexes.push_back(containers->size()-1);
    }

    void encode(bool value, CodingKey key="");
    void encode(int value, CodingKey key="");
    void encode(float value, CodingKey key="");
    void encode(string value, CodingKey key="");

    template <typename T>
    void encode(T value, CodingKey key) {
        JSONEncodeContainer result(this->containers);
        result.key = key;
        result.encodingType = JSONContainerType::closure;
        if (is_polymorphic<T>::value) {
            Codable* casted = dynamic_cast<Codable*>(&value);
            casted->encode(&result);
        }
        result.generateContent();
        containers->push_back(result);
        childrenIndexes.push_back(containers->size()-1);
    }

    template <typename T>
    void encode(T value) {
        encodingType = JSONContainerType::closure;
        if (is_polymorphic<T>::value) {
            Codable* casted = dynamic_cast<Codable*>(&value);
            casted->encode(this);
        }
        generateContent();
    }

    template <typename T>
    void encode(vector<T> value) {
        encodingType = JSONContainerType::array;
        for (int i = 0; i < value.size(); i++) {
            JSONEncodeContainer valueContainer(containers);
            valueContainer.encode(value[i]);
            containers->push_back(valueContainer);
            childrenIndexes.push_back(containers->size() - 1);
        }
        generateContent();
    }

    JSONEncodeContainer(vector<JSONEncodeContainer>* containers);
    JSONEncodeContainer(string key, string content, vector<JSONEncodeContainer>* containers);
};

class JSONDecodeContainer: public JSONContainer {
public:
    vector<JSONDecodeContainer>* containers;
    JSONContainerType parsedType;
    JSONDecodeContainer* nullContainer;
    
    JSONDecodeContainer* getChild(string key) {
        for (int i = 0; i < childrenIndexes.size(); i++) {
            if (containers->data()[childrenIndexes[i]].key == key) {
                return &containers->data()[childrenIndexes[i]];
            }
        }
        return nullContainer;
    }
    
    JSONDecodeContainer* operator [](string key) {
        return getChild(key);
    }
    
    bool decode(bool type, CodingKey key);
    int decode(int type, CodingKey key);
    float decode(float type, CodingKey key);
    string decode(string type, CodingKey key);
    
    template <class T>
    T decode(T type, CodingKey key) {
        if (is_polymorphic<T>::value) {
            Codable* casted = dynamic_cast<Codable*>(&type);
            JSONDecodeContainer* jsonContainer = getChild(key);
            if (key == MAIN_CONTAINER_KEY) {
                jsonContainer = this;
            }
            if (jsonContainer != NULL) {
                CoderContainer* container = static_cast<CoderContainer*>(jsonContainer);
                if (container != NULL) {
                    casted->decode(container);
                }
            }
        }
        return type;
    }

    template <class T>
    vector<T> decode(vector<T> type, CodingKey key) {
        vector<T> result;
        auto array = getChild(key);
        if (array == NULL) {
            return result;
        }
        for (int i = 0; i < array->childrenIndexes.size(); i++) {
            T item = containers->data()[array->childrenIndexes[i]].decode(T());
            result.push_back(item);
        }
        return result;
    }
    
    template<typename T>
    T decode(T type) {
        return decode(type, MAIN_CONTAINER_KEY);
    }

    JSONDecodeContainer(string content, vector<JSONDecodeContainer>* containers);
    JSONDecodeContainer(string key, string content, vector<JSONDecodeContainer>* containers);
};

class JSONEncoder: Encoder {
private:
    vector<JSONEncodeContainer> containers;
public:
    JSONEncodeContainer container();
};

class JSONDecoder: Decoder {
private:
    vector<JSONDecodeContainer> containers;
public:
    JSONDecodeContainer container(string content);
};

#endif

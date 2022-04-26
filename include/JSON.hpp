#ifndef JSON_H
#define JSON_H

#include "Codable.hpp"
#include <string>
#include <vector>

//Empty key for arrays and the highest closures
#define MAIN_CONTAINER_KEY ""

//Container for encoding to/decoding from JSON format
//Consists of:
// childrenIndexes - array of children's indexes in containers array (applicable only for arrays and closures)
// key - name of container
// content - text representation of children (for arrays and closures) or value (for variables)
class JSONContainer: public CoderContainer {
public:
    std::vector<unsigned long> childrenIndexes;
    std::string key;
    std::string content;
};

//Types of JSON containers:
// closure - array of other containers with '{}'
// variable - data value (number, text, boolean)
// array - array of other containers with '[]' and without keys 
enum class JSONContainerType {
    closure,
    variable,
    array
};

//Container for encoding to JSON format
//Consists of:
// containers - pointer to array of all the containers used in encoding
// encodingType - container type (closure, array or variable)
// value - data value (for variables only)
class JSONEncodeContainer: public JSONContainer {
public:
    std::vector<JSONEncodeContainer>* containers;
    JSONContainerType encodingType;
    std::string value;

    //method for converting container to JSON format
    void generateContent();

    //Encoding std::vector as array
    template <typename T>
    void encode(std::vector<T> value, CodingKey key) {
        JSONEncodeContainer array(this->containers);
        array.key = key;
        array.encodingType = JSONContainerType::array;

        for (int i = 0; i < value.size(); i++) {
            JSONEncodeContainer valueContainer(containers);
            valueContainer.encode(value[i]);
            containers->push_back(valueContainer);
            array.childrenIndexes.push_back(containers->size() - 1);
        }

        array.generateContent();
        containers->push_back(array);
        childrenIndexes.push_back(containers->size() - 1);
    }

    //Encoding methods for standard data types 

    void encode(bool value, CodingKey key="");
    void encode(int value, CodingKey key="");
    void encode(long long value, CodingKey key = "");
    void encode(float value, CodingKey key="");
    void encode(double value, CodingKey key = "");
    void encode(std::string value, CodingKey key="", bool withQuotes = true);

    //Encoding method for classes with Codable protocol 
    template <class T>
    void encode(T value, CodingKey key) {
        JSONEncodeContainer result(this->containers);
        result.key = key;
        result.encodingType = JSONContainerType::closure;

        if (std::is_polymorphic<T>::value) {
            Codable* casted = dynamic_cast<Codable*>(&value);
            casted->encode(&result);
        }

        result.generateContent();
        containers->push_back(result);
        childrenIndexes.push_back(containers->size() - 1);
    }

    //Encoding method for classes with Codable protocol without key
    template <class T>
    void encode(T value) {
        encodingType = JSONContainerType::closure;

        if (std::is_polymorphic<T>::value) {
            Codable* casted = dynamic_cast<Codable*>(&value);
            casted->encode(this);
        }

        generateContent();
    }

    //Encoding std::vector as array without key
    template <typename T>
    void encode(std::vector<T> value) {
        encodingType = JSONContainerType::array;

        for (int i = 0; i < value.size(); i++) {
            JSONEncodeContainer valueContainer(containers);
            valueContainer.encode(value[i]);
            containers->push_back(valueContainer);
            childrenIndexes.push_back(containers->size() - 1);
        }

        generateContent();
    }

    JSONEncodeContainer(std::vector<JSONEncodeContainer>* containers);
    JSONEncodeContainer(std::string key, std::string content, std::vector<JSONEncodeContainer>* containers);
};

//Container for decoding from JSON format
//Consists of:
// containers - pointer to array of all the containers used in decoding
// parsedType - container type (closure, array or variable)
class JSONDecodeContainer: public JSONContainer {
public:
    std::vector<JSONDecodeContainer>* containers;
    JSONContainerType parsedType;
    
    //Getting container with specific key from children containers
    JSONDecodeContainer* operator [](std::string key);
    
    //Decoding methods for standard data types 

    bool decode(bool type, CodingKey key);
    int decode(int type, CodingKey key);
    long long decode(long long type, CodingKey key);
    float decode(float type, CodingKey key);
    double decode(double type, CodingKey key);
    std::string decode(std::string type, CodingKey key, bool withQuotes = true);
    
    //Decoding method for classes with Codable protocol
    template <class T>
    T decode(T type, CodingKey key) {
        if (std::is_polymorphic<T>::value) {
            Codable* casted = dynamic_cast<Codable*>(&type);
            JSONDecodeContainer* jsonContainer = operator[](key);

            //If key is empty, then decode to current container
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

        //Returning modified type sample
        return type;
    }

    //Decoding method for array
    template <typename T>
    std::vector<T> decode(std::vector<T> type, CodingKey key) {
        std::vector<T> result;
        auto array = operator[](key);

        if (array != NULL) {
            for (int i = 0; i < array->childrenIndexes.size(); i++) {
                T item = containers->data()[array->childrenIndexes[i]].decode(T());
                result.push_back(item);
            }
        }

        return result;
    }

    //Decoding method for array without key (array in this container)
    template <typename T>
    std::vector<T> decode(std::vector<T> type) {
        std::vector<T> result;

        for (int i = 0; i < childrenIndexes.size(); i++) {
            T item = containers->data()[childrenIndexes[i]].decode(T());
            result.push_back(item);
        }

        return result;
    }
    
    //Decoding method for all fields without key
    template<typename T>
    T decode(T type) {
        return decode(type, MAIN_CONTAINER_KEY);
    }

    JSONDecodeContainer(std::string content, std::vector<JSONDecodeContainer>* containers);
    JSONDecodeContainer(std::string key, std::string content, std::vector<JSONDecodeContainer>* containers);
};

//JSON encoder class
//Consists of:
// containers - array of all containers created with use of this encoder
class JSONEncoder: Encoder {
private:
    std::vector<JSONEncodeContainer> containers;
public:
    JSONEncodeContainer container();
};

//JSON decoder class
//Consists of:
// containers - array of all containers created with use of this decoder
class JSONDecoder: Decoder {
private:
    std::vector<JSONDecodeContainer> containers;
public:
    JSONDecodeContainer container(std::string content);
};

#endif

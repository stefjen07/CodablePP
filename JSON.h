#ifndef JSON_H
#define JSON_H

#include "Codable.h"
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
        //Creating container for array
        JSONEncodeContainer array(this->containers);
        //Setting container's key to the value of a given key
        array.key = key;
        //Setting encoding type to array
        array.encodingType = JSONContainerType::array;
        //Encoding every element of array
        for (int i = 0; i < value.size(); i++) {
            //Creating container for element
            JSONEncodeContainer valueContainer(containers);
            //Encoding element to container
            valueContainer.encode(value[i]);
            //Adding encoded container to array's container
            containers->push_back(valueContainer);
            //Adding encoded container to array's children
            array.childrenIndexes.push_back(containers->size() - 1);
        }
        //Generate JSON respresentation of array
        array.generateContent();
        //Adding encoded container to containers array
        containers->push_back(array);
        //Adding encoded container to children array of current container
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
        //Creating container for result
        JSONEncodeContainer result(this->containers);
        //Setting container's key to the value of a given key
        result.key = key;
        //Setting encoding type to closure
        result.encodingType = JSONContainerType::closure;
        //Checking if provided type is polymorphic
        if (std::is_polymorphic<T>::value) {
            //Converting provided value to Codable
            Codable* casted = dynamic_cast<Codable*>(&value);
            //Encoding provided value to result container
            casted->encode(&result);
        }
        //Generating JSON representation of result container content
        result.generateContent();
        //Adding result container to containers array
        containers->push_back(result);
        //Adding result container to children array of current container
        childrenIndexes.push_back(containers->size() - 1);
    }

    //Encoding method for classes with Codable protocol without key
    template <class T>
    void encode(T value) {
        //Setting encoding type to closure
        encodingType = JSONContainerType::closure;
        //Checking if provided type is polymorphic
        if (std::is_polymorphic<T>::value) {
            //Converting provided value to Codable
            Codable* casted = dynamic_cast<Codable*>(&value);
            //Encoding provided value to result container
            casted->encode(this);
        }
        //Generating JSON representation of container content
        generateContent();
    }

    //Encoding std::vector as array without key
    template <typename T>
    void encode(std::vector<T> value) {
        //Setting encoding type to array
        encodingType = JSONContainerType::array;
        //Encoding every element of array
        for (int i = 0; i < value.size(); i++) {
            //Creating a container for element
            JSONEncodeContainer valueContainer(containers);
            //Encoding element to container
            valueContainer.encode(value[i]);
            //Adding element to containers std::vector
            containers->push_back(valueContainer);
            //Adding element to children std::vector of array container
            childrenIndexes.push_back(containers->size() - 1);
        }
        //Generating JSON representation of container content
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
        //Checking if provided type is polymorphic
        if (std::is_polymorphic<T>::value) {
            //Converting type sample to Codable
            Codable* casted = dynamic_cast<Codable*>(&type);
            //Getting container for decoding
            JSONDecodeContainer* jsonContainer = operator[](key);
            //If key is empty, then decoding to current container
            if (key == MAIN_CONTAINER_KEY) {
                jsonContainer = this;
            }
            if (jsonContainer != NULL) {
                //Converting JSON container to CoderContainer
                CoderContainer* container = static_cast<CoderContainer*>(jsonContainer);
                if (container != NULL) {
                    //Decoding value to container
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
        //Creating std::vector for result
        std::vector<T> result;
        //Getting container for decoding
        auto array = operator[](key);
        if (array != NULL) {
            //Decoding every element of array
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
        //Creating std::vector for result
        std::vector<T> result;
        //Decoding every element of array
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

#ifndef JSON_H
#define JSON_H

#include "Codable.h"
#include <string>
#include <vector>

//Empty key for arrays and the highest closures
#define MAIN_CONTAINER_KEY ""

using namespace std;

//Container for encoding to/decoding from JSON format
//Consists of:
// childrenIndexes - array of children's indexes in containers array (applicable only for arrays and closures)
// key - name of container
// content - text representation of children (for arrays and closures) or value (for variables)
class JSONContainer: public CoderContainer {
public:
    vector<unsigned long> childrenIndexes;
    string key;
    string content;
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
    vector<JSONEncodeContainer>* containers;
    JSONContainerType encodingType;
    string value;

    //method for converting container to JSON format
    void generateContent();

    //Encoding vector as array
    template <typename T>
    void encode(vector<T> value, CodingKey key);

    //Encoding methods for standard data types 

    void encode(bool value, CodingKey key="");
    void encode(int value, CodingKey key="");
    void encode(long long value, CodingKey key = "");
    void encode(float value, CodingKey key="");
    void encode(double value, CodingKey key = "");
    void encode(string value, CodingKey key="", bool withQuotes = true);

    //Encoding method for classes with Codable protocol 
    template <class T>
    void encode(T value, CodingKey key);

    //Encoding method for classes with Codable protocol without key
    template <class T>
    void encode(T value);

    //Encoding vector as array without key
    template <typename T>
    void encode(vector<T> value);

    JSONEncodeContainer(vector<JSONEncodeContainer>* containers);
    JSONEncodeContainer(string key, string content, vector<JSONEncodeContainer>* containers);
};

//Container for decoding from JSON format
//Consists of:
// containers - pointer to array of all the containers used in decoding
// parsedType - container type (closure, array or variable)
class JSONDecodeContainer: public JSONContainer {
public:
    vector<JSONDecodeContainer>* containers;
    JSONContainerType parsedType;
    
    //Getting container with specific key from children containers
    JSONDecodeContainer* operator [](string key);
    
    //Decoding methods for standard data types 

    bool decode(bool type, CodingKey key);
    int decode(int type, CodingKey key);
    long long decode(long long type, CodingKey key);
    float decode(float type, CodingKey key);
    double decode(double type, CodingKey key);
    string decode(string type, CodingKey key, bool withQuotes = true);
    
    //Decoding method for classes with Codable protocol
    template <class T>
    T decode(T type, CodingKey key);

    //Decoding method for array
    template <typename T>
    vector<T> decode(vector<T> type, CodingKey key);

    //Decoding method for array without key (array in this container)
    template <typename T>
    vector<T> decode(vector<T> type);
    
    //Decoding method for all fields without key
    template<typename T>
    T decode(T type);

    JSONDecodeContainer(string content, vector<JSONDecodeContainer>* containers);
    JSONDecodeContainer(string key, string content, vector<JSONDecodeContainer>* containers);
};

//JSON encoder class
//Consists of:
// containers - array of all containers created with use of this encoder
class JSONEncoder: Encoder {
private:
    vector<JSONEncodeContainer> containers;
public:
    JSONEncodeContainer container();
};

//JSON decoder class
//Consists of:
// containers - array of all containers created with use of this decoder
class JSONDecoder: Decoder {
private:
    vector<JSONDecodeContainer> containers;
public:
    JSONDecodeContainer container(string content);
};

#endif

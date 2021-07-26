#include "Codable.h"
#include "JSON.h"
#include <vector>
#include <sstream>
#include <string>

JSONEncodeContainer::JSONEncodeContainer(vector<JSONEncodeContainer>* containers) {
    this->type = CoderType::json;
    this->containers = containers;
}

JSONEncodeContainer::JSONEncodeContainer(string key, string content, vector<JSONEncodeContainer>* containers) : JSONEncodeContainer::JSONEncodeContainer(containers) {
    this->key = key;
    this->content = content;
}

//method for converting container to JSON format
void JSONEncodeContainer::generateContent() {
    content = "";
    //Adding container key if applicable
    if (key.length()) {
        content = "\"" + key + "\": ";
    }
    //If container type is variable, then encode its value
    if (encodingType == JSONContainerType::variable) {
        content += value;
        return;
    }
    //If container type is array, then add array character
    if (encodingType == JSONContainerType::array) {
        content += "[";
    }
    //If container type is closure, then add closure character
    else if (encodingType == JSONContainerType::closure) {
        content += "{";
    }
    //Encode all the children
    for (int i = 0; i < childrenIndexes.size(); i++) {
        if (i != 0)
            content += ",";
        content += containers->data()[childrenIndexes[i]].content;
    }
    //If container type is array, then end array with specific character
    if (encodingType == JSONContainerType::array) {
        content += "]";
    }
    //If container is closure, then end closure with specific character.
    else if (encodingType == JSONContainerType::closure) {
        content += "}";
    }
}

//Encoding vector as array
template <typename T>
void JSONEncodeContainer::encode(vector<T> value, CodingKey key) {
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

//Encoding method implementation for boolean
void JSONEncodeContainer::encode(bool value, CodingKey key) {
    JSONEncodeContainer result(key, "", containers);
    result.value = value ? "true" : "false";
    result.encodingType = JSONContainerType::variable;
    result.generateContent();
    containers->push_back(result);
    childrenIndexes.push_back(containers->size() - 1);
}

// !!! Encoding methods below are very similar, because they just using stringstream to convert value of specific data type to string

//Encoding method implementation for integer
void JSONEncodeContainer::encode(int value, CodingKey key) {
    JSONEncodeContainer result(key, "", containers);
    stringstream ss;
    ss << value;
    ss >> result.value;
    result.encodingType = JSONContainerType::variable;
    result.generateContent();
    containers->push_back(result);
    childrenIndexes.push_back(containers->size() - 1);
}

//Encoding method implementation for big integer
void JSONEncodeContainer::encode(long long value, CodingKey key) {
    JSONEncodeContainer result(key, "", containers);
    stringstream ss;
    ss << value;
    ss >> result.value;
    result.encodingType = JSONContainerType::variable;
    result.generateContent();
    containers->push_back(result);
    childrenIndexes.push_back(containers->size() - 1);
}

//Encoding method implementation for float
void JSONEncodeContainer::encode(float value, CodingKey key) {
    JSONEncodeContainer result(key, "", containers);
    stringstream ss;
    ss << value;
    ss >> result.value;
    result.encodingType = JSONContainerType::variable;
    result.generateContent();
    containers->push_back(result);
    childrenIndexes.push_back(containers->size() - 1);
}

//Encoding method implementation for accurate float
void JSONEncodeContainer::encode(double value, CodingKey key) {
    JSONEncodeContainer result(key, "", containers);
    stringstream ss;
    ss << value;
    ss >> result.value;
    result.encodingType = JSONContainerType::variable;
    result.generateContent();
    containers->push_back(result);
    childrenIndexes.push_back(containers->size() - 1);
}

//Encoding method implementation for string
void JSONEncodeContainer::encode(string value, CodingKey key, bool withQuotes) {
    JSONEncodeContainer result(key, "", containers);
    result.value = withQuotes ? ("\"" + value + "\"") : value;
    result.encodingType = JSONContainerType::variable;
    result.generateContent();
    containers->push_back(result);
    childrenIndexes.push_back(containers->size() - 1);
}

//Encoding method for classes with Codable protocol 
template <class T>
void JSONEncodeContainer::encode(T value, CodingKey key) {
    //Creating container for result
    JSONEncodeContainer result(this->containers);
    //Setting container's key to the value of a given key
    result.key = key;
    //Setting encoding type to closure
    result.encodingType = JSONContainerType::closure;
    //Checking if provided type is polymorphic
    if (is_polymorphic<T>::value) {
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
void JSONEncodeContainer::encode(T value) {
    //Setting encoding type to closure
    encodingType = JSONContainerType::closure;
    //Checking if provided type is polymorphic
    if (is_polymorphic<T>::value) {
        //Converting provided value to Codable
        Codable* casted = dynamic_cast<Codable*>(&value);
        //Encoding provided value to result container
        casted->encode(this);
    }
    //Generating JSON representation of container content
    generateContent();
}

//Encoding vector as array without key
template <typename T>
void JSONEncodeContainer::encode(vector<T> value) {
    //Setting encoding type to array
    encodingType = JSONContainerType::array;
    //Encoding every element of array
    for (int i = 0; i < value.size(); i++) {
        //Creating a container for element
        JSONEncodeContainer valueContainer(containers);
        //Encoding element to container
        valueContainer.encode(value[i]);
        //Adding element to containers vector
        containers->push_back(valueContainer);
        //Adding element to children vector of array container
        childrenIndexes.push_back(containers->size() - 1);
    }
    //Generating JSON representation of container content
    generateContent();
}

//Method that debeautifies JSON content (removes spaces and line breaks)
void jsonDebeautify(string& content) {
    //Creating value to prevent deleting spaces or line breaks inside field key or text value
    bool isInValue = false;
    for(int i=0;i<content.length();i++) {
        if (content[i] == '\"') {
            isInValue = !isInValue;
        }
        //Deleting spaces and line breaks
        if ((content[i] == ' ' || content[i] == '\n') && !isInValue) {
            content.erase(content.begin() + i--);
        }
    }
}

//Splitting JSON content with a lot of parameters
//Arguments:
// content - JSON content we want to split
// splitter - character that splits content
// onlyGlobal - if true, only global content (with level 0, when all previously opened brackets are closed) will be checked for matching with splitter
// onlyNonValue - only content outside of keys and values will be checked for matching with splitter (as in jsonDebeautify method)
vector<string> split(string content, char splitter, bool onlyGlobal = false, bool onlyNonValue = false) {
    vector<string> result;
    int firstIndex = 0, level = 0;
    bool isInValue = false;
    
    for(int i=0;i<content.length();i++) {
        //Updating current level by processing open and close brackets
        if(content[i] == '{' || content[i] == '[') {
            level++;
        }
        if(content[i] == '}' || content[i] == ']') {
            level--;
        }
        if(content[i] == '\"') {
            isInValue = !isInValue;
        }
        //Checking if character must be checked for matching with splitter
        if((!level || !onlyGlobal) && (!isInValue || !onlyNonValue)) {
            if(content[i] == splitter) {
                if(i-firstIndex) {
                    result.push_back(content.substr(firstIndex, i-firstIndex));
                }
                //Updating first index of new splitting part
                firstIndex = i+1;
            }
        }
    }
    //Adding last part of splitting
    if(content.length()-firstIndex) {
        result.push_back(content.substr(firstIndex, content.length()-firstIndex));
    }
    return result;
}

//Constructor for JSON container for decoding
JSONDecodeContainer::JSONDecodeContainer(string key, string content, vector<JSONDecodeContainer>* containers) {
    this->type = CoderType::json;
    this->key = key;
    this->containers = containers;
    jsonDebeautify(content);
    //Only variable container can have content with length less than 2
    if (content.length() < 2) {
        this->parsedType = JSONContainerType::variable;
        this->content = content;
        return;
    }
    else {
        //Every closure must begin with open brace
        if (content[0] == '{') {
            this->parsedType = JSONContainerType::closure;
            //Removing braces from content
            content.erase(content.begin());
            content.erase(content.end() - 1);
        }
        //Every array must begin with open square bracket
        else if (content[0] == '[') {
            this->parsedType = JSONContainerType::array;
            //Removing braces from content
            content.erase(content.begin());
            content.erase(content.end() - 1);
        }
        else {
            this->parsedType = JSONContainerType::variable;
        }
        this->content = content;
        //Variable container is final and it can't be divided into several other containers
        if (this->parsedType == JSONContainerType::variable) {
            return;
        }
        //Splitting content by matching with comma
        auto splitted = split(content, ',', true, true);
        //Process every element of splitting result
        for (int i = 0; i < splitted.size(); i++) {
            //Splitting key from value
            auto keyval = split(splitted[i], ':', true, true);
            string key = "";
            string content = "";
            if (keyval.size() > 1 && keyval[0].size() > 1) { //Key found
                //Removing quotes from key
                keyval[0].erase(keyval[0].begin());
                keyval[0].erase(keyval[0].end() - 1);
                key = keyval[0];
                content = keyval[1];
            }
            else { //There is no key for value in JSON
                content = keyval[0];
            }
            //Creating new container from key and value, then adding it to containers array
            containers->push_back(JSONDecodeContainer(key, content, containers));
            //Adding new container to children array of current container
            childrenIndexes.push_back(containers->size()-1);
        }
    }
}

JSONDecodeContainer::JSONDecodeContainer(string content, vector<JSONDecodeContainer>* containers) : JSONDecodeContainer::JSONDecodeContainer(MAIN_CONTAINER_KEY, content, containers) {}

//Getting container with specific key from children containers
JSONDecodeContainer* JSONDecodeContainer::operator [](string key) {
    for (int i = 0; i < childrenIndexes.size(); i++) {
        if (containers->data()[childrenIndexes[i]].key == key) {
            return &containers->data()[childrenIndexes[i]];
        }
    }
    return NULL;
}

//Decoding method for boolean
bool JSONDecodeContainer::decode(bool type, CodingKey key) {
    JSONDecodeContainer* child = operator[](key);
    if(child == NULL) {
        return type;
    }
    string value = child->content;
    return value == "true";
}

// !!! Decoding methods below are very similar, cause they just using stringstream to produce value in specific data type from string !!!

//Decoding method for integer
int JSONDecodeContainer::decode(int type, CodingKey key) {
    JSONDecodeContainer* child = operator[](key);
    if(child == NULL) {
        return type;
    }
    string value = child->content;
    stringstream ss;
    ss << value;
    int result;
    ss >> result;
    return result;
}

//Decoding method for big integer
long long JSONDecodeContainer::decode(long long type, CodingKey key) {
    JSONDecodeContainer* child = operator[](key);
    if (child == NULL) {
        return type;
    }
    string value = child->content;
    stringstream ss;
    ss << value;
    long long result;
    ss >> result;
    return result;
}

//Decoding method for float
float JSONDecodeContainer::decode(float type, CodingKey key) {
    JSONDecodeContainer* child = operator[](key);
    if(child == NULL) {
        return type;
    }
    string value = child->content;
    stringstream ss;
    ss << value;
    float result;
    ss >> result;
    return result;
}

//Decoding method for accurate float
double JSONDecodeContainer::decode(double type, CodingKey key) {
    JSONDecodeContainer* child = operator[](key);
    if (child == NULL) {
        return type;
    }
    string value = child->content;
    stringstream ss;
    ss << value;
    double result;
    ss >> result;
    return result;
}

//Decoding method for string
string JSONDecodeContainer::decode(string type, CodingKey key, bool withQuotes) {
    JSONDecodeContainer* child = operator[](key);
    if(child == NULL) {
        return type;
    }
    string value = child->content;
    //If app expects to receive this JSON field with quotes, then we have to delete them before returning a value.
    if (value.length() > 1 && withQuotes) {
        value.erase(value.begin());
        value.erase(value.begin() + value.length() - 1);
    }
    return value;
}

//Decoding method for classes with Codable protocol
template <class T>
T JSONDecodeContainer::decode(T type, CodingKey key) {
    //Checking if provided type is polymorphic
    if (is_polymorphic<T>::value) {
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
vector<T> JSONDecodeContainer::decode(vector<T> type, CodingKey key) {
    //Creating vector for result
    vector<T> result;
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
vector<T> JSONDecodeContainer::decode(vector<T> type) {
    //Creating vector for result
    vector<T> result;
    //Decoding every element of array
    for (int i = 0; i < childrenIndexes.size(); i++) {
        T item = containers->data()[childrenIndexes[i]].decode(T());
        result.push_back(item);
    }
    return result;
}

//Decoding method for all fields without key
template<typename T>
T JSONDecodeContainer::decode(T type) {
    return decode(type, MAIN_CONTAINER_KEY);
}

JSONEncodeContainer JSONEncoder::container() {
    return JSONEncodeContainer(&containers);
}

JSONDecodeContainer JSONDecoder::container(string content) {
    return JSONDecodeContainer(content, &containers);
}
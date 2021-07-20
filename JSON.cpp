#include <Codable.h>
#include <JSON.h>
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

JSONEncodeContainer JSONEncoder::container() {
    return JSONEncodeContainer(&containers);
}

JSONDecodeContainer JSONDecoder::container(string content) {
    return JSONDecodeContainer(content, &containers);
}

#include <string>
#include "Codable.h"
#include "JSON.h"
#include <vector>
#include <sstream>

JSONEncodeContainer::JSONEncodeContainer(vector<JSONEncodeContainer>* containers) {
    this->type = CoderType::json;
    this->containers = containers;
}

JSONEncodeContainer::JSONEncodeContainer(string key, string content, vector<JSONEncodeContainer>* containers) : JSONEncodeContainer::JSONEncodeContainer(containers) {
    this->key = key;
    this->content = content;
}

void JSONEncodeContainer::encode(bool value, CodingKey key) {
    JSONEncodeContainer result(key, "", containers);
    result.value = value ? "true" : "false";
    result.encodingType = JSONContainerType::variable;
    result.generateContent();
    containers->push_back(result);
    childrenIndexes.push_back(containers->size() - 1);
}

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

void JSONEncodeContainer::encode(string value, CodingKey key) {
    JSONEncodeContainer result(key, "", containers);
    result.value = "\"" + value + "\"";
    result.encodingType = JSONContainerType::variable;
    result.generateContent();
    containers->push_back(result);
    childrenIndexes.push_back(containers->size() - 1);
}

void jsonDebeautify(string& content) {
    bool isInValue = false;
    for(int i=0;i<content.length();i++) {
        if (content[i] == '\"') {
            isInValue = !isInValue;
        }
        if ((content[i] == ' ' || content[i] == '\n') && !isInValue) {
            content.erase(content.begin() + i--);
        }
    }
}

vector<string> split(string content, char splitter, bool onlyGlobal = false, bool onlyNonValue = false) {
    vector<string> result;
    int firstIndex = 0, level = 0;
    bool isInValue = false;
    
    for(int i=0;i<content.length();i++) {
        if(content[i] == '{') {
            level++;
        }
        if(content[i] == '}') {
            level--;
        }
        if(content[i] == '\"') {
            isInValue = !isInValue;
        }
        if((!level || !onlyGlobal) && (!isInValue || !onlyNonValue)) {
            if(content[i] == splitter) {
                if(i-firstIndex) {
                    result.push_back(content.substr(firstIndex, i-firstIndex));
                }
                firstIndex = i+1;
            }
        }
    }
    if(content.length()-firstIndex) {
        result.push_back(content.substr(firstIndex, content.length()-firstIndex));
    }
    return result;
}

JSONDecodeContainer::JSONDecodeContainer(string key, string content, vector<JSONDecodeContainer>* containers) {
    this->type = CoderType::json;
    this->key = key;
    this->containers = containers;
    jsonDebeautify(content);
    if (content.length() < 2) {
        return;
    }
    else {
        if (content[0] == '{') {
            this->parsedType = JSONContainerType::closure;
            content.erase(content.begin());
            content.erase(content.end() - 1);
        }
        else if (content[0] == '[') {
            this->parsedType = JSONContainerType::array;
            content.erase(content.begin());
            content.erase(content.end() - 1);
        }
        else {
            this->parsedType = JSONContainerType::variable;
        }
        this->content = content;
        if (this->parsedType == JSONContainerType::variable) {
            return;
        }
        auto splitted = split(content, ',', true, true);
        for (int i = 0; i < splitted.size(); i++) {
            auto keyval = split(splitted[i], ':', true, true);
            string key = "";
            string content = "";
            if (keyval.size() > 1 && keyval[0].size() > 1) {
                keyval[0].erase(keyval[0].begin());
                keyval[0].erase(keyval[0].end() - 1);
                key = keyval[0];
                content = keyval[1];
            }
            else {
                content = keyval[0];
            }
            containers->push_back(JSONDecodeContainer(key, content, containers));
            childrenIndexes.push_back(containers->size()-1);
        }
    }
}

JSONDecodeContainer::JSONDecodeContainer(string content, vector<JSONDecodeContainer>* containers) : JSONDecodeContainer::JSONDecodeContainer("main", content, containers) {}

bool JSONDecodeContainer::decode(bool type, CodingKey key) {
    JSONDecodeContainer* child = getChild(key);
    if(child == NULL) {
        return type;
    }
    string value = child->content;
    return value == "true";
}

int JSONDecodeContainer::decode(int type, CodingKey key) {
    JSONDecodeContainer* child = getChild(key);
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

float JSONDecodeContainer::decode(float type, CodingKey key) {
    JSONDecodeContainer* child = getChild(key);
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

string JSONDecodeContainer::decode(string type, CodingKey key) {
    JSONDecodeContainer* child = getChild(key);
    if(child == NULL) {
        return type;
    }
    string value = child->content;
    if (value.length() > 1) {
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

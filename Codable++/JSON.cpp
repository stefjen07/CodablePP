#include <string>
#include "Codable.h"
#include "JSON.h"
#include <vector>

#define NULL_CONTAINER_CONTENT "\nnull\n"

JSONEncodeContainer::JSONEncodeContainer() {
    this->type = CoderType::json;
}

void jsonDebeautify(string& content) {
    bool isInValue = false;
    for(int i=0;i<content.length();i++) {
        if (content[i] == '\"') {
            isInValue = !isInValue;
        }
        if((content[i] == ' ' || content[i] == '\n') && !isInValue) {
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
    if(content == NULL_CONTAINER_CONTENT) {
        this->content = content;
        return;
    }
    JSONDecodeContainer nullContainer = JSONDecodeContainer("", NULL_CONTAINER_CONTENT, containers);
    this->nullContainer = &nullContainer;
    jsonDebeautify(content);
    if (content.length() < 2) {
        return;
    }
    else {
        if (content[0] == '{') {
            this->parsedType = DecodeContainerType::closure;
            content.erase(content.begin());
            content.erase(content.end() - 1);
        }
        else if (content[0] == '[') {
            this->parsedType = DecodeContainerType::array;
            content.erase(content.begin());
            content.erase(content.end() - 1);
        }
        else {
            this->parsedType = DecodeContainerType::variable;
        }
        this->content = content;
        if (this->parsedType == DecodeContainerType::variable) {
            return;
        }
        auto splitted = split(content, ',', true, true);
        for (int i = 0; i < splitted.size(); i++) {
            auto keyval = split(splitted[i], ':', true, true);
            keyval[0].erase(keyval[0].begin());
            keyval[0].erase(keyval[0].end()-1);
            containers->push_back(JSONDecodeContainer(keyval[0], keyval[1], containers));
            childrenIndexes.push_back(containers->size()-1);
        }
    }
}

JSONDecodeContainer::JSONDecodeContainer(string content, vector<JSONDecodeContainer>* containers) : JSONDecodeContainer::JSONDecodeContainer("main", content, containers) {}

string JSONContainer::content() {
    return "";
}

JSONEncodeContainer JSONEncoder::container(CodingKey keys[], unsigned keysAmount) {
    return JSONEncodeContainer();
}

JSONDecodeContainer JSONDecoder::container(string content) {
    return JSONDecodeContainer(content, &containers);
}

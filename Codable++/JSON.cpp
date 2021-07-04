#include <string>
#include "Codable.h"
#include "JSON.h"
#include <vector>
using namespace std;

JSONEncodeContainer::JSONEncodeContainer() {
    
}

void jsonDebeautify(string* content) {
    bool isInValue = false;
    string contentCopy = content->data();
    for(int i=0;i<content->length();i++) {
        if((contentCopy[i] == ' ' || contentCopy[i] == '\n') && !isInValue) {
            contentCopy.erase(contentCopy.begin() + i--);
        }
        if(contentCopy[i] == '\"') {
            isInValue = !isInValue;
        }
    }
    content = &contentCopy;
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

JSONDecodeContainer::JSONDecodeContainer(string content) {
    jsonDebeautify(&content);
    if(content.length() < 2) {
        return;
    } else {
        if(content[0] == '{') {
            content.erase(content.begin());
            content.erase(content.end()-1);
        }
        auto splitted = split(content, ',', true, true);
        for(int i=0;i<splitted.size();i++) {
            auto keyval = split(splitted[i], ':', true, true);
            //
        }
    }
}

string JSONContainer::content() {
    return "";
}

JSONEncodeContainer JSONEncoder::container(CodingKey keys[], unsigned keysAmount) {
    return JSONEncodeContainer();
}

JSONDecodeContainer JSONDecoder::container(string content, CodingKey keys[], unsigned keysAmount) {
    return JSONDecodeContainer(content);
}

#ifndef CODABLE_H
#define CODABLE_H
#include <string>
typedef std::string CodingKey;

enum class CoderType {
    json
};

class CoderContainer {
public:
    CoderType type;
    
    virtual ~CoderContainer() {}
};

class Encoder {
public:
    CoderType type;
    
    virtual ~Encoder() {}
};

class Decoder {
public:
    CoderType type;
};

class Codable {
public:
    virtual void encode(CoderContainer* container) {};
    virtual void decode(CoderContainer* container) {};
};

#endif

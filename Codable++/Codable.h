#ifndef CODABLE_H
#define CODABLE_H
#include <string>
typedef std::string CodingKey;

enum CoderType {
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
    virtual void encode(Encoder* encoder) = 0;
    virtual void decode(CoderContainer* container) = 0;
};

#endif

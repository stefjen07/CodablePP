#ifndef CODABLE_H
#define CODABLE_H
#include <string>
typedef std::string CodingKey;

class CoderContainer {
    
};

enum CoderType {
    json
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
};

#endif

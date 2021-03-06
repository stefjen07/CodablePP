#ifndef CODABLE_H
#define CODABLE_H

#include <string>
typedef std::string CodingKey;

//Types of encoders/decoders
enum class CoderType {
    json
};

//Container for encoding/decoding
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

//Codable protocol
class Codable {
public:
    virtual void encode(CoderContainer* container) {};
    virtual void decode(CoderContainer* container) {};
};

#endif

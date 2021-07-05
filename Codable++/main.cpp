#include "JSON.h"
#include "Codable.h"
#include <iostream>
using namespace std;

const unsigned keysAmount = 2;

CodingKey keys[] = {
    "name",
    "phoneNumber"
};

class PhoneNumber: public Codable {
public:
    
    void encode(CoderContainer* container) {
        if(container->type == CoderType::json) {
            JSONEncodeContainer* jsonContainer = dynamic_cast<JSONEncodeContainer*>(container);
            jsonContainer->encode(countryCode, "country");
            jsonContainer->encode(number, "number");
        }
    }
    
    void decode(CoderContainer* container) {
        if(container->type == CoderType::json) {
            JSONDecodeContainer *jsonContainer = dynamic_cast<JSONDecodeContainer*>(container);
            this->countryCode = jsonContainer->decode(string(), "country");
            this->number = jsonContainer->decode(string(), "number");
        }
    }
    
    string countryCode;
    string number;
    
    PhoneNumber() {}
    
    PhoneNumber(string countryCode, string number) {
        this->countryCode = countryCode;
        this->number = number;
    }
};

class Contact: public Codable {
public:
    string name;
    PhoneNumber phoneNumber;
    
    void encode(CoderContainer* container) {
        if(container->type == CoderType::json) {
            JSONEncodeContainer* jsonContainer = dynamic_cast<JSONEncodeContainer*>(container);
            jsonContainer->encode(name, "name");
            jsonContainer->encode(phoneNumber, "phoneNumber");
        }
    }
    
    void decode(CoderContainer* container) {
        if(container->type == CoderType::json) {
            JSONDecodeContainer *jsonContainer = dynamic_cast<JSONDecodeContainer*>(container);
            name = jsonContainer->decode(string(), "name");
            phoneNumber = jsonContainer->decode(PhoneNumber(), "phoneNumber");
        }
    }

    Contact() {}

    Contact(string name, PhoneNumber phoneNumber) {
        this->name = name;
        this->phoneNumber = phoneNumber;
    }
};

int main() {
	JSONEncoder encoder;
    auto encodeContainer = encoder.container(keys, keysAmount);
    encodeContainer.encode(Contact("Eugene", PhoneNumber("123","456789")), "contact");
    cout << encodeContainer.content() << endl;
	JSONDecoder decoder;
	auto container = decoder.container("{\"name\": \"Eugene\", \"phoneNumber\": {\"country\": \"123\", \"number\": \"123456789\"}}");
    cout << container.content << endl;
    auto contact = container.decode(Contact());
    cout << contact.name << " " << contact.phoneNumber.countryCode << " " << contact.phoneNumber.number;
	return 0;
}

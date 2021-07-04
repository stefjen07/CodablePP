#include "JSON.h"
#include "Codable.h"
#include <iostream>
using namespace std;

const unsigned keysAmount = 2;

CodingKey keys[] = {
    "name",
    "phoneNumber"
};

class Contact: public Codable {
public:
    
    void encode(Encoder* encoder) {
        if(encoder->type == CoderType::json) {
            JSONEncoder *jsonEncoder = dynamic_cast<JSONEncoder*>(encoder);
            JSONEncodeContainer container = jsonEncoder->container(keys, keysAmount);
            container.encode(name, "name");
            container.encode(phoneNumber, "phoneNumber");
        }
    }
    
    void decode(CoderContainer* container) {
        if(container->type == CoderType::json) {
            JSONDecodeContainer *jsonContainer = dynamic_cast<JSONDecodeContainer*>(container);
            this->name = jsonContainer->decode(string(), "name");
            this->phoneNumber = jsonContainer->decode(string(), "phoneNumber");
        }
    }
    
    string phoneNumber;
    string name;

    Contact() {
        
    }

    Contact(string name, string phoneNumber) {
        this->name = name;
        this->phoneNumber = phoneNumber;
    }
};

int main() {
	JSONEncoder encoder;
    auto encodeContainer = encoder.container(keys, keysAmount);
    encodeContainer.encode(Contact("Eugene", "123456789"), "contact");
    cout << encodeContainer.content() << endl;
	JSONDecoder decoder;
	auto container = decoder.container("{\"name\": \"Eugene\", \"phoneNumber\": \"123456789\"}");
    cout << container.content << endl;
    auto contact = container.decode(Contact(), "contact");
    cout << contact.name << " " << contact.phoneNumber;
	return 0;
}

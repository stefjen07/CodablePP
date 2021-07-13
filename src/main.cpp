#include "JSON.h"
#include "Codable.h"
#include <iostream>
using namespace std;

class PhoneNumber: public Codable {
public:
    int countryCode;
    int number;
    
    void encode(CoderContainer* container) {
        if(container->type == CoderType::json) {
            JSONEncodeContainer* jsonContainer = dynamic_cast<JSONEncodeContainer*>(container);
            jsonContainer->encode(countryCode, "country");
            jsonContainer->encode(number, "number");
        }
    }
    
    void decode(CoderContainer* container) {
        if(container->type == CoderType::json) {
            JSONDecodeContainer* jsonContainer = dynamic_cast<JSONDecodeContainer*>(container);
            this->countryCode = jsonContainer->decode(int(), "country");
            this->number = jsonContainer->decode(int(), "number");
        }
    }
    
    PhoneNumber() {}
    
    PhoneNumber(int countryCode, int number) {
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

class PhoneBook: public Codable {
public:
    vector<Contact> contacts;

    void encode(CoderContainer* container) {
        if (container->type == CoderType::json) {
            JSONEncodeContainer* jsonContainer = dynamic_cast<JSONEncodeContainer*>(container);
            jsonContainer->encode(contacts, "contacts");
        }
    }

    void decode(CoderContainer* container) {
        if (container->type == CoderType::json) {
            JSONDecodeContainer* jsonContainer = dynamic_cast<JSONDecodeContainer*>(container);
            contacts = jsonContainer->decode(vector<Contact>(), "contacts");
        }
    }

    PhoneBook(vector<Contact> contacts) {
        this->contacts = contacts;
    }

    PhoneBook() {}
};

int main() {
    Contact eugene = Contact("Eugene", PhoneNumber(123, 456789));
    vector<Contact> contacts = { eugene };
    PhoneBook book(contacts);
	JSONEncoder encoder;
    auto encodeContainer = encoder.container();
    encodeContainer.encode(book);
    cout << encodeContainer.content << endl;
	JSONDecoder decoder;
	auto container = decoder.container(encodeContainer.content);
    auto decodeBook = container.decode(PhoneBook());
    for (int i = 0; i < decodeBook.contacts.size(); i++) {
        Contact contact = decodeBook.contacts[i];
        cout << contact.name << " " << contact.phoneNumber.countryCode << " " << contact.phoneNumber.number << endl;
    }
	return 0;
}

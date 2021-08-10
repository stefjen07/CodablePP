//Example of using Codable protocol and JSON encoder/decoder
//Description: Encoding phone book class to JSON string, then decoding and printing all the contacts in the book. 

#include "Codable.hpp"
#include "JSON.hpp"
#include <iostream>
using namespace std;

//Phone number class
//Consists of:
// countryCode - a phone code of the country
// number - a phone number without phone country code
// encode/decode methods for Codable protocol
class PhoneNumber: public Codable {
public:
    int countryCode;
    int number;
    
    //Encode method for Codable protocol
    void encode(CoderContainer* container) {
        //Checking for container type (there is only JSON type by default)
        if(container->type == CoderType::json) {
            //Converting CoderContainer to JSONEncodeContainer, because we need to use JSON only methods for encoding.
            JSONEncodeContainer* jsonContainer = dynamic_cast<JSONEncodeContainer*>(container);
            //Encoding country code to container using key "country"
            jsonContainer->encode(countryCode, "country");
            //Encoding phone number to container using key "number"
            jsonContainer->encode(number, "number");
        }
    }
    
    //Decode method for Codable protocol
    void decode(CoderContainer* container) {
        //Checking for container type (there is only JSON type by default)
        if(container->type == CoderType::json) {
            //Converting CoderContainer to JSONDecodeContainer, because we need to use JSON only methods for decoding.
            JSONDecodeContainer* jsonContainer = dynamic_cast<JSONDecodeContainer*>(container);
            //Decoding country code from container using key "country"
            this->countryCode = jsonContainer->decode(int(), "country");
            //Decoding phone number from container using key "number"
            this->number = jsonContainer->decode(int(), "number");
        }
    }
    
    //This constructor will be used for providing decoder with data type.
    PhoneNumber() {}
    
    PhoneNumber(int countryCode, int number) {
        this->countryCode = countryCode;
        this->number = number;
    }
};

//Phone number class
//Consists of:
// name - contact's name
// phoneNumber - a phone number as PhoneNumber class
// encode/decode methods for Codable protocol
class Contact: public Codable {
public:
    string name;
    PhoneNumber phoneNumber;
    
    //Encode method for Codable protocol
    void encode(CoderContainer* container) {
        //Checking for container type (there is only JSON type by default)
        if(container->type == CoderType::json) {
            //Converting CoderContainer to JSONEncodeContainer, because we need to use JSON only methods for encoding.
            JSONEncodeContainer* jsonContainer = dynamic_cast<JSONEncodeContainer*>(container);
            //Encoding contact's name to container using key "name"
            jsonContainer->encode(name, "name");
            //Encoding phone number to container using key "phoneNumber"
            jsonContainer->encode(phoneNumber, "phoneNumber");
        }
    }
    
    //Decode method for Codable protocol
    void decode(CoderContainer* container) {
        //Checking for container type (there is only JSON type by default)
        if(container->type == CoderType::json) {
            //Converting CoderContainer to JSONDecodeContainer, because we need to use JSON only methods for decoding.
            JSONDecodeContainer *jsonContainer = dynamic_cast<JSONDecodeContainer*>(container);
            //Decoding contact's name from container using key "name"
            name = jsonContainer->decode(string(), "name");
            //Decoding phone number from container using key "phoneNumber"
            phoneNumber = jsonContainer->decode(PhoneNumber(), "phoneNumber");
        }
    }

    //This constructor will be used for providing decoder with data type.
    Contact() {}

    Contact(string name, PhoneNumber phoneNumber) {
        this->name = name;
        this->phoneNumber = phoneNumber;
    }
};

//Phone book class
//Consists of:
// contacts - array of contacts in phone book
// encode/decode methods for Codable protocol
class PhoneBook: public Codable {
public:
    vector<Contact> contacts;

    //Encode method for Codable protocol
    void encode(CoderContainer* container) {
        //Checking for container type (there is only JSON type by default)
        if (container->type == CoderType::json) {
            //Converting CoderContainer to JSONEncodeContainer, because we need to use JSON only methods for encoding.
            JSONEncodeContainer* jsonContainer = dynamic_cast<JSONEncodeContainer*>(container);
            //Encoding contacts array to container using key "contacts"
            jsonContainer->encode(contacts, "contacts");
        }
    }

    //Decode method for Codable protocol
    void decode(CoderContainer* container) {
        //Checking for container type (there is only JSON type by default)
        if (container->type == CoderType::json) {
            //Converting CoderContainer to JSONDecodeContainer, because we need to use JSON only methods for decoding.
            JSONDecodeContainer* jsonContainer = dynamic_cast<JSONDecodeContainer*>(container);
            //Decoding contacts array from container using key "contacts"
            contacts = jsonContainer->decode(vector<Contact>(), "contacts");
        }
    }

    //This constructor will be used for providing decoder with data type.
    PhoneBook() {}

    PhoneBook(vector<Contact> contacts) {
        this->contacts = contacts;
    }
};

int main() {
    //Creating contact with name Eugene and phone number 123456789
    Contact eugene = Contact("Eugene", PhoneNumber(123, 456789));
    //Creating contacts array with contact Eugene
    vector<Contact> contacts = { eugene };
    //Creating phone book from contacts array
    PhoneBook book(contacts);
    //Initializing JSON encoder
	JSONEncoder encoder;
    //Getting container for encoding
    auto encodeContainer = encoder.container();
    //Encoding phone book to container
    encodeContainer.encode(book);
    //Printing encoded content
    cout << encodeContainer.content << endl;
    //Initializing JSON decoder
	JSONDecoder decoder;
    //Getting container for decoding of encoded content
	auto container = decoder.container(encodeContainer.content);
    //Decoding encoded content to variable
    auto decodeBook = container.decode(PhoneBook());
    //Printing all contacts' info
    for (int i = 0; i < decodeBook.contacts.size(); i++) {
        Contact contact = decodeBook.contacts[i];
        cout << contact.name << " " << contact.phoneNumber.countryCode << " " << contact.phoneNumber.number << endl;
        if(contact.name != book.contacts[i].name)
            return 1;
    }
	return 0;
}

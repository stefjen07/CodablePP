//Example of using Codable protocol and JSON encoder/decoder
//Description: Encoding phone book class to JSON string, then decoding and printing all the contacts in the book. 

#define _USE_MATH_DEFINES

#include "Codable.hpp"
#include "JSON.hpp"
#include <iostream>
#include <math.h>
using namespace std;

const long long floating_check_size = pow(10, 4);

bool round_equal(double val, double ref) {
    val *= floating_check_size;
    val = floor(val) / floating_check_size;

    ref *= floating_check_size;
    ref = floor(ref) / floating_check_size;

    return val == ref;
}

//Phone number class
//Consists of:
// countryCode - a phone code of the country
// number - a phone number without phone country code
// encode/decode methods for Codable protocol
class PhoneNumber: public Codable {
public:
    int country_code;
    long long number;
    float last_signal_level;
    
    //Encode method for Codable protocol
    void encode(CoderContainer* container) {
        //Checking for container type (there is only JSON type by default)
        if(container->type == CoderType::json) {
            //Converting CoderContainer to JSONEncodeContainer, because we need to use JSON only methods for encoding.
            JSONEncodeContainer* jsonContainer = dynamic_cast<JSONEncodeContainer*>(container);
            //Encoding phone number's info to container
            jsonContainer->encode(country_code, "country");
            jsonContainer->encode(number, "number");
            jsonContainer->encode(last_signal_level, "signal");
        }
    }
    
    //Decode method for Codable protocol
    void decode(CoderContainer* container) {
        //Checking for container type (there is only JSON type by default)
        if(container->type == CoderType::json) {
            //Converting CoderContainer to JSONDecodeContainer, because we need to use JSON only methods for decoding.
            JSONDecodeContainer* jsonContainer = dynamic_cast<JSONDecodeContainer*>(container);
            //Decoding phone number's info from container
            this->country_code = jsonContainer->decode(int(), "country");
            this->number = jsonContainer->decode(0LL, "number");
            this->last_signal_level = jsonContainer->decode(float(), "signal");
        }
    }
    
    //This constructor will be used for providing decoder with data type.
    PhoneNumber() {}
    
    PhoneNumber(int country_code, long long number, float last_signal_level) {
        this->country_code = country_code;
        this->number = number;
        this->last_signal_level = last_signal_level;
    }
};

bool check_number(PhoneNumber number, PhoneNumber ref) {
    if(number.number != ref.number) {
        cerr << "[Phone number check]: numbers' values are different\n";
        return false;
    }
    if(number.country_code != ref.country_code) {
        cerr << "[Phone number check]: numbers' country codes are different\n";
        return false;
    }
    if(!round_equal(number.last_signal_level, ref.last_signal_level)) {
        cerr << "[Phone number check]: numbers' last signals are different: " << number.last_signal_level << ' ' << ref.last_signal_level << '\n';
        return false;
    }
    return true;
}

//Phone number class
//Consists of:
// name - contact's name
// phoneNumber - a phone number as PhoneNumber class
// encode/decode methods for Codable protocol
class Contact: public Codable {
public:
    string name;
    PhoneNumber phone_number;
    bool is_valid;
    
    //Encode method for Codable protocol
    void encode(CoderContainer* container) {
        //Checking for container type (there is only JSON type by default)
        if(container->type == CoderType::json) {
            //Converting CoderContainer to JSONEncodeContainer, because we need to use JSON only methods for encoding.
            JSONEncodeContainer* jsonContainer = dynamic_cast<JSONEncodeContainer*>(container);
            //Encoding contact's info to container
            jsonContainer->encode(name, "name");
            jsonContainer->encode(phone_number, "phone_number");
            jsonContainer->encode(is_valid, "is_valid");
        }
    }
    
    //Decode method for Codable protocol
    void decode(CoderContainer* container) {
        //Checking for container type (there is only JSON type by default)
        if(container->type == CoderType::json) {
            //Converting CoderContainer to JSONDecodeContainer, because we need to use JSON only methods for decoding.
            JSONDecodeContainer *jsonContainer = dynamic_cast<JSONDecodeContainer*>(container);
            //Decoding contact's info from container
            name = jsonContainer->decode(string(), "name");
            phone_number = jsonContainer->decode(PhoneNumber(), "phone_number");
            is_valid = jsonContainer->decode(bool(), "is_valid");
        }
    }

    //This constructor will be used for providing decoder with data type.
    Contact() {}

    Contact(string name, PhoneNumber phone_number, bool is_valid) {
        this->name = name;
        this->phone_number = phone_number;
        this->is_valid = is_valid;
    }
};

bool check_contact(Contact contact, Contact ref) {
    if(contact.name != ref.name) {
        cerr << "[Contact check]: contacts' names are different\n";
        return false;
    }
    if(contact.is_valid != ref.is_valid) {
        cerr << "[Contact check]: contacts' validation statuses are different\n";
        return false;
    }
    if(!check_number(contact.phone_number, ref.phone_number)) {
        cerr << "[Contact check]: contacts' phone numbers are different\n";
        return false;
    }
    return true;
}

//Phone book class
//Consists of:
// contacts - array of contacts in phone book
// encode/decode methods for Codable protocol
class PhoneBook: public Codable {
public:
    vector<Contact> contacts;
    double time_spent;

    //Encode method for Codable protocol
    void encode(CoderContainer* container) {
        //Checking for container type (there is only JSON type by default)
        if (container->type == CoderType::json) {
            //Converting CoderContainer to JSONEncodeContainer, because we need to use JSON only methods for encoding.
            JSONEncodeContainer* jsonContainer = dynamic_cast<JSONEncodeContainer*>(container);
            //Encoding phone book's info to container
            jsonContainer->encode(contacts, "contacts");
            jsonContainer->encode(time_spent, "time_spent");
        }
    }

    //Decode method for Codable protocol
    void decode(CoderContainer* container) {
        //Checking for container type (there is only JSON type by default)
        if (container->type == CoderType::json) {
            //Converting CoderContainer to JSONDecodeContainer, because we need to use JSON only methods for decoding.
            JSONDecodeContainer* jsonContainer = dynamic_cast<JSONDecodeContainer*>(container);
            //Decoding phone book's info from container
            contacts = jsonContainer->decode(vector<Contact>(), "contacts");
            time_spent = jsonContainer->decode(double(), "time_spent");
        }
    }

    //This constructor will be used for providing decoder with data type.
    PhoneBook() {}

    PhoneBook(vector<Contact> contacts, double time_spent) {
        this->contacts = contacts;
        this->time_spent = time_spent;
    }
};

bool check_book(PhoneBook book, PhoneBook ref) {
    if(book.contacts.size() != ref.contacts.size()) {
        cerr << "[Phonebook check]: Contacts arrays' sizes are different\n";
        return false;
    }
    if(!round_equal(book.time_spent, ref.time_spent)) {
        cerr << "[Phonebook check]: Time spent values are different: " << book.time_spent << " " << ref.time_spent << '\n';
        return false;
    }
    for(int i=0;i<book.contacts.size();i++) {
        if(!check_contact(book.contacts[i], ref.contacts[i])) {
            cerr << "[Phonebook check]: Contacts with index " << i << " are different" << '\n';
            return false;
        }
    }
    return true;
}

int main() {
    //Creating contact with name Eugene and phone number 123456789
    Contact eugene = Contact("Eugene", PhoneNumber(123, 456789, M_SQRT2), true);
    //Creating contacts array with contact Eugene
    vector<Contact> contacts = { eugene };
    //Creating phone book from contacts array
    PhoneBook book(contacts, M_PI);
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
    auto decode_book = container.decode(PhoneBook());
    //Checking decoded book
    if (!check_book(decode_book, book)) {
        return 1;
    }
    //Printing all contacts' info
    for (int i = 0; i < decode_book.contacts.size(); i++) {
        Contact contact = decode_book.contacts[i];
        if(i == 0) {
            if(contact.phone_number.country_code != 123) {
                cerr << "Country code was modified during encoding/decoding";
                return 1;
            }
        }
        cout << contact.name << " " << contact.phone_number.country_code << " " << contact.phone_number.number << endl;
    }
	return 0;
}

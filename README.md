# CodablePP
[![Build Status](https://travis-ci.com/stefjen07/CodablePP.svg?branch=main)](https://travis-ci.com/stefjen07/CodablePP)
<br>
This C++ library provides Codable protocol from Swift.<br>
It may be useful if you have C++ server for iOS or macOS application.<br>
Library methods have very similar syntax to Swift's version.
## Documentation
Before using encoding and decoding methods and Codable class, you must import 2 headers:<br>
```c++
#include "Codable.h"
#include "JSON.h"
```
If you want to make class Codable as you do in Swift, you must choose Codable class as a base class.<br>
For example, if you have class PhoneBook, then its Codable version will look like this:<br>
```c++
class PhoneBook: public Codable {};
```
Also you must provide class with implementations of 2 virtual methods as you do in Swift and 1 initializer without arguments:<br>
```c++
void encode(CoderContainer* container) {}
void decode(CoderContainer* container) {}
PhoneBook() {}
```
By default, there is only one Coder type - JSON. But you have to check for type and perform `dynamic_cast`.
Here is an example for our class PhoneBook:
```c++
void encode(CoderContainer* container) {
    if (container->type == CoderType::json) {
        JSONEncodeContainer* jsonContainer = dynamic_cast<JSONEncodeContainer*>(container);
        jsonContainer->encode(contacts, "contacts");
    }
}
```
After that we can encode everything we want. In the example above we encode contacts of phone book.<br>
The `contacts` variable has type `vector<Contact>`. Contact class also must have Codable class as a base class.<br>
Notice that this library supports only vectors for JSON arrays. Simple arrays won't be eligible for that.
Decoding has the same logic:
```c++
void decode(CoderContainer* container) {
    if (container->type == CoderType::json) {
        JSONDecodeContainer* jsonContainer = dynamic_cast<JSONDecodeContainer*>(container);
        contacts = jsonContainer->decode(vector<Contact>(), "contacts");
    }
}
```
We must provide decoder with any variable with type we want to decode. In our case it is `vector<Contact>()`.<br>
The second argument in both encoding and decoding methods is key for field in JSON. It must match with key in client-application in Swift.<br><br>
So, we made our Phonebook class Codable. Now we can encode it using JSONEncoder as easy as in Swift
```c++
JSONEncoder encoder;
auto encodeContainer = encoder.container();
encodeContainer.encode(book);
cout << encodeContainer.content << endl;
```
and decode it back to Phonebook class
```c++
JSONDecoder decoder;
auto container = decoder.container(encodeContainer.content);
auto decodeBook = container.decode(PhoneBook());
```

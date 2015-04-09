#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <memory>
#include <string>
#include <initializer_list>


struct Pair {
    Pair(std::string key, std::string value) : key{key}, value{value} {}
    Pair(std::string key, int value) : key{key}, value{std::to_string(value)} {}

    std::string key;
    std::string value;
};

class Payload {
  public:
    Payload(std::initializer_list<Pair> pairs);
    Payload(std::string content) : content{content} {}

    std::string content;
};

#endif

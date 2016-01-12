#include <iostream>
#include <serializer.hpp>
#include "serializers/json.hpp"
#include "serializers/dcm_buf.hpp"

using namespace std;
using namespace literals::string_literals;
int main() {
    serializers::serializer::ptr s = serializers::dcm_buf::create();

    s->at("i")->set(1);
    s->at("s")->set("s");
    s->at("b")->set(true);
    s->at("f")->set(1.1);
    s->at("a")->set({1,"a",1.1,true});
    s->at("o")->set({{"a", 1}, {"b", 1}});

    std::string buf = s->stringify();

    std::cout << buf << std::endl;

    return 0;
}
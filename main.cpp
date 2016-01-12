#include <iostream>
#include <serializer.hpp>
#include "serializers/json.hpp"
#include "serializers/dcm_buf.hpp"

using namespace std;
using namespace literals::string_literals;
int main() {
    serializers::serializer::ptr s = serializers::dcm_buf::create_object();

    s->emplace("i", 1);
    s->emplace("s", "s");
    s->emplace("b", true);
    s->emplace("f", 1.1);
    s->emplace("a", {1,"a",1.1,true});
    s->emplace("o", {{"a", 1}, {"b", 1}});

    std::cout << s->get("s", ":(")->as_string() << std::endl;

    std::string buf = s->stringify();

    std::cout << buf << std::endl;

    return 0;
}
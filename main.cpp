#include <iostream>
#include <serializer.hpp>

using namespace std;
using namespace literals::string_literals;
int main() {
    auto s = bp::serializer::create(bp::serializers::type::Dcm);

    s->emplace("i", 1);
    s->emplace("s", "string");
    s->emplace("b", true);
    s->emplace("f", 1.1);
    s->emplace("a", {1,"a",1.1,true});
    s->emplace("o", {{"a", 1}, {"b", 1}});

//    std::cout << s->get("s", ":(")->as_string() << std::endl;

    std::string buf = s->stringify();

    std::cout << buf << std::endl;

    return 0;
}
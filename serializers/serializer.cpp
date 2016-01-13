#include "serializer.hpp"
#include "json.hpp"
#include "dcm_buf.hpp"

bp::serializer::ptr bp::serializer::create(bp::serializers::type _type){
    switch (_type) {
        case serializers::type::Json:
            return serializers::json::create();
        case serializers::type::Dcm:
            return serializers::dcm_buf::create();
        default:
            throw std::invalid_argument("no such serializer");
    }
}

#ifndef SERIALIZERS_JSON_HPP
#define SERIALIZERS_JSON_HPP

#include <memory>
#include <json/json.h>
#include "serializer.hpp"

namespace serializers {
    class json : public serializer, public std::enable_shared_from_this<json> {
        Json::Value*    root_;
        value_type      value_type_;
        bool            ref_ = false;

        json();
        json(Json::Value& _val);

        static serializer::ptr create(Json::Value& _val);
    public:
        ~json();

        static serializer::ptr create();

        virtual serializer::ptr get(const std::string &_key, const value &_default) const override;
        virtual serializer::ptr get(const std::string &_key, value &&_default) const override;

        virtual void append(const value &_val) override;
        virtual void append(value &&_val) override;
        virtual bool emplace(const std::string &_key, const value &_val) override;
        virtual bool emplace(const std::string &_key, value &&_val) override;

        virtual std::string as_string() const override;
        virtual double as_float() const override;
        virtual serializable::int_t as_int() const override;
        virtual bool as_bool() const override;

        virtual std::string stringify() const override;
        virtual void parse(const std::string &_str) override;

        virtual size_t size() const override;

        virtual serializer::ptr at(int index) override;
        virtual const serializer::ptr at(int index) const override;
        virtual serializer::ptr at(const char *_key) override;
        virtual const serializer::ptr at(const char *_key) const override;

        virtual serializer::ptr set(value &&_val) override;
        virtual serializer::ptr set(const value &_val) override;
        virtual serializer::ptr set(const std::initializer_list<value> &_val) override;
        virtual serializer::ptr set(const std::initializer_list<std::pair<std::string, value>> &_val) override;

        operator serializer::ptr();
    };
}

#endif //SERIALIZERS_JSON_HPP

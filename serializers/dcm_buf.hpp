#ifndef SERIALIZERS_DCM_BUF_HPP
#define SERIALIZERS_DCM_BUF_HPP

#include <unordered_map>
#include <dcm/interprocess/core/util.hpp>
#include "serializer.hpp"

namespace serializers {
    class dcm_buf : public serializers::serializer, public std::enable_shared_from_this<dcm_buf>{
        using obj_t = std::unordered_map<interproc::symbol_t, value>;
        using arr_t = std::vector<value>;

        boost::variant<value, obj_t, arr_t> val_;

    public:

        virtual std::string as_string() const override;

        virtual serializable::int_t as_int() const override;

        virtual serializable::float_t as_float() const override;

        virtual size_t size() const override;

        virtual serializer::ptr at(int index) override;

        virtual const serializer::ptr at(int index) const override;

        virtual void append(const value &_val) override;

        virtual void append(value &&_val) override;

        virtual serializer::ptr at(const char *_key) override;

        virtual const serializer::ptr at(const char *_key) const override;

        virtual bool emplace(const std::string &_key, const value &_val) override;

        virtual bool emplace(const std::string &_key, value &&_val) override;

        virtual serializer::ptr get(const std::string &_key, const value &_default) const override;

        virtual serializer::ptr get(const std::string &_key, value &&_default) const override;

        virtual serializer::ptr set(value &&_val) override;

        virtual serializer::ptr set(const value &_val) override;

        virtual serializer::ptr set(const std::initializer_list<value> &_val) override;

        virtual serializer::ptr set(const std::initializer_list<std::pair<std::string, value>> &_val) override;

        virtual std::string stringify() const override;

        virtual void parse(const std::string &_str) override;
    };
}


#endif //SERIALIZERS_DCM_BUF_HPP

#ifndef NVAPI_SERIALIZED_VALUE_HPP
#define NVAPI_SERIALIZED_VALUE_HPP

#include <string>
#include <boost/variant.hpp>
#include <binelpro/symbol.hpp>

namespace bp {

    namespace serializable {
        using int_t = int32_t;
        using float_t = double;
        using c_str_t = const char*;
        using bool_t = bool;
    }

    namespace serializers {
        enum class type: symbol_t {
            Dcm = "dcmbuf"_sym,
            Json = "json"_sym
        };
    }

    class serializer {
    public:
        using ptr = std::shared_ptr<serializer>;
        using value = boost::variant<
                serializable::c_str_t,//TODO: remove
                serializable::int_t,
                serializable::float_t,
                serializable::bool_t,
                std::string>;

        enum class value_type: char {
            Int = 'i',
            Float = 'f',
            CString = 'c',
            String = 's',
            Bool = 'b',
            Bytes = 'r',
            Null = 'n',
            Object = 'o',
            Array = 'a'
        };
    private:
        value_type value_type_ = value_type::Null;
    protected:
        inline void set_type(value_type _type) {value_type_ = _type;};
        struct value_type_visitor : public boost::static_visitor<value_type> {
            value_type operator()(serializable::int_t _val) const { return value_type::Int; }
            value_type operator()(serializable::float_t _val) const { return value_type::Float; }
            value_type operator()(serializable::c_str_t _val) const { return value_type::CString; }
            value_type operator()(serializable::bool_t _val) const { return value_type::Bool; }
            value_type operator()(serializable::int_t &&_val) const { return value_type::Int; }
            value_type operator()(serializable::float_t &&_val) const { return value_type::Float; }
            value_type operator()(serializable::c_str_t &&_val) const { return value_type::CString; }
            value_type operator()(serializable::bool_t &&_val) const { return value_type::Bool; }
            value_type operator()(const std::string & _val) const { return value_type::String; }
            value_type operator()(std::string && _val) const { return value_type::String; }
        };
    public:


        virtual std::string as_string() const = 0;
        virtual serializable::int_t as_int() const = 0;
        virtual serializable::float_t as_float() const = 0;
        virtual bool as_bool() const = 0;

        inline bool is_int() const {
            return value_type_ == value_type::Int;
        }

        inline bool is_float() const {
            return value_type_ == value_type::Float ;
        }

        inline bool is_bool() const {
            return value_type_ == value_type::Bool;
        }

        inline bool is_string() const {
            return value_type_ == value_type::String ;
        }

        inline bool is_null() const {
            return value_type_ == value_type::Null;
        }

        inline bool is_array() const {
            return value_type_ == value_type::Array ;
        }

        inline bool is_object() const {
            return value_type_ == value_type::Object ;
        }

        inline value_type type() const { return value_type_; };

        // for arrays:
        virtual size_t size() const = 0;
        virtual serializer::ptr at(int index) = 0;
        virtual const serializer::ptr at(int index) const = 0;
        virtual void append(const value &_val) = 0;
        virtual void append(value &&_val) = 0;
        virtual bool append(const std::initializer_list<value> &_val) = 0;
        virtual bool append(const std::initializer_list<std::pair<std::string, value>> &_val) = 0;

        // objects:
        virtual serializer::ptr at(const char *_key) = 0;
        virtual const serializer::ptr at(const char *_key) const = 0;
        virtual bool emplace(const std::string &_key, const value &_val) = 0;
        virtual bool emplace(const std::string &_key, value &&_val) = 0;
        virtual bool emplace(const std::string &_key, const std::initializer_list<value> &_val) = 0;
        virtual bool emplace(const std::string &_key, const std::initializer_list<std::pair<std::string, value>> &_val) = 0;

        virtual serializer::ptr get(const std::string &_key, const value &_default) const = 0;
        virtual serializer::ptr get(const std::string &_key, value &&_default) const = 0;

        virtual serializer::ptr set(value &&_val) = 0;
        virtual serializer::ptr set(const value &_val) = 0;  // atom
        virtual serializer::ptr set(const std::initializer_list<value> &_val) = 0;   // array
        virtual serializer::ptr set(const std::initializer_list<std::pair<std::string, value>> &_val) = 0;   // object

        //
        virtual std::string stringify() const = 0;
        virtual void parse(const std::string &_str) = 0;

        // TODO: iterators for objects and arrays

        static serializer::ptr create(bp::serializers::type _type);
    };
}


#endif //NVAPI_SERIALIZED_VALUE_HPP

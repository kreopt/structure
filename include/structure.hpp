#ifndef BP_SERIALIZER_HPP
#define BP_SERIALIZER_HPP

#include <iterator>
#include <map>
#include <vector>
#include <memory>
#include <cstdint>
#include "symbol.hpp"
#include "util.hpp"
#include "variant.hpp"

#if defined( __EXCEPTIONS) || defined( _MSC_VER)
#define HAS_EXCEPTIONS
#define ENABLE_IF_HAS_EXCEPTIONS(x) x
#else
#define ENABLE_IF_HAS_EXCEPTIONS(x)
#endif

#if __cplusplus < 201400L
namespace std {
    template<typename T>
    using remove_reference_t = typename remove_reference<T>::type;

    template<typename T>
    using remove_cv_t = typename remove_cv<T>::type;

}
#endif

#define SERIALIZABLE_TYPES int,float,double,bool,std::string,const char*,nullptr_t

namespace bp {

    using namespace bp::literals;

    // TODO: generic tree to allow custom types

    namespace serializable {

        using value = bp::variant<SERIALIZABLE_TYPES>;
        using value_ptr = std::shared_ptr<value>;

        class tree;

        using tree_ptr = std::shared_ptr<tree>;

        using object = std::map<bp::hash_type, tree_ptr>;
        using object_ptr = std::shared_ptr<object>;

        using array = std::vector<tree_ptr>;
        using array_ptr = std::shared_ptr<array>;


        template<typename T>
        using is_serializable = bp::is_convertible_to<T,SERIALIZABLE_TYPES>;
        template<typename T>
        using enable_if_serializable_t = typename std::enable_if<is_serializable<T>::value>::type;

        template<typename T>
        using enable_if_variant_ptr_type_t = typename std::enable_if<bp::is_any_of<T, array_ptr, object_ptr, value_ptr>::value>::type;

        template<typename T>
        using is_variant_type = bp::is_any_of<std::remove_cv_t<std::remove_reference_t<T>>, value, object, array>;

        template<typename T>
        using enable_if_variant_type_t = typename std::enable_if<is_variant_type<T>::value>::type;

        template<typename T>
        using enable_if_serializable_or_var_t = typename std::enable_if<
                is_serializable<T>::value || is_variant_type<T>::value>::type;

        template<typename T>
        using plain_type_t = std::remove_reference_t<std::remove_cv_t<T>>;

        namespace {
            // TODO: DRY
            template<typename ValueType>
            struct serializable_trait {
                using type = value;
            };
            template<>
            struct serializable_trait<value> {
                using type = value;
            };
            template<>
            struct serializable_trait<object> {
                using type = object;
            };
            template<>
            struct serializable_trait<array> {
                using type = array;
            };
        }

        class tree : public bp::variant<value_ptr, object_ptr, array_ptr> {
            using base = bp::variant<value_ptr, object_ptr, array_ptr>;
        public:
            tree() : base(std::make_shared<value>(nullptr)) {}
            tree(int8_t _val) : base(std::make_shared<value>(static_cast<int>(_val))) {}
            tree(uint8_t _val) : base(std::make_shared<value>(static_cast<int>(_val))) {}
            tree(int16_t _val) : base(std::make_shared<value>(static_cast<int>(_val))) {}
            tree(uint16_t _val) : base(std::make_shared<value>(static_cast<int>(_val))) {}
            tree(int32_t _val) : base(std::make_shared<value>(static_cast<int>(_val))) {}
            tree(uint32_t _val) : base(std::make_shared<value>(static_cast<int>(_val))) {}

            tree(const bp::string_view &_val) : base( std::make_shared<value>(std::string(_val))) {}
            tree(const char* _val) : base( _val?std::make_shared<value>(std::string(_val, const_length(_val))): nullptr) {}

            template<typename ValueType, typename = enable_if_serializable_or_var_t<ValueType>>
            tree(ValueType &&_val) :
                    base(
                            std::make_shared<typename serializable_trait<std::remove_reference_t<ValueType>>::type>(
                                    std::forward<ValueType>(_val))
                    ) {};

            template<typename ValueType, typename = enable_if_variant_type_t<ValueType>>
            tree(std::shared_ptr<ValueType> _val) : base(_val) {}

            inline array_ptr as_array() const {
                return bp::get<array_ptr>(*this);
            }

            inline object_ptr as_object() const {
                return bp::get<object_ptr>(*this);
            }

            inline value_ptr as_value() const {
                return bp::get<value_ptr>(*this);
            }

            template<typename T>
            T as() const {
                if (bp::holds_alternative<value_ptr>(*this)) {
                    return bp::get<T>(*this->as_value());
                } else {
                    return T();
                }
            }
        };
    }

    namespace {
        template<typename T, typename V>
        using enable_if_convertible_t = typename std::enable_if<std::is_convertible<std::remove_reference_t<T>, V>::value>::type;

        template<typename T>
        using enable_if_string = enable_if_convertible_t<T, bp::string_view>;
    }

    struct value_type_visitor;

    class structure {
    public:

//        friend class structure;
        using ptr = std::shared_ptr<structure>;
        using wptr = std::weak_ptr<structure>;

        // EXCEPTIONS
#ifdef HAS_EXCEPTIONS
        /**
         * Generic structure error
         */
        class structure_error : public std::runtime_error {
        public:
            structure_error(const char* _msg) : std::runtime_error(_msg){}
        };

        /**
         * Bad type usage error
         */
        class type_error: public structure_error {
        public:
            type_error(const char* _msg) : structure_error(_msg){}
        };

        /**
         * Structure parse error
         */
        class parse_error : public structure_error {
        public:
            parse_error(const char* _msg) : structure_error(_msg){}
        };
#endif

        /**
         * Possible structure element value types
         */
        enum class value_type : char {
            Int = 'i',
            Float = 'f',
            String = 's',
            Bool = 'b',
            Null = 'n',
            Object = 'o',
            Array = 'a'
        };

        // CONSTRUCTORS

    private:
        structure(serializable::tree_ptr _obj);

    public:
        /**
         * Create structure with root of specified type
         * @param _t type of structure root element
         */
        structure(value_type _t);

        /**
         * Create structure from primitive type
         * @param _val primitive value
         */
        template<typename T, typename = serializable::enable_if_serializable_t<T>>
        structure(T &&_val) : value_type_(value_type_visitor()(_val)),
                              val_(std::make_shared<serializable::tree>(std::forward<T>(_val))) {}

        /**
         * Create null structure
         */
        structure();

        /**
         * Alias structure. Only pointer is copied, not data. Use deepcopy() for data clone
         * @param _s structure to alias
         */
        structure(const structure &_s);

        /**
         * Move structure
         * @param _s structure to move
         */
        structure(structure &&_s);

        /**
         * Initialize structure with array of serializable items
         * @param _val array of serializable items
         */
        structure(const std::initializer_list<bp::serializable::tree> &_val);   // array

        /**
         * Initialize structure with associative array of serializable items
         * @param _val associative array of serializable items
         */
        structure(const std::initializer_list<std::pair<bp::string_view, bp::structure>> &_val);   // object
        structure(const std::initializer_list<std::pair<bp::hash_type, bp::structure>> &_val); // object

        // OPERATORS

        /**
         * Access array element. Only for array-typed structures. Throws type_error for other types.
         * @param index - array index. Use negative indexes to access array from the end
         * @return item at index or null structure in case of failure in exceptionless mode
         */
        structure operator[](int index) ENABLE_IF_HAS_EXCEPTIONS(throw (type_error));

        /**
         * Access object item with key name specified. If key not found, new null-valued item
         * will be created. Throws type_error for types other than object.
         * @param _key object key to be accessed
         * @return object item or null-typed structure in exceptionless mode
         */
        structure operator[](const bp::hash_type &_key) ENABLE_IF_HAS_EXCEPTIONS(throw (type_error));
        inline structure operator[](const bp::string_view &_key) ENABLE_IF_HAS_EXCEPTIONS(throw (type_error)) {
            return operator[](bp::symbol(_key).to_hash());
        }
//        inline structure operator[](const char *_key) ENABLE_IF_HAS_EXCEPTIONS(throw (type_error)) {
//            return operator[](bp::symbol(_key).to_hash());
//        }

        /**
         * Alias structure. Only pointer is copied, not data. Use deepcopy() for data clone
         * @param _str structure to alias
         * @return structure reference
         */
        structure &operator=(const structure &_str);

        /**
         * Move structure
         * @param _str structure to move
         * @return structure reference
         */
        structure &operator=(structure &&_str);

        /**
         * Assign structure with serializable value
         * @tparam ValType serializable type
         * @param _val serializable value
         * @return structure reference
         */
        template<typename ValType,
                 typename = enable_if_convertible_t<ValType, serializable::tree>>
        structure &operator=(ValType &&_val) {
            *val_ = serializable::tree(std::forward<ValType>(_val));
            value_type_ = get_variant_type(_val);
            return *this;
        }

        /**
         * Recursively check if structures are matching
         * @param _s structure to match
         * @return true if stuctures are equivalent
         */
        bool operator==(const bp::structure &_s) const;

        /**
         * Recursively check if structures does not match
         * @param _s structure to match
         * @return true if stuctures are different
         */
        inline bool operator!=(const bp::structure &_s) const { return !operator==(_s); };

        // GETTERS

        /**
         * Erase object item with key name specified. If key not found, std::out_of_range
         * will be thrown. Throws type_error for types other than object.
         * @param _key object key to be erased
         * @return true if erased or false in exceptionless mode
         */
        bool has_key(const bp::hash_type &_key) const ENABLE_IF_HAS_EXCEPTIONS(throw (type_error));

        /**
         * Access object item with key name specified. If key not found, std::out_of_range
         * will be thrown. Throws type_error for types other than object.
         * @param _key object key to be accessed
         * @return object item or null-typed structure in exceptionless mode
         */
        structure at(const bp::hash_type &_key) const ENABLE_IF_HAS_EXCEPTIONS(throw (type_error, std::out_of_range));

        /**
         * Access array item with index. If index not found, std::out_of_range
         * will be thrown. Throws type_error for types other than array.
         * @param _key object key to be accessed
         * @return object item or null-typed structure in exceptionless mode
         */
        structure at(int _key) const ENABLE_IF_HAS_EXCEPTIONS(throw (type_error, std::out_of_range));

        /**
         * Get primitive value
         * @tparam ValueType serializable primitive value
         * @param _key object key to get
         * @param _default value to be used if object has no such key
         * @return object item value or default value
         */
        template<typename ValueType,
                class = enable_if_convertible_t<ValueType, serializable::tree>>
        ValueType get(const bp::hash_type &_key, ValueType &&_default = ValueType()) const {    // nothrow??
            if (is_object()) {
                if (this->has_key(_key)) {
                    return this->at(_key).as<ValueType>();
                } else {
                    return _default;
                }
            }
#ifdef HAS_EXCEPTIONS
            throw type_error("not an object");
#endif
            return _default;
        };

        // SETTERS

        /**
         * Append value to array-typed structure. Throws type_error for other types.
         * @param _val serializable value
         * @return true if inserted or false in case of failure in exceptionless mode
         */
         // TODO: use universal refs
        bool append(const bp::structure &_val);

        /**
         * Clear array
         * @return true in success case
         */
        bool clear();

        /**
         * Reset to new value
         */
//        void reset(const bp::structure &_s);
        void reset();

    private:
        inline bool emplace_init() {
            initialize_if_null(value_type::Object);
            return is_object();
        }

        template<typename ValType,
                 typename = typename std::enable_if<std::is_same<serializable::plain_type_t<ValType>, serializable::tree>::value>::type
        >
        bool emplace_resolve(const bp::string_view &_key, const ValType &_var) {
            return emplace_resolve(bp::symbol(_key).to_hash(), _var);
        }

        template<typename ValType,
                typename = typename std::enable_if<std::is_same<serializable::plain_type_t<ValType>, serializable::tree>::value>::type
        >
        bool emplace_resolve(const bp::hash_type &_key, const ValType &_var) {
            return val_->as_object()->
                    emplace(_key, std::make_shared<serializable::tree>(_var)).second;
        }

        value_type get_variant_type(const serializable::tree &_var) const;
    public:

        /**
         * Construct new value in object with specified key.
         * @param _key object key to insert in
         * @param _val value to insert
         * @return true if insert take place, false on failure in exceptionless mode
         */
        // TODO: use universal refs
        bool emplace(const std::initializer_list<std::pair<bp::string_view, bp::structure>> &_val);
        bool emplace(const std::initializer_list<std::pair<bp::hash_type, bp::structure>> &_val);

        /**
         * Construct new structure clone in object with specified key.
         * @param _key
         * @param _str
         * @return
         */
        // TODO: use universal refs
        bool emplace(const bp::string_view &_key, const bp::structure &_str);
        bool emplace(const bp::hash_type &_key, const bp::structure &_str);

        /**
         * Erase object item with key name specified. If key not found, std::out_of_range
         * will be thrown. Throws type_error for types other than object.
         * @param _key object key to be erased
         * @return true if erased or false in exceptionless mode
         */
        bool erase(const bp::hash_type &_key) ENABLE_IF_HAS_EXCEPTIONS(throw (type_error, std::out_of_range));

        bp::structure& merge(const bp::structure &_s) ENABLE_IF_HAS_EXCEPTIONS(throw (type_error));

        // TYPES

        /**
         * Cast structure
         * @tparam ValueType
         * @return
         */
        template<typename ValueType,
                typename = typename std::enable_if<serializable::is_serializable<ValueType>::value>::type>
        inline ValueType as() const {
            return ValueType();
        };
    private:
        /**
         * Cast structure
         * @tparam ValueType
         * @return
         */
        template<typename T,
                typename = typename std::enable_if<std::is_convertible<T, int>::value>::type>
        inline T as_int() const {
            switch (type()) {
                case value_type::Int:
                    return static_cast<T>(val_->as<int>());
                case value_type::Float:
                    return static_cast<T>(val_->as<float>());
                case value_type::Bool:
                    return static_cast<T>(val_->as<bool>());
                default:
                    return 0;
            }
        };
    public:

        /**
         * Cast structure
         * @tparam ValueType
         * @return
         */
        template<typename ValueType,
                typename = typename std::enable_if<serializable::is_serializable<ValueType>::value>::type>
        inline bool is() const {
            return value_type_ == bp::visit(value_type_visitor(), ValueType());
        };

        /**
         * Check if structure holds integer value
         * @return true if structure is int
         */
        //[[ deprecated("use is<> instead") ]]
        inline bool is_int() const { return value_type_ == value_type::Int; }

        /**
         * Check if structure holds float value
         * @return true if structure is float
         */
        //[[ deprecated("use is<> instead") ]]
        inline bool is_float() const { return value_type_ == value_type::Float; }

        /**
         * Check if structure holds bool value
         * @return true if structure is bool
         */
        //[[ deprecated("use is<> instead") ]]
        inline bool is_bool() const { return value_type_ == value_type::Bool; }

        /**
         * Check if structure holds string value
         * @return true if structure is string
         */
        //[[ deprecated("use is<> instead") ]]
        inline bool is_string() const { return value_type_ == value_type::String; }

        /**
         * Check if structure is null
         * @return true if structure is null
         */
        inline bool is_null() const { return value_type_ == value_type::Null; }

        /**
         * Check if structure is array
         * @return true if structure is array
         */
        inline bool is_array() const { return value_type_ == value_type::Array; }

        /**
         * Check if structure is object
         * @return true if structure is object
         */
        inline bool is_object() const { return value_type_ == value_type::Object; }

        /**
         * Get structure type
         * @return structure type
         */
        inline value_type type() const { return value_type_; };

        // MISC

        /**
         * Clone structure recursively
         * @return cloned structure
         */
        structure deepcopy() const;

        /**
         * Create structure of type object
         * @return object-typed structure
         */
        static structure create_object();

        /**
         * Create structure of type array
         * @return array-typed structure
         */
        static structure create_array();

        /**
         * Create nested empty object. Only for object-typed structures
         * @param _s parent object key
         * @return true if object created
         */
        bool create_object(const bp::string_view &_s);
        bool create_object(const bp::hash_type &_s);

        /**
         * Create nested empty array. Only for object-typed structures
         * @param _s parent object key
         * @return true if object created
         */
        bool create_array(const bp::string_view &_s);
        bool create_array(const bp::hash_type&_s);

        /**
         * Get size of array or object keys count. Only for array- or object-typed structures
         * @return array size, number of object keys or 0 if not an array or object
         */
        size_t size() const;

        /**
         * Check if array or object is empty
         * @return true if empty
         */
        bool empty() const;


    public:

        /**
         * Serialize structure with specified serializer
         * @tparam serializer_type hash of serializer type name
         * @return serialized string or empty string if specialization for passed serializer type is not found
         */
        template<bp::hash_type::type serializer_type>
        std::string serialize() const { return ""; };

        /**
         * Parse string into structure with specified serializer
         * @tparam serializer_type hash of serializer type name
         * @return true if successfully parsed.
         */
        template<bp::hash_type::type serializer_type>
        bool parse(const bp::string_view &_str) { return false; };

        /**
         * Parse string into new structure with specified serializer
         * @tparam serializer_type hash of serializer type name
         * @return created structure if successfully parsed.
         * Throws parse_error in case of failure or return null-typed structure in exceptionless mode
         */
        template<bp::hash_type::type serializer_type>
        static structure create_from_string(const bp::string_view &_s) {
            structure s;
            s.parse<serializer_type>(_s);
            return s;
        };

        // ITERATORS

        /**
         * Iterator over object keys
         */
        class object_key_iterator : public std::iterator<std::input_iterator_tag, const char *> {
            const structure *object_;
            serializable::object::iterator it_;
            serializable::object::iterator end_;
        public:
            object_key_iterator(const structure *_object);

            object_key_iterator(const object_key_iterator &_it);

            object_key_iterator &operator++();

            object_key_iterator operator++(int);

            object_key_iterator &operator=(const object_key_iterator &_rhs);

            bool operator==(const object_key_iterator &_rhs) const;

            bool operator!=(const object_key_iterator &_rhs) const;

            bp::hash_type operator*();
        };

        // TODO: methods to conform forward iterator interface
        /**
         * Iterator over object (key, value) pairs
         */
        class object_iterator : public std::iterator<std::forward_iterator_tag, structure> {
            const structure *object_;
            serializable::object::iterator it_;
            serializable::object::iterator end_;
        public:
            object_iterator(const structure *_object);

            object_iterator(const object_iterator &_it);

            object_iterator &operator++();

            object_iterator operator++(int);

            object_iterator &operator=(const object_iterator &_rhs);

            bool operator==(const object_iterator &_rhs) const;

            bool operator!=(const object_iterator &_rhs) const;

            std::pair<bp::serializable::object::key_type, bp::structure> operator*();

            /////
//            reference operator*() const;
//            object_iterator operator++(int);
        };

        // TODO: methods to conform random access iterator interface
        /**
         * Iterator over array elements
         */
        class array_iterator : public std::iterator<std::random_access_iterator_tag, structure> {
            const structure *object_;
            serializable::array::iterator it_;
            serializable::array::iterator end_;
//            size_t  index;
//            size_t  size;
        public:
            array_iterator(const structure *_object);

            array_iterator(const array_iterator &_it);

            array_iterator &operator++();

            array_iterator operator++(int);

            array_iterator &operator=(const array_iterator &_rhs);

            bool operator==(const array_iterator &_rhs) const;

            bool operator!=(const array_iterator &_rhs) const;

            bp::structure operator*();

            /////
//            array_iterator& operator--(); //prefix increment
//            array_iterator operator--(int); //postfix decrement
//            friend bool operator<(const array_iterator&, const array_iterator&);
//            friend bool operator>(const array_iterator&, const array_iterator&);
//            friend bool operator<=(const array_iterator&, const array_iterator&);
//            friend bool operator>=(const array_iterator&, const array_iterator&);
//
//            array_iterator& operator+=(size_t);
//            friend array_iterator operator+(const array_iterator&, size_t);
//            friend array_iterator operator+(size_t, const array_iterator&);
//            array_iterator& operator-=(size_t);
//            friend array_iterator operator-(const array_iterator&, size_t);
//            friend difference_type operator-(array_iterator, array_iterator);
//
//            reference operator[](size_t) const;
        };

    private:
        void initialize_if_null(bp::structure::value_type _type);

        template<typename iterator_type>
        class iterable {
            iterator_type it_;
        public:
            iterable(const iterator_type &_it) : it_(_it) {}

            iterable(const iterable &_it) : it_(_it.it_) {}

            iterator_type begin() const { return it_; }

            iterator_type end() const { return iterator_type(nullptr); }

            iterable &operator=(const iterable &_it) {
                it_ = _it.it_;
                return *this;
            }
        };

        template<typename iterator_type>
        iterable<iterator_type> get_iterable() {
            return iterable<iterator_type>(iterator_type(this));
        }

    public:

        /**
         * Get object keys iterator (input iterator)
         * @return
         */
        iterable<object_key_iterator> keys() { return this->get_iterable<object_key_iterator>(); }

        /**
         * Get object keys const iterator (input iterator)
         * @return
         */
        const iterable<object_key_iterator> keys() const { return const_cast<structure *>(this)->keys(); }

        /**
         * Get object items iterator (forward iterator)
         * @return
         */
        iterable<object_iterator> as_object() { return this->get_iterable<object_iterator>(); }

        /**
         * Get object items const iterator (forward iterator)
         * @return
         */
        const iterable<object_iterator> as_object() const { return const_cast<structure *>(this)->as_object(); }

        /**
         * Get array items iterator (random access iterator)
         * @return
         */
        iterable<array_iterator> as_array() { return this->get_iterable<array_iterator>(); }

        /**
         * Get array items const iterator (random access iterator)
         * @return
         */
        const iterable<array_iterator> as_array() const { return const_cast<structure *>(this)->as_array(); }

    private:
        value_type value_type_ = value_type::Null;
        serializable::tree_ptr val_;
    };
}
namespace bp {

    template<>
    std::string structure::as<std::string>() const;

    template<>
    int32_t structure::as<int32_t>() const;
    template<>
    uint32_t structure::as<uint32_t>() const;
    template<>
    int16_t structure::as<int16_t>() const;
    template<>
    uint16_t structure::as<uint16_t>() const;
    template<>
    int8_t structure::as<int8_t>() const;
    template<>
    uint8_t structure::as<uint8_t>() const;

    //TODO: check portability
#if defined(SPARK) || defined(_INT32_T_DECLARED)
    template<>
    int structure::as<int>() const;
#endif

    template<>
    float structure::as<float>() const;

    template<>
    bool structure::as<bool>() const;


    struct value_type_visitor {
        using result_type = bp::structure::value_type;

        bp::structure::value_type operator()(nullptr_t) const { return bp::structure::value_type::Null; }

        template <typename T, typename = typename std::enable_if<std::is_convertible<T, int>::value>::type>
        bp::structure::value_type operator()(T) const { return bp::structure::value_type::Int; }

        bp::structure::value_type
        operator()(float) const { return bp::structure::value_type::Float; }

        bp::structure::value_type operator()(bool) const { return bp::structure::value_type::Bool; }

        bp::structure::value_type
        operator()(const bp::string_view &) const { return bp::structure::value_type::String; }

        bp::structure::value_type
        operator()(const char*) const { return bp::structure::value_type::String; }
    };
}

#endif //BP_SERIALIZER_HPP

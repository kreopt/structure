#ifndef BP_SERIALIZER_HPP
#define BP_SERIALIZER_HPP

#include <string>
#include <iterator>
#include <unordered_map>
#include <vector>
#include <boost/variant.hpp>
#include <binelpro/symbol.hpp>
#include <binelpro/util.hpp>

namespace bp {
    using namespace bp::literals;

    namespace serializable {
        using int_t = int32_t;
        using float_t = double;
        using bool_t = bool;
        using symbol = bp::symbol;
        using string_t = std::string;

        using value = boost::variant<
                serializable::int_t,
                serializable::float_t,
                serializable::bool_t,
                serializable::symbol,
                serializable::string_t>;
        using value_ptr = std::shared_ptr<value>;

        class variant;

        using variant_ptr = std::shared_ptr<variant>;

        using object = std::unordered_map<bp::symbol, variant_ptr>;
        using object_ptr = std::shared_ptr<object>;

        using array = std::vector<variant_ptr>;
        using array_ptr = std::shared_ptr<array>;

        class variant : public boost::variant<value_ptr, object_ptr, array_ptr> {
        public:
            variant() : boost::variant<value_ptr, object_ptr, array_ptr>(std::make_shared<value>(0)) { }

            variant(const serializable::int_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(
                    std::make_shared<value>(_val)) { }

            variant(const serializable::float_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(
                    std::make_shared<value>(_val)) { }

            variant(const serializable::bool_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(
                    std::make_shared<value>(_val)) { }

            variant(const serializable::symbol &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(
                    std::make_shared<value>(_val)) { }

            variant(const serializable::string_t &_val) : boost::variant<value_ptr, object_ptr, array_ptr>(
                    std::make_shared<value>(_val)) { }

            variant(const char *_val) : boost::variant<value_ptr, object_ptr, array_ptr>(
                    _val ? std::make_shared<value>(std::string(_val)) : nullptr) { }

            template<typename ValueType,
                    class = typename std::enable_if<
                            bp::is_any_of<
                                    std::remove_cv_t<std::remove_reference_t<ValueType>>,
                                    value, object, array>::value
                    >::type>
            variant(const std::shared_ptr<ValueType> &_val) :
                    boost::variant<value_ptr, object_ptr, array_ptr>(_val) { }

            template<typename ValueType,
                    class = typename std::enable_if<
                            bp::is_any_of<
                                    std::remove_cv_t<std::remove_reference_t<ValueType>>,
                                    value, object, array>::value
                    >::type>
            variant(std::shared_ptr<ValueType> &&_val) :
                    boost::variant<value_ptr, object_ptr, array_ptr>(std::forward<std::shared_ptr<ValueType>>(_val)) { }

            template<typename ValueType,
                    class = typename std::enable_if<
                            bp::is_any_of<
                                    std::remove_cv_t<std::remove_reference_t<ValueType>>,
                                    value, object, array>::value
                    >::type>
            variant(ValueType &&_val) :
                    boost::variant<value_ptr, object_ptr, array_ptr>(
                            std::make_shared<std::remove_reference_t<ValueType>>(std::forward<ValueType>(_val))
                    ) { }
        };

        template <typename V, class = typename std::enable_if<bp::is_any_of<V, array_ptr, object_ptr, value_ptr>::value>::type>
        V get_variant(const variant_ptr &_val) {
            return boost::get<V>(*_val);
        }

        template <typename V>
        V get_value(const variant_ptr &_val) {
            return boost::get<V>(*get_variant<value_ptr>(_val));
        }
    }

    class structure {
    public:
        using ptr = std::shared_ptr<structure>;
        using wptr = std::weak_ptr<structure>;

        enum class value_type: char {
            Int = 'i',
            Float = 'f',
            String = 's',
            Bool = 'b',
            Null = 'n',
            Object = 'o',
            Array = 'a',
            Symbol = 'S'
        };

        structure(serializable::variant_ptr _obj = nullptr);
        structure(const structure& _s);
        structure(structure &&_s);

        inline operator bool() const {
            return static_cast<bool>(val_);
        }

        serializable::variant_ptr data() const {return val_;}

        structure deepcopy() const;

        serializable::string_t  as_string() const;
        serializable::int_t     as_int() const;
        serializable::float_t   as_float() const;
        serializable::bool_t    as_bool() const;
        serializable::symbol    as_symbol() const;

        inline bool is_int() const { return value_type_ == value_type::Int; }
        inline bool is_float() const {return value_type_ == value_type::Float; }
        inline bool is_bool() const { return value_type_ == value_type::Bool; }
        inline bool is_string() const { return value_type_ == value_type::String; }
        inline bool is_null() const { return value_type_ == value_type::Null; }
        inline bool is_array() const { return value_type_ == value_type::Array; }
        inline bool is_object() const { return value_type_ == value_type::Object; }
        inline bool is_symbol() const { return value_type_ == value_type::Symbol; }

        inline value_type type() const { return value_type_; };
        inline void set_type(value_type _type) {value_type_ = _type;};
        value_type get_variant_type(const serializable::variant &_var) const;

        // arrays:
        size_t size() const;

        structure  operator[] (int index);

        structure at(int index) const;

        template<typename ValType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<ValType>, serializable::variant>::value>::type>
        void append(ValType &&_val) {
            initialize_if_null(value_type::Array);
            if (type() == value_type::Array) {
                return serializable::get_variant<serializable::array_ptr>(val_)->push_back(std::make_shared<serializable::variant>(std::forward<ValType>(_val)));
            } else {
                throw std::range_error("not an array");
            }
        }
        bool append(const std::initializer_list<serializable::variant> &_val);
        bool append(const std::initializer_list<std::pair<std::string, serializable::variant>> &_val);

        // objects:
        structure operator [](const symbol &_key);
        structure operator [](const char* _key) {return operator[](bp::symbol(_key));}

        structure at(const symbol &_key) const;

        void erase(const symbol &_key);

    private:
        inline void emplace_init(){
            initialize_if_null(value_type::Object);
            if (type() != value_type::Object) {
                throw std::range_error("not an object");
            }
        }
        template <typename KeyType>
        inline bool emplace_resolve(KeyType &&_key, const serializable::variant& _var) {
            return serializable::get_variant<serializable::object_ptr>(val_)->
                    emplace(std::forward<KeyType>(_key), std::make_shared<serializable::variant>(_var)).second;
        }
    public:

        template <typename KeyType, typename ValType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<KeyType>, bp::symbol>::value>::type,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<ValType>, serializable::variant>::value>::type>
        bool emplace(KeyType &&_key, ValType &&_val) {
            emplace_init();
            return emplace_resolve(std::forward<KeyType>(_key), std::forward<ValType>(_val));
        };

        template <typename KeyType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<KeyType>, bp::symbol>::value>::type>
        bool emplace(KeyType &&_key, const std::initializer_list<serializable::variant> &_val) {
            emplace_init();
            serializable::array_ptr obj = std::make_shared<serializable::array>();
            for (auto item: _val) {
                obj->push_back(std::make_shared<serializable::variant>(item));
            }
            return emplace_resolve(std::forward<KeyType>(_key), obj);
        };

        template <typename KeyType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<KeyType>, bp::symbol>::value>::type>
        bool emplace(KeyType &&_key, const std::initializer_list<std::pair<bp::symbol, serializable::variant>> &_val) {
            emplace_init();
            serializable::object_ptr obj = std::make_shared<serializable::object>();
            for (auto item: _val) {
                obj->emplace(item.first, std::make_shared<serializable::variant>(item.second));
            }
            return emplace_resolve(std::forward<KeyType>(_key), obj);
        };

        template <typename KeyType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<KeyType>, bp::symbol>::value>::type>
        bool emplace(KeyType &&_key, const std::initializer_list<std::pair<bp::symbol, bp::structure>> &_val) {
            emplace_init();
            serializable::object_ptr obj = std::make_shared<serializable::object>();
            for (auto item: _val) {
                obj->emplace(item.first, item.second.data());
            }
            return emplace_resolve(std::forward<KeyType>(_key), obj);
        };

        bool emplace(const std::initializer_list<std::pair<bp::symbol, serializable::variant>> &_val);

        bool emplace(const std::initializer_list<std::pair<bp::symbol, structure>> &_val);

        bool emplace(symbol &&_key, const bp::structure & _str);


        template<typename ValueType,
                class = typename std::enable_if<std::is_convertible<std::remove_cv_t<std::remove_reference_t<ValueType>>, serializable::variant>::value>::type>
        structure get(const bp::symbol &_key, ValueType && _default) const {
            if (type() == value_type::Object) {
                try {
                    auto val = serializable::get_variant<serializable::object_ptr>(val_)->at(_key);
                    return bp::structure(val);
                } catch (std::out_of_range &e) {
                    return bp::structure(std::make_shared<serializable::variant>(std::forward<ValueType>(_default)));
                }
            } else {
                throw std::range_error("not an object");
            }
        };
        inline structure get(const bp::symbol &_key) const {
            return get(_key, serializable::variant(nullptr));
        };

        structure& operator=(const structure &_str);
        structure& operator=(structure &&_str);
        structure& operator=(const std::initializer_list<serializable::variant> &_val) ;   // array
        structure& operator=(const std::initializer_list<std::pair<std::string, serializable::variant>> &_val) ;   // object
        structure& operator=(const std::initializer_list<std::pair<std::string, structure>> &_val) ;   // object

//        structure& operator=(serializable::variant &&_var);
//        structure& operator=(const serializable::variant &_var);

        template<typename ValType,
                class = typename std::enable_if<std::is_convertible<std::remove_reference_t<ValType>, serializable::variant>::value>::type>
        structure& operator=(ValType &&_val) {
            initialize_if_null(value_type::Object);
            if (val_) {
                *val_ = _val;
            } else {
                val_ = std::make_shared<serializable::variant>(std::forward<ValType>(_val));
            }
            set_type(get_variant_type(_val));
            return *this;
        }

        template <symbol::hash_type serializer_type>
        std::string stringify() const {return "";};

        template <symbol::hash_type serializer_type>
        void parse(const std::string &_str) {};

        template <symbol::hash_type serializer_type>
        static structure create_from_string(const std::string &_s){
            structure s;
            s.parse<serializer_type>(_s);
            return s;
        };

        // ITERATORS

        class object_key_iterator : public std::iterator<std::input_iterator_tag, const char*> {
            const structure*  object_;
            serializable::object::iterator    it;
            serializable::object::iterator    end;
        public:
            object_key_iterator(const structure *_object);
            object_key_iterator(const object_key_iterator&_it);
            object_key_iterator & operator++();
            object_key_iterator operator++(int);
            bool operator==(const object_key_iterator& rhs) const;
            bool operator!=(const object_key_iterator& rhs) const;
            const char * operator*();
        };

        // TODO: methods to conform forward iterator interface
        class object_iterator : public std::iterator<std::forward_iterator_tag, structure> {
            const structure* object_;
            serializable::object::iterator    it;
            serializable::object::iterator    end;
        public:
            object_iterator(const structure *_object);
            object_iterator(const object_iterator&_it);
            object_iterator & operator++();
            object_iterator operator++(int);
            bool operator==(const object_iterator& rhs) const;
            bool operator!=(const object_iterator& rhs) const;
            std::pair<bp::serializable::object::key_type, bp::structure>  operator*();

            /////
//            reference operator*() const;
//            object_iterator operator++(int);
        };

        // TODO: methods to conform random access iterator interface
        class array_iterator : public std::iterator<std::random_access_iterator_tag, structure> {
            const structure* object_;
            serializable::array::iterator    it;
            serializable::array::iterator    end;
//            size_t  index;
//            size_t  size;
        public:
            array_iterator(const structure *_object);
            array_iterator(const array_iterator&_it);
            array_iterator & operator++();
            array_iterator operator++(int);
            bool operator==(const array_iterator& rhs) const;
            bool operator!=(const array_iterator& rhs) const;
            bp::structure  operator*();

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

        template <typename V, class = typename std::enable_if<bp::is_any_of<V, serializable::array, serializable::object>::value>::type>
        void initialize_val() {
            if (val_) {
                *val_ = std::make_shared<V>();
            } else {
                val_ = std::make_shared<serializable::variant>(std::make_shared<V>());
            }
        }

        template <typename iterator_type>
        class iterable {
            iterator_type it_;
        public:
            iterable(const iterator_type &_it) : it_(_it){}
            iterable(const iterable &_it) : it_(_it.it_) {}
            iterator_type begin() const {return it_;}
            iterator_type end() const { return iterator_type(nullptr);}
        };
        template <typename iterator_type>
        iterable<iterator_type> get_iterable() {
            return iterable<iterator_type>(iterator_type(this));
        }

    public:

        iterable<object_key_iterator> keys() {return this->get_iterable<object_key_iterator>();}
        iterable<object_iterator> as_object() {return this->get_iterable<object_iterator>();}
        iterable<array_iterator> as_array() {return this->get_iterable<array_iterator>();}

        const iterable<object_key_iterator> keys() const {return static_cast<const structure*>(this)->keys();}
        const iterable<object_iterator> as_object() const {return static_cast<const structure*>(this)->as_object();}
        const iterable<array_iterator> as_array() const {return static_cast<const structure*>(this)->as_array();}
        // EXCEPTIONS

        class structure_error : public std::runtime_error {
        public:
            structure_error(const char* _msg) : std::runtime_error(_msg){}
        };
        class parse_error : public structure_error {
        public:
            parse_error(const char* _msg) : structure_error(_msg){}
        };

    private:
        value_type  value_type_ = value_type::Null;
        serializable::variant_ptr val_;
    };
}


#endif //BP_SERIALIZER_HPP

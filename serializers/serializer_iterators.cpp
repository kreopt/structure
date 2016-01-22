#include "serializer.hpp"

// OBJECT KEYS

bp::serializer::object_key_iterator::object_key_iterator(const serializer::ptr &_object) : object_(_object) {
    if (object_ && object_->is_object()) {
        it = object_->get_variant<object_ptr>(object_->val_)->begin();
    }
}

bp::serializer::object_key_iterator::object_key_iterator(const bp::serializer::object_key_iterator&_it)
        : object_(_it.object_), it(_it.it), end(_it.end) {}

bp::serializer::object_key_iterator &bp::serializer::object_key_iterator::operator++() {
    ++it;
    return *this;
}

bp::serializer::object_key_iterator bp::serializer::object_key_iterator::operator++(int) {object_key_iterator tmp(*this); operator++(); return tmp;}

bool bp::serializer::object_key_iterator::operator==(const object_key_iterator& rhs) {
    if (!rhs.object_) {
        return it==end;
    }
    return it==rhs.it;
}

const char *bp::serializer::object_key_iterator::operator*() {
    if (!object_ || it==end) {
        return nullptr;
    }
    return it->first;
}

bool bp::serializer::object_key_iterator::operator!=(const object_key_iterator& rhs) {
    if (!rhs.object_) {
        return it!=end;
    }
    return it!=rhs.it;
}

// OBJECT ITEMS

bp::serializer::object_iterator::object_iterator(const serializer::ptr &_object) : object_(_object) {
    if (object_ && object_->is_object()) {
        it = object_->get_variant<object_ptr>(object_->val_)->begin();
    }
}

bp::serializer::object_iterator::object_iterator(const object_iterator&_it)
        : object_(_it.object_), it(_it.it), end(_it.end) {}

bp::serializer::object_iterator &bp::serializer::object_iterator::operator++() {
    ++it;
    return *this;
}

bp::serializer::object_iterator bp::serializer::object_iterator::operator++(int) {object_iterator tmp(*this); operator++(); return tmp;}

bool bp::serializer::object_iterator::operator==(const bp::serializer::object_iterator& rhs) {
    if (!rhs.object_) {
        return it==end;
    }
    return it==rhs.it;
}

std::pair<bp::serializer::object_t::key_type, bp::serializer::ptr> bp::serializer::object_iterator::operator*() {
    if (!object_ || it==end) {
        return std::make_pair(""_sym, bp::serializer::ptr());
    }
    return std::make_pair(it->first, bp::serializer::create(it->second));
}

bool bp::serializer::object_iterator::operator!=(const bp::serializer::object_iterator& rhs) {
    if (!rhs.object_) {
        return it!=end;
    }
    return it!=rhs.it;
}

// ARRAY ITEMS

bp::serializer::array_iterator::array_iterator(const serializer::ptr &_object) : object_(_object) {
    index = 0;
    size = 0;
    if (object_ && object_->is_array()) {
        size = object_->size();
    }
}

bp::serializer::array_iterator::array_iterator(const bp::serializer::array_iterator &_it)
        : object_(_it.object_), index(_it.index), size(_it.size) {}

bp::serializer::array_iterator &bp::serializer::array_iterator::operator++() {
    if (index<size) {
        ++index;
    }
    return *this;
}

bp::serializer::array_iterator bp::serializer::array_iterator::operator++(int) {array_iterator tmp(*this); operator++(); return tmp;}

bool bp::serializer::array_iterator::operator==(const bp::serializer::array_iterator& rhs) {
    if (!rhs.object_) {
        return index>=size || index == -1;
    }
    return index==rhs.index;
}

bp::serializer::ptr bp::serializer::array_iterator::operator*() {
    if (!object_ || index>=size || index == -1) {
        return nullptr;
    }
    return object_->at(index);
}

bool bp::serializer::array_iterator::operator!=(const bp::serializer::array_iterator& rhs) {
    return !operator==(rhs);
}
#include <string.h>
#include "Object.hpp"

namespace zebra {

  Object::Object(const char* s):
    m_type(Type::String),
    m_number(0.0) {
        if(s) strncpy_s(m_string, Object::MAX_STRING_SIZE, s, strlen(s));
    }

  Object::Object(double number):
    m_type(Type::Number),
    m_number(number) {}

  Object::Object(bool value):
    m_type(value ? Type::True : Type::False),
    m_number(0.0) {}

  Object::Object():
    m_type(Type::Nil),
    m_number(0.0) {}

  bool Object::is_bool() const {
    return m_type == Type::True || m_type == Type::False;
  }

  //False if boolean false or nil, True otherwise.
  //Truthiness based on Ruby way of doing it
  bool Object::is_true() const {
    if (m_type == Type::Nil || m_type == Type::False) return false;
    return true;
  }

  bool Object::is_nil() const {
    return m_type == Type::Nil;
  }

  bool Object::is_number() const {
    return m_type == Type::Number;
  }

  bool Object::is_string() const {
    return m_type == Type::String;
  }

  double Object::get_number() const {
    return m_number;
  }

  const char* Object::get_string() const {
    return m_string;
  }

  bool Object::get_bool() const {
    if (m_type == Type::False) {
      return false;
    }

    return true;
  }

  void Object::set_number(double number) {
    m_number = number;
  }


}

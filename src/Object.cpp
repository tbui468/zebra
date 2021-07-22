#include <string.h>
#include "Object.hpp"

namespace zebra {

/*
      Type m_type; 
      std::string m_string;
      float m_float;
      int m_int;*/

  Object::Object(const std::string& s):
    m_type(Type::String),
    m_string(s),
    m_float(0.0f),
    m_int(0) {}

  Object::Object(int i):
    m_type(Type::Int),
    m_string(""),
    m_float(0.f),
    m_int(i) {}

  Object::Object(float f):
    m_type(Type::Float),
    m_string(""),
    m_float(f),
    m_int(0) {}

  Object::Object(bool value):
    m_type(value ? Type::True : Type::False),
    m_string(""),
    m_float(0.f),
    m_int(0)
    {}

  Object::Object():
    m_type(Type::Nil),
    m_string(""),
    m_float(0.f),
    m_int(0)
    {}

  bool Object::is_bool() const {
    return m_type == Type::True || m_type == Type::False;
  }

  //False if boolean false or nil, True otherwise.
  //Truthiness based on Ruby way of doing it
  bool Object::is_truthy() const {
    if (m_type == Type::Nil || m_type == Type::False) return false;
    return true;
  }

  bool Object::is_nil() const {
    return m_type == Type::Nil;
  }

  bool Object::is_float() const {
    return m_type == Type::Float;
  }
  bool Object::is_int() const {
    return m_type == Type::Int;
  }

  bool Object::is_string() const {
    return m_type == Type::String;
  }

  int Object::get_int() const {
    return m_int;
  }

  float Object::get_float() const {
    return m_float;
  }

  std::string Object::get_string() const {
    return m_string;
  }

  bool Object::get_bool() const {
    if (m_type == Type::False) {
      return false;
    }

    return true;
  }

  /*
  void Object::set_number(double number) {
    m_number = number;
  }*/


}

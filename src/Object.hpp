#ifndef ZEBRA_OBJECT_H
#define ZEBRA_OBJECT_H

#include <string>

namespace zebra {

  class Object {
    public:
      Object(const std::string& s);
      Object(float f);
      Object(int i);
      Object(bool value);
      Object(); //of type nil
      virtual ~Object() {}
      
      bool is_bool() const;
      bool is_truthy() const;
      bool is_nil() const;
      bool is_int() const;
      bool is_float() const;
      bool is_string() const;
      float get_float() const;
      std::string get_string() const;
      int get_int() const;
      bool get_bool() const;
//      void set_number(double n);
    private:
      enum class Type{
        True,
        False,
        Nil,
        Int,
        Float,
        String
      };
      Type m_type; 
      std::string m_string;
      float m_float;
      int m_int;
  };
}


#endif //ZEBRA_OBJECT_H

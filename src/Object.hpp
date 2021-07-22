#ifndef ZEBRA_OBJECT_H
#define ZEBRA_OBJECT_H


namespace zebra {

  class Object {
    public:
      Object(const char* s);
      Object(double number);
      Object(bool value);
      Object(); //of type nil
      virtual ~Object() {}
      
      bool is_bool() const;
      bool is_true() const;
      bool is_nil() const;
      bool is_number() const;
      bool is_string() const;
      double get_number() const;
      bool get_bool() const;
      void set_number(double n);
      const char* get_string() const;
    private:
      static const int MAX_STRING_SIZE = 256;
      enum class Type{
        True,
        False,
        Nil,
        Number,
        String
      };
      Type m_type; 
      char m_string[MAX_STRING_SIZE] = {0};
      double m_number;
  };
}


#endif //ZEBRA_OBJECT_H

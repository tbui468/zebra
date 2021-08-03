#ifndef ZEBRA_EXPR_H
#define ZEBRA_EXPR_H

#include <string>
#include <memory>
#include <unordered_map>
#include "Token.hpp"
#include "DataType.hpp"

namespace zebra {

    class Object;


    /*
     * Forward declare expressions for interfaces
     * This is also a summary of the order
     */
    struct Import;

    struct Unary;
    struct Binary;
    struct Group;
    struct Literal;
    struct Logic;

    struct DeclVar;
    struct GetVar;
    struct SetVar;
    struct DeclFun;
    struct CallFun;
    struct Return;

    struct Block;
    struct If;
    struct For;
    struct While;

    struct DeclClass;
    struct InstClass;
    struct GetField;
    struct SetField;
    struct CallMethod;

    struct ExprStringVisitor {
        virtual std::string visit(Import* expr) = 0;

        virtual std::string visit(Unary* expr) = 0;
        virtual std::string visit(Binary* expr) = 0;
        virtual std::string visit(Group* expr) = 0;
        virtual std::string visit(Literal* expr) = 0;
        virtual std::string visit(Logic* expr) = 0;

        virtual std::string visit(DeclVar* expr) = 0;
        virtual std::string visit(GetVar* expr) = 0;
        virtual std::string visit(SetVar* expr) = 0;
        virtual std::string visit(DeclFun* expr) = 0;
        virtual std::string visit(CallFun* expr) = 0;
        virtual std::string visit(Return* expr) = 0;

        virtual std::string visit(Block* expr) = 0;
        virtual std::string visit(If* expr) = 0;
        virtual std::string visit(For* expr) = 0;
        virtual std::string visit(While* expr) = 0;

        virtual std::string visit(DeclClass* expr) = 0;
        virtual std::string visit(InstClass* expr) = 0;
        virtual std::string visit(GetField* expr) = 0;
        virtual std::string visit(SetField* expr) = 0;
        virtual std::string visit(CallMethod* expr) = 0;
    };

    struct ExprObjectVisitor {
        virtual std::shared_ptr<Object> visit(Import* expr) = 0;

        virtual std::shared_ptr<Object> visit(Unary* expr) = 0;
        virtual std::shared_ptr<Object> visit(Binary* expr) = 0;
        virtual std::shared_ptr<Object> visit(Group* expr) = 0;
        virtual std::shared_ptr<Object> visit(Literal* expr) = 0;
        virtual std::shared_ptr<Object> visit(Logic* expr) = 0;

        virtual std::shared_ptr<Object> visit(DeclVar* expr) = 0;
        virtual std::shared_ptr<Object> visit(GetVar* expr) = 0;
        virtual std::shared_ptr<Object> visit(SetVar* expr) = 0;
        virtual std::shared_ptr<Object> visit(DeclFun* expr) = 0;
        virtual std::shared_ptr<Object> visit(CallFun* expr) = 0;
        virtual std::shared_ptr<Object> visit(Return* expr) = 0;

        virtual std::shared_ptr<Object> visit(Block* expr) = 0;
        virtual std::shared_ptr<Object> visit(If* expr) = 0;
        virtual std::shared_ptr<Object> visit(For* expr) = 0;
        virtual std::shared_ptr<Object> visit(While* expr) = 0;

        virtual std::shared_ptr<Object> visit(DeclClass* expr) = 0;
        virtual std::shared_ptr<Object> visit(InstClass* expr) = 0;
        virtual std::shared_ptr<Object> visit(GetField* expr) = 0;
        virtual std::shared_ptr<Object> visit(SetField* expr) = 0;
        virtual std::shared_ptr<Object> visit(CallMethod* expr) = 0;
    };

    struct DataTypeVisitor {
        virtual DataType visit(Import* expr) = 0;

        virtual DataType visit(Unary* expr) = 0;
        virtual DataType visit(Binary* expr) = 0;
        virtual DataType visit(Group* expr) = 0;
        virtual DataType visit(Literal* expr) = 0;
        virtual DataType visit(Logic* expr) = 0;

        virtual DataType visit(DeclVar* expr) = 0;
        virtual DataType visit(GetVar* expr) = 0;
        virtual DataType visit(SetVar* expr) = 0;
        virtual DataType visit(DeclFun* expr) = 0;
        virtual DataType visit(CallFun* expr) = 0;
        virtual DataType visit(Return* expr) = 0;

        virtual DataType visit(Block* expr) = 0;
        virtual DataType visit(If* expr) = 0;
        virtual DataType visit(For* expr) = 0;
        virtual DataType visit(While* expr) = 0;

        virtual DataType visit(DeclClass* expr) = 0;
        virtual DataType visit(InstClass* expr) = 0;
        virtual DataType visit(GetField* expr) = 0;
        virtual DataType visit(SetField* expr) = 0;
        virtual DataType visit(CallMethod* expr) = 0;
    };

    /*
     * Base class
     */
    struct Expr {
        public:
            virtual ~Expr() {}
            virtual std::string accept(ExprStringVisitor& visitor) = 0;
            virtual std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) = 0;
            virtual DataType accept(DataTypeVisitor& visitor) = 0;
    };


    /*
     * Misc.
     */
    struct Import: public Expr {
        public:
            Import(Token name, std::unordered_map<std::string, std::shared_ptr<Object>> functions):
                m_name(name), m_functions(functions) {}
            ~Import() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::unordered_map<std::string, std::shared_ptr<Object>> m_functions;
    };

    /*
     * Basic
     */
    struct Unary: public Expr {
        public:
            Unary(Token op, std::shared_ptr<Expr> right): m_op(op), m_right(right) {}
            ~Unary() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_op;
            std::shared_ptr<Expr> m_right;
    };

    struct Binary: public Expr {
        public:
            Binary(Token op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right): m_op(op), m_left(left), m_right(right) {}
            ~Binary() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_op;
            std::shared_ptr<Expr> m_left;
            std::shared_ptr<Expr> m_right;
    };


    struct Group: public Expr {
        public:
            Group(Token name, std::shared_ptr<Expr> expr): m_name(name), m_expr(expr) {}
            ~Group() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_expr;
    };

    struct Literal: public Expr {
        public:
            Literal(Token token): m_token(token) {}
            ~Literal() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_token;
    };


    struct Logic: public Expr {
        public:
            Logic(Token op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right): m_op(op), m_left(left), m_right(right) {}
            ~Logic() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_op;
            std::shared_ptr<Expr> m_left;
            std::shared_ptr<Expr> m_right;
    };

    /*
     * Variables and Functions
     */
    struct DeclVar: public Expr {
        public:
            DeclVar(Token name, Token type, std::shared_ptr<Expr> value): 
                m_name(name), m_type(type), m_value(value) {}
            ~DeclVar() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            Token m_type;
            std::shared_ptr<Expr> m_value;
    };

    struct GetVar: public Expr {
        public:
            GetVar(Token name): m_name(name) {}
            ~GetVar() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
    };

    struct SetVar: public Expr {
        public:
            SetVar(Token name, std::shared_ptr<Expr> value): m_name(name), m_value(value) {}
            ~SetVar() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_value;
    };

    struct DeclFun: public Expr {
        public:
            DeclFun(Token name, std::vector<std::shared_ptr<Expr>> parameters, TokenType type, std::shared_ptr<Expr> body): 
                m_name(name), m_parameters(parameters), m_return_type(type), m_body(body), m_arity(parameters.size()) {}
            ~DeclFun() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::vector<std::shared_ptr<Expr>> m_parameters;
            TokenType m_return_type;
            std::shared_ptr<Expr> m_body;
            int m_arity;
    };

    struct CallFun: public Expr {
        public:
            CallFun(Token name, std::vector<std::shared_ptr<Expr>> arguments): 
                m_name(name), m_arguments(arguments), m_arity(arguments.size()) {}
            ~CallFun() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::vector<std::shared_ptr<Expr>> m_arguments;
            int m_arity;
            std::shared_ptr<Object> m_return {nullptr};
    };

    struct Return: public Expr {
        public:
            Return(Token name, std::shared_ptr<Expr> value): 
                m_name(name), m_value(value) {}
            ~Return() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_value;
    };


    /*
     * Control Flow
     */
    struct Block: public Expr {
        public:
            Block(Token name, std::vector<std::shared_ptr<Expr>> expressions): m_name(name), m_expressions(expressions) {}
            ~Block() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::vector<std::shared_ptr<Expr>> m_expressions;
    };

    struct If: public Expr {
        public:
            If(Token name, std::shared_ptr<Expr> condition, std::shared_ptr<Expr> then_branch, std::shared_ptr<Expr> else_branch): 
                m_name(name), m_condition(condition), m_then_branch(then_branch), m_else_branch(else_branch) {}
            ~If() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Expr> m_then_branch;
            std::shared_ptr<Expr> m_else_branch;
    };

    struct For: public Expr {
        public:
            For(Token name, std::shared_ptr<Expr> initializer, std::shared_ptr<Expr> condition, std::shared_ptr<Expr> update, std::shared_ptr<Expr> body): 
                m_name(name), m_initializer(initializer), m_condition(condition), m_update(update), m_body(body) {}
            ~For() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_initializer;
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Expr> m_update;
            std::shared_ptr<Expr> m_body;
    };


    struct While: public Expr {
        public:
            While(Token name, std::shared_ptr<Expr> condition, std::shared_ptr<Expr> body): 
                m_name(name), m_condition(condition), m_body(body) {}
            ~While() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Expr> m_body;
    };

    /*
     * Classes
     */
    struct DeclClass: public Expr {
        public:
            DeclClass(Token name, Token base, std::vector<std::shared_ptr<Expr>> fields, std::vector<std::shared_ptr<Expr>> methods):
                m_name(name), m_base(base), m_fields(fields), m_methods(methods) {}
            ~DeclClass() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            Token m_base;
            std::vector<std::shared_ptr<Expr>> m_fields;
            std::vector<std::shared_ptr<Expr>> m_methods;
    };

    struct InstClass: public Expr {
        public:
            InstClass(Token name, Token class_decl, std::vector<std::shared_ptr<Expr>> arguments):
                m_name(name), m_class(class_decl), m_arguments(arguments) {}
            ~InstClass() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            Token m_class;
            std::vector<std::shared_ptr<Expr>> m_arguments;
    };

    struct GetField: public Expr {
        public:
            GetField(Token name, Token field): m_name(name), m_field(field) {}
            ~GetField() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            Token m_field;
    };

    struct SetField: public Expr {
        public:
            SetField(Token name, Token field, std::shared_ptr<Expr> value): 
                m_name(name), m_field(field), m_value(value) {}
            ~SetField() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            Token m_field;
            std::shared_ptr<Expr> m_value;
    };

    struct CallMethod: public Expr {
        public:
            CallMethod(Token name, Token method, std::vector<std::shared_ptr<Expr>> arguments): 
                m_name(name), m_method(method), m_arguments(arguments), m_arity(arguments.size()) {}
            ~CallMethod() {}
            std::string accept(ExprStringVisitor& visitor) { return visitor.visit(this); }
            std::shared_ptr<Object> accept(ExprObjectVisitor& visitor) { return visitor.visit(this); }
            DataType accept(DataTypeVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_name;
            Token m_method;
            std::vector<std::shared_ptr<Expr>> m_arguments;
            int m_arity;
            std::shared_ptr<Object> m_return {nullptr};

    };

}


#endif //ZEBRA_EXPR_H

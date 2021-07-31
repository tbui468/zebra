#ifndef ZEBRA_STMT_H
#define ZEBRA_STMT_H

#include <vector>
#include <unordered_map>

namespace zebra {

    //foward declarations
    struct Expr;
    class Object;

    struct If;
    struct While;
    struct For;
    struct Return;
    struct AssignField;
    struct FunDecl;
    struct Call;
    struct StructDecl;
    struct StructInst;
    struct Import;

    struct StmtStringVisitor {
        virtual std::string visit(If* stmt) = 0;
        virtual std::string visit(While* stmt) = 0;
        virtual std::string visit(For* stmt) = 0;
        virtual std::string visit(Return* stmt) = 0;
        virtual std::string visit(AssignField* stmt) = 0;
        virtual std::string visit(FunDecl* stmt) = 0;
        virtual std::string visit(Call* stmt) = 0;
        virtual std::string visit(StructDecl* stmt) = 0;
        virtual std::string visit(StructInst* stmt) = 0;
        virtual std::string visit(Import* stmt) = 0;
    };
    struct StmtVoidVisitor {
        virtual void visit(If* stmt) = 0;
        virtual void visit(While* stmt) = 0;
        virtual void visit(For* stmt) = 0;
        virtual void visit(Return* stmt) = 0;
        virtual void visit(AssignField* stmt) = 0;
        virtual void visit(FunDecl* stmt) = 0;
        virtual void visit(Call* stmt) = 0;
        virtual void visit(StructDecl* stmt) = 0;
        virtual void visit(StructInst* stmt) = 0;
        virtual void visit(Import* stmt) = 0;
    };


    //Statements
    struct Stmt {
        Stmt(Token name): m_name(name) {}
        virtual ~Stmt() {}
        virtual std::string accept(StmtStringVisitor& visitor) = 0;
        virtual void accept(StmtVoidVisitor& visitor) = 0;
        Token m_name;
    };


    struct If: public Stmt {
        public:
            If(Token name, std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> then_branch, std::shared_ptr<Stmt> else_branch): 
                Stmt(name), m_condition(condition), m_then_branch(then_branch), m_else_branch(else_branch) {}
            ~If() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Stmt> m_then_branch;
            std::shared_ptr<Stmt> m_else_branch;
    };


    struct While: public Stmt {
        public:
            While(Token name, std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body): 
                Stmt(name), m_condition(condition), m_body(body) {}
            ~While() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Stmt> m_body;
    };

    struct For: public Stmt {
        public:
            For(Token name, std::shared_ptr<Stmt> initializer, std::shared_ptr<Expr> condition, std::shared_ptr<Expr> update, std::shared_ptr<Stmt> body): 
                Stmt(name), m_initializer(initializer), m_condition(condition), m_update(update), m_body(body) {}
            ~For() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            std::shared_ptr<Stmt> m_initializer; //this is messy in parser since statments expect a semicolon
            std::shared_ptr<Expr> m_condition;
            std::shared_ptr<Expr> m_update;
            std::shared_ptr<Stmt> m_body;
    };


    struct Return: public Stmt {
        public:
            Return(Token name, TokenType return_type, std::shared_ptr<Expr> value): 
                Stmt(name), m_return_type(return_type), m_value(value) {}
            ~Return() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            TokenType m_return_type;
            std::shared_ptr<Expr> m_value;
    };

    struct AssignField: public Stmt {
        public:
            AssignField(Token name, Token instance, Token field, std::shared_ptr<Expr> value): 
                Stmt(name), m_instance(instance), m_field(field), m_value(value) {}
            ~AssignField() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_instance;
            Token m_field;
            std::shared_ptr<Expr> m_value;
    };




    struct FunDecl: public Stmt {
        public:
            FunDecl(Token name, std::vector<std::shared_ptr<Stmt>> parameters, TokenType type, std::shared_ptr<Stmt> body): 
                Stmt(name), m_parameters(parameters), m_return_type(type), m_body(body), m_arity(parameters.size()) {}
            ~FunDecl() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            std::vector<std::shared_ptr<Stmt>> m_parameters;
            TokenType m_return_type;
            std::shared_ptr<Stmt> m_body;
            int m_arity;
    };


    struct Call: public Stmt {
        public:
            Call(Token name, std::vector<std::shared_ptr<Expr>> arguments): 
                Stmt(name), m_arguments(arguments), m_arity(arguments.size()) {}
            ~Call() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            std::vector<std::shared_ptr<Expr>> m_arguments;
            int m_arity;
            std::shared_ptr<Object> m_return {nullptr};
    };

    struct StructDecl: public Stmt {
        public:
            StructDecl(Token name, std::vector<std::shared_ptr<VarDecl>> fields): 
                Stmt(name), m_fields(fields) {}
            ~StructDecl() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            std::vector<std::shared_ptr<VarDecl>> m_fields;
    };

    struct StructInst: public Stmt {
        public:
            StructInst(Token name, Token struct_token, std::vector<std::shared_ptr<Expr>> arguments):
                Stmt(name), m_struct(struct_token), m_arguments(arguments) {}
            ~StructInst() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            Token m_struct;
            std::vector<std::shared_ptr<Expr>> m_arguments;
    };

    struct Import: public Stmt {
        public:
            Import(Token name, std::unordered_map<std::string, std::shared_ptr<Object>> functions):
                Stmt(name), m_functions(functions) {}
            ~Import() {}
            std::string accept(StmtStringVisitor& visitor) { return visitor.visit(this); }
            void accept(StmtVoidVisitor& visitor) { return visitor.visit(this); }
        public:
            std::unordered_map<std::string, std::shared_ptr<Object>> m_functions;
    };


}


#endif //ZEBRA_STMT_H

#ifndef ZEBRA_STMT_H
#define ZEBRA_STMT_H

#include <vector>
#include <unordered_map>

namespace zebra {

    //foward declarations
    struct Expr;
    class Object;

    struct AssignField;
    struct StructDecl;
    struct StructInst;

    struct StmtStringVisitor {
        virtual std::string visit(AssignField* stmt) = 0;
        virtual std::string visit(StructDecl* stmt) = 0;
        virtual std::string visit(StructInst* stmt) = 0;
    };
    struct StmtVoidVisitor {
        virtual void visit(AssignField* stmt) = 0;
        virtual void visit(StructDecl* stmt) = 0;
        virtual void visit(StructInst* stmt) = 0;
    };


    //Statements
    struct Stmt {
        Stmt(Token name): m_name(name) {}
        virtual ~Stmt() {}
        virtual std::string accept(StmtStringVisitor& visitor) = 0;
        virtual void accept(StmtVoidVisitor& visitor) = 0;
        Token m_name;
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


}


#endif //ZEBRA_STMT_H

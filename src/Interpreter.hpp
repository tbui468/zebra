#ifndef ZEBRA_INTERPRETER_H
#define ZEBRA_INTERPRETER_H

#include <vector>
#include <iostream>
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Environment.hpp"
#include "ResultCode.hpp"

namespace zebra {

    class Object;


    struct RuntimeError {
        Token m_token;
        std::string m_message;
        RuntimeError(Token token, const std::string& message): m_token(token), m_message(message) {}
        void print() {
            std::cout << "[Line " << m_token.m_line << "] Runtime Error: " << m_message << std::endl;
        }
    };


    class Interpreter: public StmtVoidVisitor, public ExprObjectVisitor {
        private:
            bool m_error_flag;
            std::vector<RuntimeError> m_errors;
        public:
            std::shared_ptr<Environment> m_environment;
            std::shared_ptr<Environment> m_global;
        public:
            Interpreter();
            ~Interpreter();
            ResultCode run(const std::vector<std::shared_ptr<Expr>> expressions);
            std::vector<RuntimeError> get_errors() const;
            void add_error(Token token, const std::string& message);
            void execute(Stmt* stmt);
            std::shared_ptr<Object> evaluate(Expr* expr);
            void visit(AssignField* stmt);
            void visit(StructDecl* stmt);
            void visit(StructInst* stmt);
            std::shared_ptr<Object> visit(Unary* expr);
            std::shared_ptr<Object> visit(Binary* expr);
            std::shared_ptr<Object> visit(Group* expr);
            std::shared_ptr<Object> visit(Literal* expr);
            std::shared_ptr<Object> visit(Logic* expr);
            std::shared_ptr<Object> visit(Variable* expr);
            std::shared_ptr<Object> visit(StmtExpr* expr);
            std::shared_ptr<Object> visit(VarDecl* expr);
            std::shared_ptr<Object> visit(Assign* expr);
            std::shared_ptr<Object> visit(Block* expr);
            std::shared_ptr<Object> visit(If* expr);
            std::shared_ptr<Object> visit(For* expr);
            std::shared_ptr<Object> visit(While* expr);
            std::shared_ptr<Object> visit(FunDecl* expr);
            std::shared_ptr<Object> visit(Return* expr);
            std::shared_ptr<Object> visit(Call* expr);
            std::shared_ptr<Object> visit(MethodCall* expr);
            std::shared_ptr<Object> visit(Import* expr);
            std::shared_ptr<Object> visit(ClassDecl* expr);
            std::shared_ptr<Object> visit(InstClass* expr);
            std::shared_ptr<Object> visit(GetField* expr);
            std::shared_ptr<Object> visit(SetField* expr);

    };

}


#endif //ZEBRA_INTERPRETER_H

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
    };


    class Interpreter: public StmtVoidVisitor, public ExprObjectVisitor {
        private:
            bool m_error_flag;
            std::vector<RuntimeError> m_errors;
        public:
            std::shared_ptr<Environment> m_environment;
        public:
            Interpreter();
            ~Interpreter();
            ResultCode run(const std::vector<std::shared_ptr<Expr>> expressions);
            std::vector<RuntimeError> get_errors() const;
            void add_error(Token token, const std::string& message);
            void execute(Stmt* stmt);
            std::shared_ptr<Object> evaluate(Expr* expr);
            void visit(If* stmt);
            void visit(While* stmt);
            void visit(For* stmt);
            void visit(Return* stmt);
            void visit(AssignField* stmt);
            void visit(FunDecl* stmt);
            void visit(StructDecl* stmt);
            void visit(StructInst* stmt);
            void visit(Call* stmt);
            void visit(Import* stmt);
            std::shared_ptr<Object> visit(Unary* expr);
            std::shared_ptr<Object> visit(Binary* expr);
            std::shared_ptr<Object> visit(Group* expr);
            std::shared_ptr<Object> visit(Literal* expr);
            std::shared_ptr<Object> visit(Logic* expr);
            std::shared_ptr<Object> visit(Variable* expr);
            std::shared_ptr<Object> visit(Access* expr);
            std::shared_ptr<Object> visit(StmtExpr* expr);
            std::shared_ptr<Object> visit(VarDecl* expr);
            std::shared_ptr<Object> visit(Assign* expr);
            std::shared_ptr<Object> visit(Block* expr);
            std::shared_ptr<Object> visit(Print* expr);

    };

}


#endif //ZEBRA_INTERPRETER_H

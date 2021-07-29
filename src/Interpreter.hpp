#ifndef ZEBRA_INTERPRETER_H
#define ZEBRA_INTERPRETER_H

#include <vector>
#include <iostream>
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Environment.hpp"

namespace zebra {

    class Object;

    class Interpreter: public StmtVoidVisitor, public ExprObjectVisitor {
        public:
            std::vector<std::shared_ptr<Stmt>> m_statements;
            std::shared_ptr<Environment> m_environment;
        public:
            Interpreter(const std::vector<std::shared_ptr<Stmt>> statements);
            ~Interpreter();
            void run();
            void execute(Stmt* stmt);
            std::shared_ptr<Object> evaluate(Expr* expr);
            void visit(If* stmt);
            void visit(Block* stmt);
            void visit(While* stmt);
            void visit(For* stmt);
            void visit(Return* stmt);
            void visit(Assign* stmt);
            void visit(AssignField* stmt);
            void visit(VarDecl* stmt);
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

    };

}


#endif //ZEBRA_INTERPRETER_H

#include "Interpreter.hpp"
#include "Object.hpp"

namespace zebra {

    Interpreter::Interpreter() {
        m_environment = std::make_shared<Environment>();
    }

    Interpreter::~Interpreter() {}

    ResultCode Interpreter::run(const std::vector<std::shared_ptr<Expr>> expressions) {
        for(std::shared_ptr<Expr> expr: expressions) {
            evaluate(expr.get());
        }

        if(!m_error_flag) {
            return ResultCode::SUCCESS;
        } else {
            return ResultCode::FAILED;
        }
    }

    std::vector<RuntimeError> Interpreter::get_errors() const {
        return m_errors;
    }

    void Interpreter::add_error(Token token, const std::string& message) {
        m_errors.emplace_back(token, message);
    }

    void Interpreter::execute(Stmt* stmt) {
        if (m_environment->get_return()) {
            return;
        }
        stmt->accept(*this);
    }

    std::shared_ptr<Object> Interpreter::evaluate(Expr* expr) {
        return expr->accept(*this);
    }

    void Interpreter::visit(If* stmt) {
        std::shared_ptr<Object> condition = evaluate(stmt->m_condition.get());
        if(dynamic_cast<Bool*>(condition.get()) && dynamic_cast<Bool*>(condition.get())->m_value) {
            execute(stmt->m_then_branch.get());                    
        }else if(stmt->m_else_branch) {
            execute(stmt->m_else_branch.get());
        }
    }

    void Interpreter::visit(Block* stmt) {
        std::shared_ptr<Environment> block_env = std::make_shared<Environment>(m_environment, false);
        std::shared_ptr<Environment> closure = m_environment;
        m_environment = block_env;
        for(std::shared_ptr<Stmt> s: stmt->m_statements) {
            execute(s.get());  
            if( dynamic_cast<Return*>(s.get())) {
                break;
            }
        } 
        m_environment = closure;   
    }

    void Interpreter::visit(While* stmt) {
        std::shared_ptr<Expr> condition = stmt->m_condition;
        while(dynamic_cast<Bool*>(evaluate(condition.get()).get())->m_value) {
            execute(stmt->m_body.get());
        }
    }

    void Interpreter::visit(For* stmt) {
        execute(stmt->m_initializer.get());
        std::shared_ptr<Expr> condition = stmt->m_condition;
        while(dynamic_cast<Bool*>(evaluate(condition.get()).get())->m_value) {
            execute(stmt->m_body.get());
            evaluate(stmt->m_update.get()); //not using result of expression
        }
    }

    void Interpreter::visit(Return* stmt) {
        std::shared_ptr<Object> ret = evaluate(stmt->m_value.get());
        m_environment->set_return(ret);
    }

    void Interpreter::visit(AssignField* stmt) {
        StructInstance* inst = dynamic_cast<StructInstance*>(m_environment->get(stmt->m_instance).get());
        inst->m_fields[stmt->m_field.m_lexeme] = evaluate(stmt->m_value.get());
    }

    void Interpreter::visit(FunDecl* stmt) {
        std::shared_ptr<Object> fun = std::make_shared<FunDef>(stmt->m_parameters, stmt->m_body);
        m_environment->define(stmt->m_name, fun);
    }

    void Interpreter::visit(StructDecl* stmt) {
        std::shared_ptr<Object> struct_def = std::make_shared<StructDef>(stmt);
        m_environment->define(stmt->m_name, struct_def);
    }

    void Interpreter::visit(StructInst* stmt) {
        StructDef* def = dynamic_cast<StructDef*>(m_environment->get(stmt->m_struct).get());
        std::unordered_map<std::string, std::shared_ptr<Object>> fields;
        if (!(stmt->m_arguments.empty())) {
            for (int i = 0; i < def->m_node->m_fields.size(); i++) {
                //get lexeme from declaration
                std::string lexeme = def->m_node->m_fields.at(i)->m_name.m_lexeme;
                //get value from argument evaluation
                std::shared_ptr<Object> value = evaluate(stmt->m_arguments.at(i).get());
                fields[lexeme] = value;
            }
        } else { //use default values in struct definition
            for (std::shared_ptr<VarDecl> var_decl: def->m_node->m_fields) {
                std::string lexeme = var_decl->m_name.m_lexeme;
                std::shared_ptr<Object> value = evaluate(var_decl->m_value.get());
                fields[lexeme] = value;
            }
        }

        std::shared_ptr<Object> struct_instance = std::make_shared<StructInstance>(fields);
        m_environment->define(stmt->m_name, struct_instance);
    }

    //FunDef is not even being used - it's just a wrapper for FunDecl
    void Interpreter::visit(Call* stmt) {
        std::shared_ptr<Object> obj = m_environment->get(stmt->m_name);
        Callable* fun = dynamic_cast<Callable*>(obj.get());

        //evaluate call arguments
        std::vector<std::shared_ptr<Object>> arguments;
        for (std::shared_ptr<Expr> e: stmt->m_arguments) {
            arguments.push_back(evaluate(e.get()));
        }

        std::shared_ptr<Environment> block_env = std::make_shared<Environment>(m_environment, true);
        std::shared_ptr<Environment> closure = m_environment;
        m_environment = block_env;

        std::shared_ptr<Object> return_value = fun->call(arguments, this);

        m_environment = closure;

        stmt->m_return = return_value;
    } 

    
    void Interpreter::visit(Import* stmt) {
        for (std::pair<std::string, std::shared_ptr<Object>> p: stmt->m_functions) {
            m_environment->define(Token(TokenType::FUN_TYPE, p.first, 1), p.second);    
        }
    }

    /*
     * Expressions
     */


    std::shared_ptr<Object> Interpreter::visit(Unary* expr) {
        std::shared_ptr<Object> right = expr->m_right->accept(*this);

        if(expr->m_op.m_type == TokenType::MINUS) {
            if (dynamic_cast<Float*>(right.get())) {
                return std::make_shared<Float>(-dynamic_cast<Float*>(right.get())->m_value);
            }else if (dynamic_cast<Int*>(right.get())) {
                return std::make_shared<Int>(-dynamic_cast<Int*>(right.get())->m_value);
            }
        }else if(expr->m_op.m_type == TokenType::BANG) {
            return std::make_shared<Bool>(!dynamic_cast<Bool*>(right.get())->m_value);
        }

    }

    std::shared_ptr<Object> Interpreter::visit(Binary* expr) {
        std::shared_ptr<Object> left = expr->m_left->accept(*this);
        std::shared_ptr<Object> right = expr->m_right->accept(*this);

        if(dynamic_cast<Int*>(left.get())) {
            int a = dynamic_cast<Int*>(left.get())->m_value;
            int b = dynamic_cast<Int*>(right.get())->m_value;
            switch(expr->m_op.m_type) {
                case TokenType::PLUS: return std::make_shared<Int>(a + b);
                case TokenType::MINUS: return std::make_shared<Int>(a - b);
                case TokenType::STAR: return std::make_shared<Int>(a * b);
                case TokenType::SLASH: return std::make_shared<Int>(a / b);
                case TokenType::MOD: return std::make_shared<Int>(a % b);
            }
        }
        if(dynamic_cast<Float*>(left.get())) {
            float a = dynamic_cast<Float*>(left.get())->m_value;
            float b = dynamic_cast<Float*>(right.get())->m_value;
            switch(expr->m_op.m_type) {
                case TokenType::PLUS: return std::make_shared<Float>(a + b);
                case TokenType::MINUS: return std::make_shared<Float>(a - b);
                case TokenType::STAR: return std::make_shared<Float>(a * b);
                case TokenType::SLASH: return std::make_shared<Float>(a / b);
            }
        }
        if(dynamic_cast<String*>(left.get())) {
            std::string a = dynamic_cast<String*>(left.get())->m_value;
            std::string b = dynamic_cast<String*>(right.get())->m_value;
            switch(expr->m_op.m_type) {
                case TokenType::PLUS: return std::make_shared<String>(a + b);
            }
        }

    }
    std::shared_ptr<Object> Interpreter::visit(Group* expr) {
        return expr->m_expr->accept(*this);
    }
    std::shared_ptr<Object> Interpreter::visit(Literal* expr) {
        switch(expr->m_token.m_type) {
            case TokenType::FLOAT:
                return std::make_shared<Float>(stof(expr->m_token.m_lexeme));
            case TokenType::INT:
                return std::make_shared<Int>(stoi(expr->m_token.m_lexeme));
            case TokenType::STRING:
                return std::make_shared<String>(expr->m_token.m_lexeme);
            case TokenType::TRUE:
                return std::make_shared<Bool>(true);
            case TokenType::FALSE:
                return std::make_shared<Bool>(false);
            case TokenType::NIL:
                return std::make_shared<Nil>();
        }
    }

    std::shared_ptr<Object> Interpreter::visit(Logic* expr) {

        std::shared_ptr<Object> left = expr->m_left->accept(*this);
        std::shared_ptr<Object> right = expr->m_right->accept(*this);

        Bool* bool_left = dynamic_cast<Bool*>(left.get());
        Bool* bool_right = dynamic_cast<Bool*>(right.get());
        if(bool_left && bool_right) {
            switch(expr->m_op.m_type) {
                case TokenType::OR:
                    return std::make_shared<Bool>(bool_left->m_value || bool_right->m_value);
                case TokenType::AND:
                    return std::make_shared<Bool>(bool_left->m_value && bool_right->m_value);
                case TokenType::EQUAL_EQUAL: 
                    return std::make_shared<Bool>(bool_left->m_value == bool_right->m_value);
                case TokenType::BANG_EQUAL:
                    return std::make_shared<Bool>(bool_left->m_value != bool_right->m_value);
            }
        }

        Int* int_left = dynamic_cast<Int*>(left.get());
        Int* int_right = dynamic_cast<Int*>(right.get());
        if(int_left && int_right) {
            switch(expr->m_op.m_type) {
                case TokenType::EQUAL_EQUAL:
                    return std::make_shared<Bool>(int_left->m_value == int_right->m_value);
                case TokenType::BANG_EQUAL:
                    return std::make_shared<Bool>(int_left->m_value != int_right->m_value);
                case TokenType::LESS:
                    return std::make_shared<Bool>(int_left->m_value < int_right->m_value);
                case TokenType::LESS_EQUAL:
                    return std::make_shared<Bool>(int_left->m_value <= int_right->m_value);
                case TokenType::GREATER:
                    return std::make_shared<Bool>(int_left->m_value > int_right->m_value);
                case TokenType::GREATER_EQUAL:
                    return std::make_shared<Bool>(int_left->m_value >= int_right->m_value);
            }
        }

        Float* float_left   =   dynamic_cast<Float*>(left.get());
        Float* float_right   =   dynamic_cast<Float*>(right.get());
        if(float_left && float_right) {
            switch(expr->m_op.m_type) {
                case TokenType::EQUAL_EQUAL: 
                    return std::make_shared<Bool>(abs(float_left->m_value - float_right->m_value) < 0.01f);
                case TokenType::BANG_EQUAL:
                    return std::make_shared<Bool>(abs(float_left->m_value - float_right->m_value) >= 0.01f);
                case TokenType::LESS:
                    return std::make_shared<Bool>(float_left->m_value < float_right->m_value);
                case TokenType::LESS_EQUAL:
                    return std::make_shared<Bool>(float_left->m_value < float_right->m_value ||
                            abs(float_left->m_value - float_right->m_value) < 0.01f);
                case TokenType::GREATER:
                    return std::make_shared<Bool>(float_left->m_value > float_right->m_value);
                case TokenType::GREATER_EQUAL:
                    return std::make_shared<Bool>(float_left->m_value > float_right->m_value ||
                            abs(float_left->m_value - float_right->m_value) < 0.01f);
            }
        }

        String* string_left =   dynamic_cast<String*>(left.get());
        String* string_right =   dynamic_cast<String*>(right.get());
        if(string_left && string_right) {
            switch(expr->m_op.m_type) {
                case TokenType::EQUAL_EQUAL: 
                    return std::make_shared<Bool>(string_left->m_value == string_right->m_value);
                case TokenType::BANG_EQUAL:
                    return std::make_shared<Bool>(string_left->m_value != string_right->m_value);
            }
        }

    }

    std::shared_ptr<Object> Interpreter::visit(Variable* expr) {
        return m_environment->get(expr->m_name);
    }

    std::shared_ptr<Object> Interpreter::visit(Access* expr) {
        StructInstance* inst = dynamic_cast<StructInstance*>(m_environment->get(expr->m_instance).get());
        return inst->m_fields[expr->m_field.m_lexeme];
    }

    std::shared_ptr<Object> Interpreter::visit(StmtExpr* expr) {
        execute(expr->m_stmt.get());

        Call* call = dynamic_cast<Call*>(expr->m_stmt.get());
        if (call) {
            return call->m_return;
        }

        Assign* assignment = dynamic_cast<Assign*>(expr->m_stmt.get());
        if (assignment) {
            return m_environment->get(assignment->m_name);
        }

        VarDecl* var_decl = dynamic_cast<VarDecl*>(expr->m_stmt.get());
        if (var_decl) {
            return m_environment->get(var_decl->m_name);
        }
        
        add_error(expr->m_stmt->m_name, "Invalid StmtExpr");
    }

    std::shared_ptr<Object> Interpreter::visit(VarDecl* expr) {
        std::shared_ptr<Object> value = evaluate(expr->m_value.get());
        m_environment->define(expr->m_name, value);
        return value;
    }

    std::shared_ptr<Object> Interpreter::visit(Assign* expr) {
        std::shared_ptr<Object> value = evaluate(expr->m_value.get());
        m_environment->assign(expr->m_name, value);
        return value;
    }

    std::shared_ptr<Object> Interpreter::visit(Print* expr) {
        std::shared_ptr<Object> value = evaluate(expr->m_value.get());

        if(dynamic_cast<Bool*>(value.get())) {
            std::cout << dynamic_cast<Bool*>(value.get())->m_value << std::endl;
        }
        if(dynamic_cast<Int*>(value.get())) {
            std::cout << dynamic_cast<Int*>(value.get())->m_value << std::endl;
        }
        if(dynamic_cast<Float*>(value.get())) {
            std::cout << dynamic_cast<Float*>(value.get())->m_value << std::endl;
        }
        if(dynamic_cast<String*>(value.get())) {
            std::cout << dynamic_cast<String*>(value.get())->m_value << std::endl;
        }

        return value;
    }


}

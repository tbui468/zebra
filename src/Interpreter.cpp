#include "Interpreter.hpp"
#include "Object.hpp"

namespace zebra {

    Interpreter::Interpreter() {
        m_global = std::make_shared<Environment>();
        m_environment = std::make_shared<Environment>(m_global, false);
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

    std::shared_ptr<Object> Interpreter::evaluate(Expr* expr) {
        return expr->accept(*this);
    }

    /*
     * Misc.
     */

    std::shared_ptr<Object> Interpreter::visit(Import* expr) {
        for (std::pair<std::string, std::shared_ptr<Object>> p: expr->m_functions) {
            m_environment->define_global(Token(TokenType::FUN_TYPE, p.first, 1), p.second);    
        }

        return std::shared_ptr<Nil>();
    }

    /*
     * Basic
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

    /*
     * Variables and Functions
     */

    std::shared_ptr<Object> Interpreter::visit(DeclVar* expr) {
        std::shared_ptr<Object> value = evaluate(expr->m_value.get());
        m_environment->define(expr->m_name, value);
        return value;
    }

    std::shared_ptr<Object> Interpreter::visit(GetVar* expr) {
        if (expr->m_env.m_type != TokenType::NIL) {
            ClassInst* inst = dynamic_cast<ClassInst*>(m_environment->get(expr->m_env).get()); 
            return inst->m_environment->get(expr->m_name);
        }

        return m_environment->get(expr->m_name);
    }

    std::shared_ptr<Object> Interpreter::visit(SetVar* expr) {
        if (expr->m_env.m_type != TokenType::NIL) {
            ClassInst* inst = dynamic_cast<ClassInst*>(m_environment->get(expr->m_env).get());
            std::shared_ptr<Object> value = evaluate(expr->m_value.get());
            inst->m_environment->assign(expr->m_name, value);
            return value;
        }
        std::shared_ptr<Object> value = evaluate(expr->m_value.get());
        m_environment->assign(expr->m_name, value);
        return value;
    }

    std::shared_ptr<Object> Interpreter::visit(DeclFun* expr) {
        std::shared_ptr<Object> fun = std::make_shared<FunDef>(expr->m_parameters, expr->m_body);
        m_environment->define(expr->m_name, fun);
        return fun;
    }


    std::shared_ptr<Object> Interpreter::visit(CallFun* expr) {
        /*
         * Instance method
         */
        if (expr->m_env.m_type != TokenType::NIL) {
            ClassInst* inst = dynamic_cast<ClassInst*>(m_environment->get(expr->m_env).get());
            Callable* method = dynamic_cast<Callable*>(inst->m_environment->get(expr->m_name).get());

            //evaluate call arguments
            std::vector<std::shared_ptr<Object>> arguments;
            for (std::shared_ptr<Expr> e: expr->m_arguments) {
                arguments.push_back(evaluate(e.get()));
            }

            //create new env pointing at instance env. with is_func set to true
            std::shared_ptr<Environment> method_env = std::make_shared<Environment>(inst->m_environment, true);
            std::shared_ptr<Environment> closure = m_environment;
            m_environment = method_env;

            std::shared_ptr<Object> return_value = method->call(arguments, this);

            m_environment = closure;

            expr->m_return = return_value;

            return return_value;
        }
        
        /*
         * Regular Function
         */
        std::shared_ptr<Object> obj = m_environment->get(expr->m_name);
        Callable* fun = dynamic_cast<Callable*>(obj.get());

        //evaluate call arguments
        std::vector<std::shared_ptr<Object>> arguments;
        for (std::shared_ptr<Expr> e: expr->m_arguments) {
            arguments.push_back(evaluate(e.get()));
        }

        std::shared_ptr<Environment> block_env = std::make_shared<Environment>(m_environment, true);
        std::shared_ptr<Environment> closure = m_environment;
        m_environment = block_env;

        std::shared_ptr<Object> return_value = fun->call(arguments, this);

        m_environment = closure;

        expr->m_return = return_value;

        return return_value;
    } 

    std::shared_ptr<Object> Interpreter::visit(Return* expr) {
        if (expr->m_value) {
            std::shared_ptr<Object> ret = evaluate(expr->m_value.get());
            m_environment->set_return(ret);
            return ret;
        } else {
            std::shared_ptr<Object> ret = std::make_shared<Nil>();
            m_environment->set_return(ret);
            return ret;
        }
    }


    /*
     * Control Flow
     */

    std::shared_ptr<Object> Interpreter::visit(Block* expr) {
        std::shared_ptr<Environment> block_env = std::make_shared<Environment>(m_environment, false);
        std::shared_ptr<Environment> closure = m_environment;
        m_environment = block_env;

        for(std::shared_ptr<Expr> e: expr->m_expressions) {
            evaluate(e.get());  
            if (dynamic_cast<Return*>(e.get())) {
                break;
            }
        } 

        m_environment = closure;   

        return std::make_shared<Nil>();
    }

    std::shared_ptr<Object> Interpreter::visit(If* expr) {
        std::shared_ptr<Object> condition = evaluate(expr->m_condition.get());
        if(dynamic_cast<Bool*>(condition.get())->m_value) {
            evaluate(expr->m_then_branch.get());                    
        }else if(expr->m_else_branch) {
            evaluate(expr->m_else_branch.get());
        }

        return std::make_shared<Nil>();
    }

    std::shared_ptr<Object> Interpreter::visit(For* expr) {
        if(expr->m_initializer) evaluate(expr->m_initializer.get());

        while(expr->m_condition && dynamic_cast<Bool*>(evaluate(expr->m_condition.get()).get())->m_value) {
            evaluate(expr->m_body.get());
            if(expr->m_update) evaluate(expr->m_update.get()); //not using result of expression
        }

        return std::make_shared<Nil>();
    }

    std::shared_ptr<Object> Interpreter::visit(While* expr) {
        while(dynamic_cast<Bool*>(evaluate(expr->m_condition.get()).get())->m_value) {
            evaluate(expr->m_body.get());
        }

        return std::make_shared<Nil>();
    }

    /*
     * Classes
     */
    
    std::shared_ptr<Object> Interpreter::visit(DeclClass* expr) {
        std::vector<std::pair<Token, std::shared_ptr<Object>>> fields;
        for (std::shared_ptr<Expr> field: expr->m_fields) {
            std::shared_ptr<Object> value = evaluate(field.get());
            Token token = dynamic_cast<DeclVar*>(field.get())->m_name;
            fields.push_back(std::pair<Token, std::shared_ptr<Object>>(token, value));
        }

        std::vector<std::pair<Token, std::shared_ptr<Object>>> methods;
        for (std::shared_ptr<Expr> method: expr->m_methods) {
            DeclFun* method_decl = dynamic_cast<DeclFun*>(method.get());
            std::shared_ptr<Object> fun = std::make_shared<FunDef>(method_decl->m_parameters, method_decl->m_body);
            methods.push_back(std::pair<Token, std::shared_ptr<Object>>(method_decl->m_name, fun));
        }

        //base is a pointer to base class Object (ClassDef)
        std::shared_ptr<Object> base = nullptr;
        if (expr->m_base.m_type != TokenType::NIL) {
            base = m_environment->get(expr->m_base);
        }

        std::shared_ptr<Object> class_def = std::make_shared<ClassDef>(base, fields, methods);
        m_environment->define(expr->m_name, class_def);

        return class_def;
    }
   

}

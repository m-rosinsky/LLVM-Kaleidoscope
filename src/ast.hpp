/*!
 * @file src/ast.hpp
 *
 * @brief This file contains the class implementations for the AST.
 */

#ifndef _LLVM_AST_H
#define _LLVM_AST_H

#include <memory>
#include <string>
#include <vector>

/*!
 * @brief This class is the base class for all expression nodes.
 */
class ExprAST
{
public:
    // Virtual destructor.
    virtual ~ExprAST() = default;
};

/*!
 * @brief This class handles expressions for numeric literals such as "1.0".
 */
class NumberExprAST : public ExprAST
{
private:
    // The numeric value.
    double val;

public:
    // Ctor.
    NumberExprAST()
        : val(0.0) {}
    NumberExprAST(double val)
        : val(val) {}
};

/*!
 * @brief This class handle variables, such as "a".
 */
class VariableExprAST : public ExprAST
{
private:
    // The variable name.
    std::string name;

public:
    // Ctor.
    VariableExprAST()
        : name("") {}
    VariableExprAST(const std::string& name)
        : name(name) {}
};

/*!
 * @brief This class handles binary operators.
 */
class BinaryExprAST : public ExprAST
{
private:
    // The operator.
    char op;
    // The operands.
    std::unique_ptr<ExprAST> lhs;
    std::unique_ptr<ExprAST> rhs;

public:
    // Ctor.
    BinaryExprAST(char op,
                  std::unique_ptr<ExprAST> lhs,
                  std::unique_ptr<ExprAST> rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

/*!
 * @brief This class handles function calls.
 */
class CallExprAST : public ExprAST
{
private:
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args;

public:
    CallExprAST(const std::string& callee,
                std::vector<std::unique_ptr<ExprAST>> args)
        : callee(callee), args(std::move(args)) {}
};

/*!
 * @brief This class represents the "prototype" for a function.
 *
 *          This captures its name and its argument names (which
 *              implicitly captures the number of args the function takes).
 */
class PrototypeAST
{
private:
    std::string name;
    std::vector<std::string> args;

public:
    PrototypeAST(const std::string& name,
                 std::vector<std::string> args)
        : name(name), args(std::move(args)) {}

    const std::string& get_name() const noexcept { return name; }
};

/*!
 * @brief This class represents a function definition itself.
 */
class FunctionAST
{
private:
    std::unique_ptr<PrototypeAST> proto;
    std::unique_ptr<ExprAST> body;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto,
                std::unique_ptr<ExprAST> body)
        : proto(std::move(proto)), body(std::move(body)) {}
};

#endif // _LLVM_AST_H

/***   end of file   ***/

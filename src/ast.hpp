/*!
 * @file src/ast.hpp
 *
 * @brief This file contains the class implementations for the AST.
 */

#ifndef _LLVM_AST_H
#define _LLVM_AST_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

extern std::unique_ptr<llvm::LLVMContext> g_context;
extern std::unique_ptr<llvm::IRBuilder<>> g_builder;
extern std::unique_ptr<llvm::Module> g_module;
extern std::map<std::string, llvm::Value *> g_named_values;

/*!
 * @brief This class is the base class for all expression nodes.
 */
class ExprAST
{
public:
    // Virtual destructor.
    virtual ~ExprAST() = default;

    // Pure virtual codegen function.
    virtual llvm::Value * codegen() = 0;
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

    llvm::Value * codegen() override;
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

    llvm::Value * codegen() override;
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

    llvm::Value * codegen() override;
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

    llvm::Value * codegen() override;
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

    llvm::Function * codegen();
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

    llvm::Function * codegen();
};

/*!
 * @brief This function is a helper function for error handling.
 *
 * @param p_str The error message to print.
 */
std::unique_ptr<ExprAST>
log_error (const char * p_str);

/*!
 * @brief This function is a helper function for prototype error handling.
 *
 * @param p_str The error message to print.
 */
std::unique_ptr<PrototypeAST>
log_error_p (const char * p_str);

#endif // _LLVM_AST_H

/***   end of file   ***/

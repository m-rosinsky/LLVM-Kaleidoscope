/*!
 * @file src/ast.cpp
 *
 * @brief This file contains the class implementations for the AST.
 */

#include "ast.hpp"

/*!
 * @brief These are static globals for codegen functions.
 */
std::unique_ptr<llvm::LLVMContext> g_context = std::make_unique<llvm::LLVMContext>();
std::unique_ptr<llvm::IRBuilder<>> g_builder = std::make_unique<llvm::IRBuilder<>>(*g_context);
std::unique_ptr<llvm::Module> g_module;
std::map<std::string, llvm::Value *> g_named_values;

/*!
 * @brief This function is used for error handling.
 *
 * @param p_str The error message to print.
 */
std::unique_ptr<ExprAST>
log_error (const char * p_str)
{
    fprintf(stderr, "Error: %s\n", p_str);
    return nullptr;
}

/*!
 * @brief This function is used for prototype error handling.
 *
 * @param p_str The error message to print.
 */
std::unique_ptr<PrototypeAST>
log_error_p (const char * p_str)
{
    log_error(p_str);
    return nullptr;
}

/*!
 * @brief This function is used for codegen error handling.
 */
llvm::Value *
log_error_v (const char * p_str)
{
    log_error(p_str);
    return nullptr;
}

/******************************************************************************/

/*!
 * @brief This function generates code for a NumberExprAST object.
 */
llvm::Value *
NumberExprAST::codegen()
{
    return llvm::ConstantFP::get(*g_context, llvm::APFloat(val));
}

/******************************************************************************/

/*!
 * @brief This function generates code for a VariableExprAST object.
 */
llvm::Value *
VariableExprAST::codegen()
{
    llvm::Value * v = g_named_values[name];
    if (!v)
    {
        log_error_v("Unknown variable name");
    }

    return v;
}

/******************************************************************************/

/*!
 * @brief This function generates code for a BinaryExprAST object.
 */
llvm::Value *
BinaryExprAST::codegen()
{
    // Generate code for the left and right hand side of the operator.
    llvm::Value * l = lhs->codegen();
    llvm::Value * r = rhs->codegen();

    // Ensure both returned without error.
    if (!l || !r)
    {
        return nullptr;
    }

    // Handle various binary operators.
    switch (op)
    {
        // Addition.
        case '+':
            return g_builder->CreateFAdd(l, r, "addtmp");
        break;

        // Subtraction.
        case '-':
            return g_builder->CreateFSub(l, r, "subtmp");
        break;

        // Multiplication.
        case '*':
            return g_builder->CreateFMul(l, r, "multmp");
        break;

        // Comparison.
        case '<':
            l = g_builder->CreateFCmpULT(l, r, "cmptmp");

            return g_builder->CreateUIToFP(l, llvm::Type::getDoubleTy(*g_context), "booltmp");
        break;

        // Invalid operators.
        default:
            return log_error_v("Invalid binary operator");
        break;
    }
}

/******************************************************************************/

/*!
 * @brief This function generates code for a CallExprAST object.
 */
llvm::Value *
CallExprAST::codegen()
{
    // Look up the name in the global module table.
    llvm::Function * callee_f = g_module->getFunction(callee);
    if (!callee_f)
    {
        return log_error_v("Unknown function referenced");
    }

    // Check that correct number of args were passed.
    if (callee_f->arg_size() != args.size())
    {
        return log_error_v("Incorrect number of args passed");
    }

    // Create a vector to hold code for all args.
    std::vector<llvm::Value *> args_v;
    for (unsigned i = 0, e = args.size(); i != e; ++i)
    {
        args_v.push_back(args[i]->codegen());
        if (!args_v.back())
        {
            return nullptr;
        }
    }

    // Create the call IR with the callee and args.
    return g_builder->CreateCall(callee_f, args_v, "calltmp");
}

/******************************************************************************/

llvm::Function *
PrototypeAST::codegen()
{
    // Make the function type.
    std::vector<llvm::Type *> doubles(
        args.size(),
        llvm::Type::getDoubleTy(*g_context)
    );

    llvm::FunctionType * ft = llvm::FunctionType::get(
        llvm::Type::getDoubleTy(*g_context),
        doubles,
        false
    );

    llvm::Function * f = llvm::Function::Create(
        ft,
        llvm::Function::ExternalLinkage,
        name,
        g_module.get()
    );

    // Fill the argument names.
    unsigned idx = 0;
    for (auto &arg : f->args())
    {
        arg.setName(args[idx++]);
    }

    return f;
}

/******************************************************************************/

llvm::Function *
FunctionAST::codegen()
{
    // Check for an existing function from a previous extern decl.
    llvm::Function * the_func = g_module->getFunction(proto->get_name());

    // If nothing was returned, perform code generation.
    if (!the_func)
    {
        the_func = proto->codegen();
    }

    // Error check code gen.
    if (!the_func)
    {
        return nullptr;
    }

    // Assert that the function has no body before codegen starts.
    if (!the_func->empty())
    {
        return (llvm::Function *) log_error_v("Function cannot be redefined");
    }

    // Create a basic block to start insertion into.
    llvm::BasicBlock * bb = llvm::BasicBlock::Create(
        *g_context,
        "entry",
        the_func
    );

    // Set the builder's insertion point.
    g_builder->SetInsertPoint(bb);

    // Record the function args in the named_values map.
    g_named_values.clear();
    for (auto &arg : the_func->args())
    {
        g_named_values[std::string(arg.getName())] = &arg;
    }

    // Generate code for the function body.
    llvm::Value * ret_val = body->codegen();
    if (!ret_val)
    {
        // Error reading body, remove the function.
        the_func->eraseFromParent();
        return nullptr;
    }

    // Finish off the function.
    g_builder->CreateRet(ret_val);

    // Validate the generated code, checking for consistency.
    llvm::verifyFunction(*the_func);

    // Return the finished function.
    return the_func;
}

/***   end of file   ***/

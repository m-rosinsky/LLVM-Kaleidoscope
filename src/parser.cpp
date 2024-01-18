/*!
 * @file src/parser.cpp
 *
 * @brief This file contains the functionality of the parser.
 */

#include <map>
#include <memory>

#include "parser.hpp"

// This map holds the precedence of binary operators.
static std::map<char, int> binop_precedence;

int cur_tok = 0;

/*!
 * @brief This function returns the precedence of a given binary operator.
 */
int
get_tok_precedence (void)
{
    if (!isascii(cur_tok))
    {
        return -1;
    }

    // Make sure it's a declared binary operator.
    int tok_prec = binop_precedence[cur_tok];
    if (tok_prec <= 0)
    {
        return -1;
    }
    return tok_prec;
}

/*!
 * @brief This function reads another token from the lexer and updates
 *          cur_tok with its results.
 */
int
get_next_token()
{
    return cur_tok = gettok();
}

/*!
 * @brief This function is a helper function for error handling.
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
 * @brief This function is a helper function for prototype error handling.
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
 * @brief This function parses variable references and function calls.
 *
 * @return Pointer to an AST expression.
 */
std::unique_ptr<ExprAST>
parse_identifier_expr (void)
{
    std::string id_name = identifier_str;

    // Consume the identifier.
    get_next_token();

    // Simple variable reference.
    if ('(' != cur_tok)
    {
        return std::make_unique<VariableExprAST>(id_name);
    }

    // Function call.
    get_next_token(); // Consume opening paren.
    std::vector<std::unique_ptr<ExprAST>> args;

    // Collect arguments.
    if (')' != cur_tok)
    {
        for (;;)
        {
            auto arg = parse_expression();
            if (arg)
            {
                args.push_back(std::move(arg));
            }
            else
            {
                // An error occurred.
                return nullptr;
            }

            // Check for closing paren.
            if (')' == cur_tok)
            {
                break;
            }

            // Expects comma separated list.
            if (',' != cur_tok)
            {
                return log_error("Expected ')' or ',' in arg list");
            }

            // Collect next token.
            get_next_token();
        }
    }

    // Consume the closing paren.
    get_next_token();

    // Create the call expression from the function name and arg list.
    return std::make_unique<CallExprAST>(id_name, std::move(args));
}

/*!
 * @brief This function is a helper function to parse priamry expressions.
 */
std::unique_ptr<ExprAST>
parse_primary (void)
{
    switch (cur_tok)
    {
        case tok_identifier:
            return parse_identifier_expr();
        break;
        case tok_number:
            return parse_number_expr();
        break;
        case '(':
            return parse_paren_expr();
        break;
        default:
            return log_error("Unknown token when expecting an expression");
        break;
    }
}

/*!
 * @brief This function parses binary operators.
 *
 * @return Pointer to an AST expression.
 */
std::unique_ptr<ExprAST>
parse_expression (void)
{
    auto lhs = parse_primary();
    if (!lhs)
    {
        return nullptr;
    }

    return parse_binop_rhs(0, std::move(lhs));
}

/*!
 * @brief This is a helper function to parse binary operator pairs.
 *
 * @param expr_prec The minimum operator precedence that the function
 *                      is allowed to consume.
 * @param lhs The left hand expression.
 */
std::unique_ptr<ExprAST>
parse_binop_rhs (int expr_prec, std::unique_ptr<ExprAST> lhs)
{
    // If this is a binary operator, find its precedence.
    for (;;)
    {
        int tok_prec = get_tok_precedence();

        // If this is a binary operator that binds at least as tightly
        // as the current operator, consume it, otherwise we are done.
        if (tok_prec < expr_prec)
        {
            return lhs;
        }

        // We now know this is a binary operator.
        int binop = cur_tok;

        // Consume the operator.
        get_next_token();

        // Parse the primary expression after the operator.
        auto rhs = parse_primary();
        if (!rhs)
        {
            return nullptr;
        }
        
        // If binop binds less tightly with rhs than the operator after rhs,
        // let the pending operator take rhs as its lhs.
        int next_prec = get_tok_precedence();
        if (tok_prec < next_prec)
        {
            rhs = parse_binop_rhs(tok_prec + 1, std::move(rhs));
            if (!rhs)
            {
                return nullptr;
            }
        }

        // Merge lhs and rhs.
        lhs = std::make_unique<BinaryExprAST>(
            binop,
            std::move(lhs),
            std::move(rhs)
        );


    }
}

/*!
 * @brief This function parses numeric literals.
 *
 * @return Pointer to an AST expression.
 */
std::unique_ptr<ExprAST>
parse_number_expr (void)
{
    auto result = std::make_unique<NumberExprAST>(num_val);
    get_next_token(); // Consume the number.
    return std::move(result);
}

/*!
 * @brief This function parses parentheses operators.
 *
 * @return Pointer to an AST expression.
 */
std::unique_ptr<ExprAST>
parse_paren_expr (void)
{
    // Consume the opening paren.
    get_next_token();

    // Generate expressions inside the parens.
    auto v = parse_expression();
    if (!v)
    {
        return nullptr;
    }

    // Ensure a closing paren is consumed.
    if (')' != cur_tok)
    {
        return log_error("expected ')'");
    }

    // Consume the closing paren.
    get_next_token();

    // Return the expressions inside the parents.
    return v;
}

/*!
 * @brief This function parses function prototypes.
 *
 * @return Pointer to an AST expression.
 */
std::unique_ptr<PrototypeAST>
parse_prototype (void)
{
    if (cur_tok != tok_identifier)
    {
        return log_error_p("Expected function name in prototype");
    }

    // Save the function name.
    std::string func_name = identifier_str;

    // Consume the function name.
    get_next_token();

    // Expect an open paren.
    if (cur_tok != '(')
    {
        return log_error_p("Expected '(' in prototype");
    }

    // Read the list of argument names.
    std::vector<std::string> arg_names;
    while (get_next_token() == tok_identifier)
    {
        arg_names.push_back(identifier_str);
    }

    // Expect a close paren.
    if (cur_tok != ')')
    {
        return log_error_p("Expected ')' in prototype");
    }

    // Success, consume the closing paren.
    get_next_token();

    // Return the new function.
    return std::make_unique<PrototypeAST>(func_name, std::move(arg_names));
}

/*!
 * @brief This function parses the function definition keyword.
 */
std::unique_ptr<FunctionAST>
parse_definition (void)
{
    // Eat the 'def' keyword.
    get_next_token();

    // Parse the function prototype.
    auto proto = parse_prototype();
    if (!proto)
    {
        return nullptr;
    }

    // Parse the function body.
    auto body = parse_expression();
    if (!body)
    {
        return nullptr;
    }
    
    return std::make_unique<FunctionAST>(std::move(proto), std::move(body));
}

/*!
 * @brief This function parses the extern keyword.
 */
std::unique_ptr<PrototypeAST>
parse_extern (void)
{
    // Eat the 'extern' keyword.
    get_next_token();

    // Parse the prototype.
    return parse_prototype();
}

/*!
 * @brief This function parses an arbitrary top-level expression.
 */
std::unique_ptr<FunctionAST>
parse_top_level_expr (void)
{
    // Parse the expression.
    auto expr = parse_expression();
    if (!expr)
    {
        return nullptr;
    }

    // Make an anonymous prototype with no name or args.
    auto proto = std::make_unique<PrototypeAST>(
        "",
        std::vector<std::string>()
    );
    return std::make_unique<FunctionAST>(std::move(proto), std::move(expr));
}

void
handle_definition (void)
{
    if (parse_definition())
    {
        fprintf(stderr, "Parsed a function definition\n");
    }
    else
    {
        // Skip token for error recovery.
        get_next_token();
    }
}

void
handle_extern (void)
{
    if (parse_extern())
    {
        fprintf(stderr, "Parsed an extern\n");
    }
    else
    {
        // Skip token for error recovery.
        get_next_token();
    }
}

void
handle_top_level_expression (void)
{
    if (parse_top_level_expr())
    {
        fprintf(stderr, "Parsed a top-level expr\n");
    }
    else
    {
        // Skip token for error recovery.
        get_next_token();
    }
}

/*!
 * @brief This is the main parser loop for the parser.
 */
void
parse (void)
{
    // Install standard binary operators.
    binop_precedence['<'] = 10;
    binop_precedence['+'] = 20;
    binop_precedence['-'] = 30;
    binop_precedence['*'] = 40;

    // Prime the first token.
    fprintf(stderr, "ready> ");
    get_next_token();

    for (;;)
    {
        fprintf(stderr, "ready> ");
        switch (cur_tok)
        {
            case tok_eof:
                return;
            break;

            case ';':
                // Ignore top-level semicolons.
                get_next_token();
            break;

            case tok_def:
                handle_definition();
            break;

            case tok_extern:
                handle_extern();
            break;

            default:
                handle_top_level_expression();
            break;
        }
    }
}

/***   end of file   ***/

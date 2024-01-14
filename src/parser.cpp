/*!
 * @file src/parser.cpp
 *
 * @brief This file contains the functionality of the parser.
 */

#include "parser.hpp"

/*!
 * @brief This function reads another token from the lexer and updates
 *          cur_tok with its results.
 */
static int
get_next_token()
{
    return cur_tok = gettok();
}

/*!
 * @brief This function is a helper function for error handling.
 *
 * @param p_str The error message to print.
 */
static std::unique_ptr<ExprAST>
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
static std::unique_ptr<PrototypeAST>
log_error_p (const char * p_str)
{
    log_error(p_str);
    return nullptr;
}

/*!
 * @brief This function is a helper function to parse priamry expressions.
 */
static std::unique_ptr<ExprAST>
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

/***   end of file   ***/

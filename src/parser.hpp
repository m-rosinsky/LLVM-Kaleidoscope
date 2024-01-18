/*!
 * @file src/parser.hpp
 *
 * @brief This file contains the functionality of the parser.
 */

#ifndef _LLVM_PARSER_H
#define _LLVM_PARSER_H

#include <memory>

#include "ast.hpp"
#include "lexer.hpp"

// The current token the parser is looking at.
extern int cur_tok;

/*!
 * @brief This is the main parser loop for the parser.
 */
void
parse (void);

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

/*!
 * @brief This function returns the precedence of a given binary operator.
 */
int
get_tok_precedence (void);

/*!
 * @brief This function reads another token from the lexer and updates
 *          cur_tok with its results.
 */
int
get_next_token();

/*!
 * @brief This function parses variable references and function calls.
 *
 * @return Pointer to an AST expression.
 */
std::unique_ptr<ExprAST>
parse_identifier_expr (void);

/*!
 * @brief This function is a helper function to parse priamry expressions.
 */
std::unique_ptr<ExprAST>
parse_primary (void);

/*!
 * @brief This function parses binary operators.
 *
 * @return Pointer to an AST expression.
 */
std::unique_ptr<ExprAST>
parse_expression (void);

/*!
 * @brief This is a helper function to parse binary operator pairs.
 *
 * @param expr_prec The minimum operator precedence that the function
 *                      is allowed to consume.
 * @param lhs The left hand expression.
 */
std::unique_ptr<ExprAST>
parse_binop_rhs (int expr_prec, std::unique_ptr<ExprAST> lhs);

/*!
 * @brief This function parses numeric literals.
 *
 * @return Pointer to an AST expression.
 */
std::unique_ptr<ExprAST>
parse_number_expr (void);

/*!
 * @brief This function parses parentheses operators.
 *
 * @return Pointer to an AST expression.
 */
std::unique_ptr<ExprAST>
parse_paren_expr (void);

/*!
 * @brief This function parses function prototypes.
 *
 * @return Pointer to an AST expression.
 */
std::unique_ptr<PrototypeAST>
parse_prototype (void);

/*!
 * @brief This function parses the function definition keyword.
 */
std::unique_ptr<FunctionAST>
parse_definition (void);

/*!
 * @brief This function parses the extern keyword.
 */
std::unique_ptr<PrototypeAST>
parse_extern (void);

/*!
 * @brief This function parses an arbitrary top-level expression.
 */
std::unique_ptr<FunctionAST>
parse_top_level_expr (void);

void
handle_definition (void);

void
handle_extern (void);

void
handle_top_level_expression (void);

#endif // _LLVM_PARSER_H

/***   end of file   ***/

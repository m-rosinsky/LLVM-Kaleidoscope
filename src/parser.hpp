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
int cur_tok;

/*!
 * @brief This function reads another token from the lexer and updates
 *          cur_tok with its results.
 */
static int
get_next_token();

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
 * @brief This function parses variable references and function calls.
 *
 * @return Pointer to an AST expression.
 */
std::unique_ptr<ExprAST>
parse_identifier_expr (void);

#endif // _LLVM_PARSER_H

/***   end of file   ***/

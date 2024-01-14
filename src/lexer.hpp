/*!
 * @file src/lexer.hpp
 *
 * @brief This file contains the functionality of the lexer.
 */

#ifndef _LLVM_LEXER_H
#define _LLVM_LEXER_H

#include <string>

// Globals.
std::string identifier_str;  // Filled in if tok_identifier.
double num_val;              // Filled in if tok_number.

/*!
 * @brief This enum contains the types of tokens.
 */
enum Token
{
    tok_eof = -1,

    // Commands.
    tok_def = -2,
    tok_extern = -3,

    // Primary.
    tok_identifier = -4,
    tok_number = -5,
};

/*!
 * @brief This function is called to return the next token from standard
 *          input.
 */
int
gettok (void);

#endif // _LLVM_LEXER_H

/***   end of file   ***/

/*!
 * @file src/lexer.cpp
 *
 * @brief This file contains the functionality of the lexer.
 */

#include "lexer.hpp"

std::string identifier_str;
double num_val;

/*!
 * @brief This function is called to return the next token from standard
 *          input.
 */
int
gettok (void)
{
    static int last_char = ' ';

    // Skip any whitespace.
    while (isspace(last_char))
    {
        last_char = getchar();
    }

    // Handle identifiers.
    if (isalpha(last_char))
    {
        identifier_str = last_char;
        while (isalnum((last_char = getchar())))
        {
            identifier_str += last_char;
        }

        // Compare to reserved strings.
        if ("def" == identifier_str)
        {
            return tok_def;
        }
        if ("extern" == identifier_str)
        {
            return tok_extern;
        }

        // Not a reserved identifier.
        return tok_identifier;
    }

    // Handle numeric values.
    if (isdigit(last_char) || last_char == '.')
    {
        std::string num_str;
        do
        {
            num_str += last_char;
            last_char = getchar();
        } while (isdigit(last_char) || last_char == '.');

        // TODO: More robust error handling instead of strtod.
        num_val = strtod(num_str.c_str(), nullptr);
        return tok_number;
    }

    // Handle comments.
    if ('#' == last_char)
    {
        // Comment until EOL.
        do
        {
            last_char = getchar();
        } while (last_char != EOF && last_char != '\n' && last_char != '\r');

        if (last_char != EOF)
        {
            return gettok();
        }
    }

    // Handle (but do not eat) EOF.
    if (last_char == EOF)
    {
        return tok_eof;
    }

    // Return the character as its ASCII value.
    int this_char = last_char;
    last_char = getchar();
    return this_char;
}

/***   end of file   ***/

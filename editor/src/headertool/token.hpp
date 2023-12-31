/**
 *  xEditor - Editor and tools for creating games
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_TOKEN_HPP
#define XENGINE_TOKEN_HPP

#include <string>

namespace xng {
    struct Token {
        enum TokenType {
            BRACKET_OPEN,
            BRACKET_CLOSE,
            SQUARE_BRACKET_OPEN,
            SQUARE_BRACKET_CLOSE,
            CURLY_BRACKET_OPEN,
            CURLY_BRACKET_CLOSE,
            ASTERISK,
            AMPERSAND,
            SEMICOLON,
            LESS_THAN,
            GREATER_THAN,
            EQUAL_SIGN,
            COMMA,
            IDENTIFIER,
            LITERAL_STRING,
            LITERAL_NUMERIC,
            COMMENT
        } type{};

        std::string value{};

        size_t lineNumber {};

        Token() = default;

        explicit Token(TokenType type, size_t lineNumber, std::string value)
                : type(type), lineNumber(lineNumber), value(std::move(value)) {}
    };
}
#endif //XENGINE_TOKEN_HPP

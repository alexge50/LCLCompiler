#ifndef LCLCOMPILER_AST_HPP
#define LCLCOMPILER_AST_HPP

#include <string_view>
#include <variant>

#include <gsl/span>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/filter.hpp>

#include <tokenizer.hpp>

namespace lcl
{
    struct ast;

    struct ast_base
    {
        const std::string_view            code;
        const gsl::span<const lcl::token> tokens;
    };

    struct global_scope_ast : public ast_base
    {
        const gsl::span<const lcl::ast> top_level_nodes;
    };

    //import Name: * | [name, name]
    struct import_statement_ast : public ast_base
    {
        auto module_name() const noexcept -> std::string_view 
        {
            assert(tokens.size() >= 2);
            return tokens[1].code;
        }

        auto is_full_import() const noexcept -> bool 
        {
            assert(tokens.size() >= 4);
            return tokens[3].type == lcl::token_type::star;
        }

        auto module_imports() const noexcept
        {
            return tokens | ranges::views::drop(3) 
                          | ranges::views::filter([](const lcl::token& it) { return it.type != lcl::token_type::comma && it.type != lcl::token_type::semicolon; })
                          | ranges::views::transform([](const lcl::token& it) { return it.code });
        }
    };

    struct ast : std::variant<global_scope_ast, import_statement_ast> 
    {

    };
}

#endif //LCLCOMPILER_AST_HPP
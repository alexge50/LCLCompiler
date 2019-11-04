#ifndef LCLCOMPILER_AST_HPP
#define LCLCOMPILER_AST_HPP

#include <string_view>
#include <variant>
#include <memory>

#include <gsl/span>
#include <gsl/gsl_assert>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>

#include <tokenizer.hpp>

namespace lcl
{
    struct ast;

    struct ast_base
    {
        std::string_view            code;
        gsl::span<const lcl::token> tokens;
    };

    struct global_scope_ast : public ast_base
    {
        std::vector<ast> top_level_nodes;
    };

    //import Name: * | [name, name]
    struct import_statement_ast : public ast_base
    {
        //Eg: import Name: *
        auto is_full_import() const noexcept -> bool 
        {
            Expects(tokens.size() >= 2);
            return tokens[3].type == lcl::token_type::star;
        }

        auto is_simple_import() const noexcept -> bool
        {
            Expects(tokens.size() >= 2);
            //The two possible cases are `import Name;` and `import Name`. This assumes the import_statement_ast is valid.
            return tokens.size() == 2 || tokens.size() == 3;
        }

        auto is_specific_import() const noexcept -> bool
        {
            Expects(tokens.size() >= 2);
            return !is_simple_import() && !is_full_import();
        }

        auto module_name() const noexcept -> std::string_view 
        {
            Expects(tokens.size() >= 2);
            return tokens[1].code;
        }

        auto module_imports() const noexcept
        {
            Expects(tokens.size() >= 2);
            Expects(is_specific_import());
            return tokens | ranges::views::drop(3) 
                          | ranges::views::filter([](const lcl::token& it) { return it.type != lcl::token_type::comma && it.type != lcl::token_type::semicolon; })
                          | ranges::views::transform([](const lcl::token& it) { return it.code; });
        }
    };

    struct ast : std::variant<global_scope_ast, import_statement_ast> 
    {

    };

    auto generate_ast(const std::string_view& code, const gsl::span<const lcl::token>& tokens) noexcept -> global_scope_ast;
}

#endif //LCLCOMPILER_AST_HPP
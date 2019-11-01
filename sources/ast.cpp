#include <string_view>
#include <variant>
#include <memory>

#include <fmt/core.h>
#include <fmt/format.h>
#include <gsl/span>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/filter.hpp>

#include <tokenizer.hpp>
#include <ast.hpp>

namespace lcl
{
    struct ast_gen_context
    {
        const std::string_view                      code;
        const gsl::span<const lcl::token>           tokens;
        gsl::span<const lcl::token>::const_iterator tokens_iterator;

        auto advance_tokens_iterator() noexcept -> void 
        {
            tokens_iterator = std::next(tokens_iterator);
        }

        auto reached_end_of_tokens() const noexcept -> bool 
        {
            return tokens_iterator == std::cend(tokens);
        }

        // Prints formatted error message.
        auto internal_report_error(const char* format, fmt::format_args args) 
        {
            fmt::print("AST Error: ");
            fmt::vprint(format, args);
        }

        template <typename... Args> auto check(const bool condition, const char* format, const Args&... args) 
        {
            if (condition) 
            {
                internal_report_error(format, fmt::make_format_args(args...));
            }
        }

        auto try_parse_import_statement() -> lcl::import_statement_ast
        {
            //If not import exit
            if (!tokens_iterator->is_import_keyword())
            {
                return;
            }

            //Now we know 100% that this is either an import or an error.
            const auto import_statement_tokens_begin = tokens_iterator;
            
            advance_tokens_iterator();
            check(!reached_end_of_tokens(), "Import statement must be followed by an identifier.");
            {
                const auto  iterator_at_module_name = tokens_iterator;
                const auto& module_name_token       = *iterator_at_module_name;
                
                check(module_name_token.is_identifier(), "Import statement must be followed by an identifier.");
            }
            
            advance_tokens_iterator();
            {
                const auto iterator_after_module_name = tokens_iterator;

                if (iterator_after_module_name == std::cend(tokens))
                {
                    
                }

                const auto& token_after_module_name = *iterator_after_module_name;
            }
        }
    };

    auto generate_ast(const std::string_view& code, const gsl::span<const lcl::token>& tokens) noexcept -> global_scope_ast
    {
        auto global_scope = lcl::global_scope_ast { code, tokens };
        auto ast_context  = lcl::ast_gen_context  { code, tokens, tokens.cbegin() };

        while (!ast_context.reached_end_of_tokens())
        {
            
        }

        return global_scope;
    }
}
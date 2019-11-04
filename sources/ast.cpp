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

        auto reached_end_of_tokens() const noexcept -> bool
        {
            return tokens_iterator == tokens.cend();
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
                return std::nullopt;
            }

            //Now we know 100% that this is either an import statement or an error.
            const auto  import_statement_tokens_begin = tokens_iterator;
            const auto  import_statement_code_begin   = import_statement_tokens_begin->code.cbegin();
            const auto& import_keyword_token          = *import_statement_tokens_begin;
            
            tokens_iterator++;

            check(!reached_end_of_tokens(), "Import statement must be followed by an identifier.");

            //Module name
            const auto& module_name_token = *tokens_iterator;
            
            check(module_name_token.is_identifier(), "Import statement must be followed by an identifier.");
                        
            tokens_iterator++;
            
            //Return `import ModuleName`
            if (reached_end_of_tokens())
            {
                const auto code   = string_view_slice(import_statement_code_begin, module_name_token.code.cend());
                const auto tokens = gsl::make_span(import_statement_tokens_begin, tokens_iterator);
                return lcl::import_statement_ast { code, tokens };
            }
            //Return `import ModuleName;
            else if (tokens_iterator->type == lcl::token_type::semicolon)
            {
                const auto iterator_at_semicolon = tokens_iterator;
                        
                tokens_iterator++;

                const auto code   = string_view_slice(import_statement_code_begin, iterator_at_semicolon.code.cend());
                const auto tokens = gsl::make_span(import_statement_tokens_begin, tokens_iterator); 
                return lcl::import_statement_ast { code, tokens };
            }
            //Look for `import ModuleName: *` or `import ModuleName: Name, ...`
            else if (iterator_after_module_name->type == lcl::token_type::colon)
            {
                tokens_iterator++;

                check(!reached_end_of_tokens(), "The `:` in an import statement must be followed by a list of import names or a wild card.");

                const auto is_imports_list     = tokens_iterator->is_identifier();
                const auto is_wild_card_import = tokens_iterator->type == lcl::token_type::star;

                check(is_impors_list || is_wild_card_import, "The `:` in an import statement must be followed by a list of import names or a wild card.");

                //`import ModuleName: *`
                if (is_wild_card_import)
                {
                    const auto wild_card_token = *tokens_iterator;

                    tokens_iterator++;

                    //Return `import Name: *`
                    if (reached_end_of_tokens())
                    {
                        const auto code   = string_view_slice(import_statement_code_begin, wild_card_token.code.cend());
                        const auto tokens = gsl::make_span(import_statement_tokens_begin, tokens_iterator); 
                        return lcl::import_statement_ast { code, tokens };
                    }
                    //Return `import Name: *;`
                    else if (tokens_iterator->type == lcl::token_type::semicolon)
                    {
                        const auto iterator_at_semicolon = tokens_iterator;
                        
                        tokens_iterator++;

                        const auto code   = string_view_slice(import_statement_code_begin, iterator_at_semicolon->code.cend());
                        const auto tokens = gsl::make_span(import_statement_tokens_begin, tokens_iterator); 
                        return lcl::import_statement_ast { code, tokens };
                    }
                    else if (tokens_iterator->line == wild_card_token)
                    {

                    }

                    UNREACHABLE_CODE;
                }
            }

            const auto& token_after_module_name = *iterator_after_module_name;
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
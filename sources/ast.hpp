#ifndef LCLCOMPILER_AST_HPP
#define LCLCOMPILER_AST_HPP

namespace lcl
{
    class ast_base
    {
        const std::string_view code;
    };

    class if_statement_ast : public ast_base
    {

    };
}

#endif //LCLCOMPILER_AST_HPP
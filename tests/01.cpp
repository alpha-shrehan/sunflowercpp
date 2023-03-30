#include <sunflower.hpp>

std::string read_file(std::string _path)
{
    std::ifstream fl;
    fl.open(_path);

    std::string content = "\n";
    std::string line = "";

    while (std::getline(fl, line))
        content += line + "\n";

    fl.close();

    return content;
}

using namespace sf;

ast_expression *test_puts(Module *m, int *state_flag)
{
    ast_expression *arg1 = m->names.get("arg1")->val;
    assert(arg1 != nullptr);

    switch (arg1->expr_type)
    {
    case AST_EXPR_BOOL:
        std::cout << ((ast_expr_bool *)arg1)->to_string() << std::endl;
        break;
    case AST_EXPR_INT:
        std::cout << ((ast_expr_int *)arg1)->to_string() << std::endl;
        break;
    case AST_EXPR_FLOAT:
        std::cout << ((ast_expr_float *)arg1)->to_string() << std::endl;
        break;
    case AST_EXPR_STRING:
        std::cout << ((ast_expr_string *)arg1)->to_string() << std::endl;
        break;

    default:
        std::cout << arg1->to_string();
        break;
    }

    return new ast_expr_bool(true);
}

void test1()
{
    std::string s = read_file("..\\..\\tests\\test.sf");
    // std::cout << s << '\n';

    std::vector<token *> tokens = tokenize(s);
    // std::cout << "Tokens: " << tokens.size() << std::endl;

    // _sf_token_print(tokens);
    std::vector<ast *> asts = parse_ast(tokens);

    // _sf_print_ast(asts);

    Module *mod = new Module(MOD_TYPE_FILE);
    mod->ast_nodes = asts;

    function *f = new function("puts", {"arg1"});
    f->isnative = true;
    f->native = test_puts;
    function *ff = add_function_to_fstack(f);
    mod->names.set("puts", add_to_stack(new stack_node(new ast_expr_function(ff), 0)));

    int sf = STATE_OK;
    // while (1)
    {
        ip_eval_tree(mod, &sf);
        // mod->names.~name_lookup();
        // mod->names.set("puts", add_to_stack(new stack_node(new ast_expr_function(ff), 0)));
        // sf = STATE_OK;

        // std::cout << getStack().size() << std::endl;
    }

    // stack_node *a = get_var_from_mod_r("a", mod);
    // stack_node *b = get_var_from_mod_r("b", mod);
    // std::cout << a << std::endl;
    // std::cout << b << std::endl;

    // for (auto &i: getStack())
    // {
    //     std::cout << i->ref_count << std::endl;
    // }

    // for (auto x : mod->names.val)
    // {
    //     std::cout << x.first << std::endl;
    //     _sf_print_ast(x.second->val);
    // }
    delete mod;
}

int main(int argc, char const *argv[])
{
    test1();

    return printf("Program ended.\n") && 0;
}
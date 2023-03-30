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

ast_expression *test_write(Module *m, int *state_flag)
{
    ast_expression *arg1 = m->names.get("argv")->val;
    ast_expression *vjoin = m->names.get("join")->val;
    ast_expression *vend = m->names.get("end")->val;
    assert(arg1 != nullptr);

    ast_expr_array *arr = (ast_expr_array *)arg1;
    std::string s = "";

    for (size_t i = 0; i < arr->value.size(); i++)
    {
        ast_expression *n = arr->value[i]->val;

        switch (n->expr_type)
        {
        case AST_EXPR_BOOL:
            s += ((ast_expr_bool *)n)->to_string();
            break;
        case AST_EXPR_INT:
            s += ((ast_expr_int *)n)->to_string();
            break;
        case AST_EXPR_FLOAT:
            s += ((ast_expr_float *)n)->to_string();
            break;
        case AST_EXPR_STRING:
            s += ((ast_expr_string *)n)->to_string();
            break;

        default:
            s += n->to_string();
            break;
        }

        if (i != arr->value.size() - 1)
            s += vjoin->to_string();
    }

    printf("%s", (s + vend->to_string()).c_str());
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

    function *f2 = new function("write", {"argv", "join", "end"});
    f2->def_vals = {
        nullptr,
        new ast_expr_string(" "),
        new ast_expr_string("\n")};
    
    f2->isnative = true;
    f2->va_args = true;
    f2->native = test_write;
    function *ff2 = add_function_to_fstack(f2);
    mod->names.set("write", add_to_stack(new stack_node(new ast_expr_function(ff2), 0)));

    int sf = STATE_OK;
    // while (1)
    {
        ip_eval_tree(mod, &sf);
        // mod->names.~name_lookup();
        // mod->names.set("puts", add_to_stack(new stack_node(new ast_expr_function(ff), 0)));
        // mod->names.set("write", add_to_stack(new stack_node(new ast_expr_function(ff2), 0)));
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
#include "sffunc.hpp"

namespace sf
{
    SF_API std::vector<function *> _sf_functions;

    SF_API function *add_function_to_fstack(function *f)
    {
        _sf_functions.push_back(f);
        return _sf_functions.back();
    }
} // namespace sf

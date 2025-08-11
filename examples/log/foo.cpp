#include "foo.hpp"

#include "log.hpp"

namespace foo {

void dump()
{
    xostd::log::log("foo");
}

} // namespace foo

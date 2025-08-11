#pragma once

#include "log.hpp"

namespace boo {

void dump();

template <typename T>
void dumpType()
{
    xostd::log::log("{}", __PRETTY_FUNCTION__);
}

} // namespace boo

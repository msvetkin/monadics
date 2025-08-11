#pragma once

#include "log.hpp"
#include <iostream>
#include <sstream>

struct MyLogger
{
    MyLogger()
    {
        std::cout << "MyLogger" << std::endl;
    }

    ~MyLogger()
    {
        std::cout << "~MyLogger" << std::endl;
    }

    void print(std::string) const;

    template <typename... Args>
    void log(Args&&... args) const
    {
        std::stringstream s;
        s << "MyLogger::log(";
        (s << ... << args);
        s << ")";

        std::cout << s.str() << std::endl;
        // print(s.str());
    }
};

// extern MyLogger::print;

namespace xostd::log {

// extern template MyLogger injectedLogger<>;
// template <>
// inline Logger auto injectedLogger<> = MyLogger{};

} // namespace xostd::log

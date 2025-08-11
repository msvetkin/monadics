#include "log.hpp"

#include <string_view>

#include "boo.hpp"
#include "foo.hpp"

// struct MyLogger
// {
    // MyLogger()
    // {
        // std::cout << "MyLogger" << std::endl;
    // }

    // ~MyLogger()
    // {
        // std::cout << "~MyLogger" << std::endl;
    // }

    // template <typename... Args>
    // void log(Args&&... args) const
    // {
        // std::cout << "MyLogger::log(";
        // (std::cout << ... << args);
        // std::cout << ")" << std::endl;
    // }
// };

// template <>
// inline xostd::log::Logger auto xostd::log::injectedLogger<> = MyLogger{};

// void MyLogger::print(std::string msg) const {
  // std::cout << msg << std::endl;
// }

int main()
{
    xostd::log::log("from main");

    // xostd::log::sink{
    //   [] (auto level, auto location, auto message) {
    //      
    //   }
    // }

    boo::dump();
    foo::dump();

    boo::dumpType<int>();
}

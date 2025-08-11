#pragma once

#include <iostream>
#include <type_traits>

namespace xostd::log {

template <typename T>
concept Logger = requires(T logger) {
    {
        logger.log(1)
    } -> std::same_as<void>;
};

struct ConsoleLogger
{
    ConsoleLogger()
    {
        std::cout << "ConsoleLogger" << std::endl;
    }

    ~ConsoleLogger()
    {
        std::cout << "~ConsoleLogger" << std::endl;
    }

    template <typename... Args>
    void log(Args&&... args)
    {
        std::cout << "ConsoleLogger::log(";
        (std::cout << ... << args);
        std::cout << ")" << std::endl;
    }
};

template <typename...>
inline Logger auto injectedLogger = ConsoleLogger{};

} // namespace xostd::log

#if __has_include(CONFIG_NAME)
  #include CONFIG_NAME
#endif

namespace xostd::log {

template <typename... Args, typename... DummyArgs>
    requires(sizeof...(DummyArgs) == 0)
auto log(Args&&... args) -> void
{
    Logger auto& logger = injectedLogger<DummyArgs...>;
    logger.log(std::forward<Args>(args)...);
}

} // namespace xostd::log

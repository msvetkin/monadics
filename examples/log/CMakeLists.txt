add_library(log INTERFACE)
target_sources(log
  INTERFACE
    log.hpp
)
target_compile_definitions(log
  INTERFACE
    CONFIG_NAME="log-config.hpp"
)

add_library(tst-log-libs-foo)
target_sources(tst-log-libs-foo
  PRIVATE
    foo.cpp
    foo.hpp
)
target_link_libraries(tst-log-libs-foo
  PUBLIC
    log
)

add_library(tst-log-libs-boo)
target_sources(tst-log-libs-boo
  PRIVATE
    boo.cpp
    boo.hpp
)
target_link_libraries(tst-log-libs-boo
  PUBLIC
    log
)

add_executable(tst-log-libs)
target_sources(tst-log-libs
  PRIVATE
    main.cpp
)
target_include_directories(tst-log-libs
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(tst-log-libs
  PRIVATE
    log
    tst-log-libs-foo
    tst-log-libs-boo
)

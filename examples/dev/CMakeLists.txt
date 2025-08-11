add_library(beman.monadics.examples.dev-core INTERFACE)

target_sources(beman.monadics.examples.dev-core
  INTERFACE
    concepts.cpp
)

add_executable(beman.monadics.examples.dev)
target_sources(beman.monadics.examples.dev
  PRIVATE
    main.cpp
)

include_guard(GLOBAL)

function(add_beman_library name)
  set(target "beman.${name}")
  set(alias "beman::${name}")

  add_library(${target})
  add_library(${alias} ALIAS ${target})

  target_sources(${target}
    PUBLIC
      FILE_SET HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
  )

  set_target_properties(${target}
    PROPERTIES
      VERIFY_INTERFACE_HEADER_SETS ON
      EXPORT_NAME ${name}
  )

  set(beman_lib_target ${target} PARENT_SCOPE)
endfunction()


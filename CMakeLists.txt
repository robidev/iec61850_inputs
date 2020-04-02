include_directories(
   .
)

set(server_example_inputs_SRCS
   config_file_parser_extensions.c
   dynamic_model_extensions.c
   inputs.c
   server_example_inputs.c
   static_model.c
   static_input.c
)

IF(WIN32)
set_source_files_properties(${server_example_inputs_SRCS}
                                       PROPERTIES LANGUAGE CXX)
ENDIF(WIN32)

add_executable(server_example_inputs
  ${server_example_inputs_SRCS}
)

target_link_libraries(server_example_inputs
    iec61850
)



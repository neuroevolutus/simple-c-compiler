add_executable(lexer_tests lexer_tests.cpp)
target_include_directories(lexer_tests
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_BINARY_DIR}/include>
)

target_link_libraries(lexer_tests PRIVATE compiler)
target_link_libraries(lexer_tests PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(lexer_tests)

add_executable(parser_tests parser_tests.cpp)
target_include_directories(parser_tests
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_BINARY_DIR}/include>
)

target_link_libraries(parser_tests PRIVATE compiler)
target_link_libraries(parser_tests PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(parser_tests)

add_executable(code_generator_tests code_generator_tests.cpp)
target_include_directories(code_generator_tests
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_BINARY_DIR}/include>
)

target_link_libraries(code_generator_tests PRIVATE compiler)
target_link_libraries(code_generator_tests PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(code_generator_tests)

add_executable(tacky_tests tacky_tests.cpp)
target_include_directories(tacky_tests
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_BINARY_DIR}/include>
)

target_link_libraries(tacky_tests PRIVATE compiler)
target_link_libraries(tacky_tests PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(tacky_tests)

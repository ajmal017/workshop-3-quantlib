cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
project(wells-ws3-quantlib LANGUAGES CXX)

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(QL REQUIRED quantlib)

find_package(aws-lambda-runtime REQUIRED)
add_executable(${PROJECT_NAME} "message.cpp" "options_configuration_header.cpp" "optionpricer.cpp" "dbhandler.cpp" "main.cpp")
add_definitions(-DWELLS_QUANTLIB_DEBUG -D_FILE_OFFSET_BITS=64)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/../third-party/)
target_link_libraries(${PROJECT_NAME} PUBLIC AWS::aws-lambda-runtime "/apps/local/x64/lib/libQuantLib.a" "/apps/local/x64/lib/libhiredis.a")

target_compile_features(${PROJECT_NAME} PRIVATE "cxx_std_11")
target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra")

# this line creates a target that packages your binary and zips it up
aws_lambda_package_target(${PROJECT_NAME})


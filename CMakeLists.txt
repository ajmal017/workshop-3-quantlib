cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)
project(wells-ws3-quantlib LANGUAGES CXX)

find_package(AWSSDK REQUIRED COMPONENTS core dynamodb)
find_package(aws-lambda-runtime REQUIRED)
add_executable(${PROJECT_NAME} "message.cpp" "main.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC AWS::aws-lambda-runtime ${AWSSDK_LINK_LIBRARIES})

target_compile_features(${PROJECT_NAME} PRIVATE "cxx_std_11")
target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra")

# this line creates a target that packages your binary and zips it up
aws_lambda_package_target(${PROJECT_NAME})


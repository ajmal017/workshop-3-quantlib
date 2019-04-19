cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)
list(APPEND CMAKE_MODULE_PATH "/apps/local/x64/lib/aws-lambda-runtime/cmake")
list(APPEND CMAKE_MODULE_PATH "/apps/local/x64/lib64/cmake/AWSSDK")
list(APPEND CMAKE_MODULE_PATH "/apps/local/x64/lib64/aws-c-common/cmake")
list(APPEND CMAKE_MODULE_PATH "/apps/local/x64/lib64/cmake")
list(APPEND CMAKE_MODULE_PATH "/apps/local/x64/lib64/cmake/aws-cpp-sdk-s3")
list(APPEND CMAKE_MODULE_PATH "/apps/local/x64/lib64/cmake/aws-cpp-sdk-core")
list(APPEND CMAKE_MODULE_PATH "/apps/local/x64/lib64/aws-c-event-stream/cmake")

project(wells-ws3-quantlib LANGUAGES CXX)

find_package(AWSSDK REQUIRED COMPONENTS s3 core)
find_package(aws-lambda-runtime)
add_executable(${PROJECT_NAME} "main.cpp" "message.cpp" "OptionParser.cpp" )

target_link_libraries(${PROJECT_NAME} PRIVATE AWS::aws-lambda-runtime ${AWSSDK_LINK_LIBRARIES})

target_compile_features(${PROJECT_NAME} PRIVATE "cxx_std_11")
target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra")

# this line creates a target that packages your binary and zips it up
aws_lambda_package_target(${PROJECT_NAME})


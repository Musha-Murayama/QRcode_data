cmake_minimum_required(VERSION 3.10)
project(QrCodeJpgSplitter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# OpenCVライブラリを探す
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# ライブラリのソースファイル
add_library(qrcodegen lib/qrcodegen/QrCode.cpp)
add_library(base64 lib/cpp-base64/base64.cpp)

# 実行ファイルの生成
add_executable(app src/main.cpp src/encoder.cpp src/decoder.cpp)

# 実行ファイルにライブラリとインクルードパスをリンク
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/qrcodegen
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/cpp-base64
)

target_link_libraries(app
    PRIVATE
    qrcodegen
    base64
    ${OpenCV_LIBS}
)
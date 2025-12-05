#cmakeのバージョン(コマンドプロンプトで[cmake -version]と入力すると確認できる) 
cmake_minimum_required(VERSION 3.29.3) 
 
#ディレクトリを指定 
project(opencv_test VERSION 0.1.0 LANGUAGES CXX) 
 
#パッケージを指定 
find_package(OpenCV REQUIRED) 
 
#include-Pathを指定する 
include_directories(${OpenCV_INCLUDE_DIRS}) 
 
#実行ファイル名を変数化 
set(EXECUTABLE_NAME "test") 

#ソースファイルを実行ファイルにする 
add_executable(${EXECUTABLE_NAME} main.cpp) 
 
#リンクする 
target_link_libraries(${EXECUTABLE_NAME} ${OpenCV_LIBS})
cmake_minimum_required(VERSION 3.28)
project(HUT_Compiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(HUT_Compiler main.cpp
        Frontend/Token.hpp
        Frontend/Scanner.hpp
        Frontend/Keywords.hpp
        Frontend/DFA.hpp
        Frontend/DFA.hpp
        Frontend/SYNTAX_ANALYZER.hpp
        Frontend/SYNTAX_ANALYZER.hpp
)

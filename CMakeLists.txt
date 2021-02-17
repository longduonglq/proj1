cmake_minimum_required(VERSION 3.17)
project(proj1)

set(CMAKE_CXX_STANDARD 17)

add_executable(proj1 main.cpp
                ui/Table.h ui/Table.cpp
                ui/Column.h ui/Column.cpp
                ui/configuration.h
                statisticsUI.h
                statistics.h
                baseConverter.h
                input.h
                common.h
                ui/OptionUI.h ui/Prerequisite.h ui/Parameter.h ui/inputType.h ui/UIExcept.h ui/MixedColumn.h)
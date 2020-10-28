# Copyright(C) 2020 Henry Bullingham
# This file is subject to the license terms in the LICENSE file
# found in the top - level directory of this distribution.


add_executable(cppspt_example cppspt_example.cpp)            
target_link_libraries(cppspt_example PUBLIC cppspt)
target_include_directories(cppspt_example PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET cppspt_example PROPERTY CXX_STANDARD 11)

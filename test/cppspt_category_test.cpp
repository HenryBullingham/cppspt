// Copyright(C) 2020 Henry Bullingham
// This file is subject to the license terms in the LICENSE file
// found in the top - level directory of this distribution.

#include "catch.hpp"

#include "cppspt/cppspt.hpp"
#include "cppspt/cppspt_category.hpp"

#include <string>

std::string append(cppspt::in<std::string> str)
{
    std::string temp = cppspt::resolve(std::move(str));
    temp += " appended!";
    return temp;
}

cppspt::uninit<std::string> repeat(int x)
{
    std::string str(x, 'X');
    return str;
}

cppspt::uninit<std::string> twice(cppspt::in<std::string> str)
{
    std::string temp = cppspt::resolve(std::move(str));
    return temp + temp;
}

TEST_CASE("Testing Category Usage", "[CPPSPT::Out]")
{
    //Test functorality

    std::string test_str = "Test. 1. 2. 3";

    cppspt::uninit<std::string> wrapped = cppspt::freturn<std::string>(test_str);

    REQUIRE(*wrapped == test_str);

    cppspt::uninit<std::string> appended = cppspt::fapply<std::string, std::string>(append, wrapped);

    REQUIRE(*appended == append(test_str));
    
    //Test monadic ability

    cppspt::uninit<std::string> xs = cppspt::mbind<std::string, std::string>(repeat(2), twice);

    REQUIRE(*xs == "XXXX");
}

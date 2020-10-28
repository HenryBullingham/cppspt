// Copyright(C) 2020 Henry Bullingham
// This file is subject to the license terms in the LICENSE file
// found in the top - level directory of this distribution.

#include "catch.hpp"

#include "cppspt/cppspt.hpp"

#include "cppspt_test.hpp"

//Test that we can call a function with an in parameter
void test_call_in(cppspt::in<NXString> str)
{

}

//Test that we can call a function with an in parameter and copy from it
void test_copy_from_in(cppspt::in<NXString> str)
{
    NXString temp = str; //Will always copy
}

//Test that we can capture from a moved 'in'
void test_resolve_from_in(cppspt::in<NXString> str)
{
    NXString temp = cppspt::resolve(str);
}

//Test that we can pass 'in' parameters with no copying
void test_pass_in_middle(cppspt::in<NXString> str)
{
    test_resolve_from_in(std::move(str));
}

//Test that we can pass 'in' parameters with no copying
void test_pass_in_outer(cppspt::in<NXString> str)
{
    test_pass_in_middle(std::move(str));
}

/*
void test_in_non_copyable(cppspt::in<ncString> nc)
{
    ncString nc2 = cppspt::resolve(nc);
}

void test_in_non_movable(cppspt::in<nmString> nm)
{
    nmString nm2 = cppspt::resolve(nm);
}
*/



TEST_CASE("Call with in", "[CPPSPT:IN]")
{
    //Testing that we can call 'in' functions with moves or refs
    REQUIRE(run_with_history([] {test_call_in(NXString()); }) == "ctor dtor ");
    REQUIRE(run_with_history([] {NXString str;  test_call_in(str); }) == "ctor dtor ");

    //Testing that we can copy from 'in' parameters with moves or refs
    REQUIRE(run_with_history([] {test_copy_from_in(NXString()); }) == "ctor copy-ctor dtor dtor ");
    REQUIRE(run_with_history([] {NXString str;  test_copy_from_in(str); }) == "ctor copy-ctor dtor dtor ");

    //Testing that we can capture moves from in parameters
    REQUIRE(run_with_history([] { test_resolve_from_in(NXString()); }) == "ctor move-ctor dtor dtor ");
    REQUIRE(run_with_history([] {NXString str;  test_resolve_from_in(str); }) == "ctor copy-ctor dtor dtor ");

    //Test that we can pass at multiple levels and incur no costs
    REQUIRE(run_with_history([] { test_pass_in_outer(NXString()); }) == "ctor move-ctor dtor dtor ");
    REQUIRE(run_with_history([] {NXString str;  test_pass_in_outer(str); }) == "ctor copy-ctor dtor dtor ");

}

TEST_CASE("Non Copyable/Movable")
{
    ncString nc(std::string(""));
    nmString nm(std::string(""));

    //test_in_non_copyable(nc);
    //test_in_non_movable(nm);
}
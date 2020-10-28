// Copyright(C) 2020 Henry Bullingham
// This file is subject to the license terms in the LICENSE file
// found in the top - level directory of this distribution.

#include "catch.hpp"
#include "cppspt/cppspt.hpp"
#include "cppspt_test.hpp"

void create_uninit()
{
    cppspt::uninit<NXString> str;
}

void construct_to_uninit(cppspt::in<NXString> str)
{
    cppspt::uninit<NXString> copy = std::move(str);
}

void assign_to_uninit(cppspt::in<NXString> str)
{
    cppspt::uninit<NXString> copy;
    copy = std::move(str);
}

void move_construct_uninit_then_assign(cppspt::in<NXString> strToAssign)
{
    cppspt::uninit<NXString> copy = NXString();
    copy = std::move(strToAssign);
}

//This test case checks that uninit constructs without constructing the underlying object
TEST_CASE("Testing Default Construction of Uninitialized", "[CPPSPT::Uninit]")
{
    REQUIRE(run_with_history([] {create_uninit(); }) == "");
    REQUIRE(run_with_constructions([] {create_uninit(); }).constructions == 0);
}

//This test case checks construction of the uninit
TEST_CASE("Testing Construction of Uninitialized", "[CPPSPT::Uninit]")
{
    auto construct_to_uninit_with_move = []
    {
        construct_to_uninit(NXString());
    };

    auto construct_to_uninit_with_copy = []
    {
        NXString str;
        construct_to_uninit(str);
    };

    REQUIRE(run_with_history(construct_to_uninit_with_move) == "ctor move-ctor dtor dtor ");
    REQUIRE(run_with_history(construct_to_uninit_with_copy) == "ctor copy-ctor dtor dtor ");

    construction_count count = run_with_constructions(construct_to_uninit_with_move);
    REQUIRE(count.constructions == 2);
    REQUIRE(count.destructions == 2);
    REQUIRE(count.copy_constructions == 0);
    REQUIRE(count.move_constructions == 1);
    REQUIRE(count.copy_assignments == 0);
    REQUIRE(count.move_assignments == 0);

    count = run_with_constructions(construct_to_uninit_with_copy);
    REQUIRE(count.constructions == 2);
    REQUIRE(count.destructions == 2);
    REQUIRE(count.copy_constructions == 1);
    REQUIRE(count.move_constructions == 0);
    REQUIRE(count.copy_assignments == 0);
    REQUIRE(count.move_assignments == 0);

}

//This test case checks that the first assignment to uninit calls the underlying constructor of the appropriate type (move or copy)
TEST_CASE("Testing Assignement of Uninitialized", "[CPPSPT::Uninit]")
{
    auto assign_to_uninit_with_move = []
    {
        assign_to_uninit(NXString());
    };

    auto assign_to_uninit_with_copy = []
    {
        NXString str;
        assign_to_uninit(str);
    };

    REQUIRE(run_with_history(assign_to_uninit_with_move) == "ctor move-ctor dtor dtor ");
    REQUIRE(run_with_history(assign_to_uninit_with_copy) == "ctor copy-ctor dtor dtor ");

    construction_count count = run_with_constructions(assign_to_uninit_with_move);
    REQUIRE(count.constructions == 2);
    REQUIRE(count.destructions == 2);
    REQUIRE(count.copy_constructions == 0);
    REQUIRE(count.move_constructions == 1);
    REQUIRE(count.copy_assignments == 0);
    REQUIRE(count.move_assignments == 0);

    count = run_with_constructions(assign_to_uninit_with_copy);
    REQUIRE(count.constructions == 2);
    REQUIRE(count.destructions == 2);
    REQUIRE(count.copy_constructions == 1);
    REQUIRE(count.move_constructions == 0);
    REQUIRE(count.copy_assignments == 0);
    REQUIRE(count.move_assignments == 0);
    
}

//This test case checks that the second assignment to an uninit will be a copy
TEST_CASE("Testing Construction-Assignement of Uninitialized", "[CPPSPT::Uninit]")
{
    auto assign_to_uninit_with_move = []
    {
        move_construct_uninit_then_assign(NXString());
    };

    auto assign_to_uninit_with_copy = []
    {
        NXString str;
        move_construct_uninit_then_assign(str);
    };

    REQUIRE(run_with_history(assign_to_uninit_with_move) == "ctor ctor move-ctor dtor move-assn dtor dtor ");
    REQUIRE(run_with_history(assign_to_uninit_with_copy) == "ctor ctor move-ctor dtor copy-assn dtor dtor ");

    construction_count count = run_with_constructions(assign_to_uninit_with_move);
    REQUIRE(count.constructions == 3);
    REQUIRE(count.destructions == 3);
    REQUIRE(count.copy_constructions == 0);
    REQUIRE(count.move_constructions == 1);
    REQUIRE(count.copy_assignments == 0);
    REQUIRE(count.move_assignments == 1);

    count = run_with_constructions(assign_to_uninit_with_copy);
    REQUIRE(count.constructions == 3);
    REQUIRE(count.destructions == 3);
    REQUIRE(count.copy_constructions == 0);
    REQUIRE(count.move_constructions == 1);
    REQUIRE(count.copy_assignments == 1);
    REQUIRE(count.move_assignments == 0);

}
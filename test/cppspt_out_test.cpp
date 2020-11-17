// Copyright(C) 2020 Henry Bullingham
// This file is subject to the license terms in the LICENSE file
// found in the top - level directory of this distribution.

#include "catch.hpp"

#include "cppspt/cppspt.hpp"

#include "cppspt_test.hpp"

void dont_write_anything(cppspt::out<NXString> str)
{
}

void write_const_ref(cppspt::out<NXString> str)
{
    NXString a_str;
    str = a_str;
}

void write_move(cppspt::out<NXString> str)
{
    str = NXString();
}

TEST_CASE("Testing Out", "[CPPSPT::Out]")
{
    //Testing that writing nothing does nothing
    REQUIRE(run_with_history([] {NXString str; dont_write_anything(str); }) == "ctor dtor ");
    REQUIRE(run_with_history([] {cppspt::uninit<NXString> str; dont_write_anything(str); }) == "");

    //Testing that writing a const ref to out works
    REQUIRE(run_with_history([] {NXString str; write_const_ref(str); }) == "ctor ctor copy-assn dtor dtor ");
    REQUIRE(run_with_history([] {cppspt::uninit<NXString> str; write_const_ref(str); }) == "ctor copy-ctor dtor dtor ");

    //Testing that writing a move to out works
    REQUIRE(run_with_history([] {NXString str; write_move(str); }) == "ctor ctor move-assn dtor dtor ");
    REQUIRE(run_with_history([] {cppspt::uninit<NXString> str; write_move(str); }) == "ctor move-ctor dtor dtor ");
}

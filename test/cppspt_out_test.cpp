// Copyright(C) 2020 Henry Bullingham
// This file is subject to the license terms in the LICENSE file
// found in the top - level directory of this distribution.

#include "catch.hpp"

#include "cppspt/cppspt.hpp"

#include "cppspt_test.hpp"





/*

Calling assn operators

*/

void TestAssignFromCopy(const NString& val)
{
    NString s;
    s = val;
}

void TestAssignFromMove(NString&& val)
{
    NString s;
    s = std::move(val);
}

void TestAssignFromIn(cppspt::in<NString> val)
{
    cppspt::uninit<NString> s = NString();
    s = std::move(val);
}

/*

Multiple constructions

*/

void TestConstructCopyCopy(const NString& a, const NString& b)
{
    NString c = a;
    NString d = b;
}

void TestConstructCopyMove(const NString& a, NString&& b)
{
    NString c = a;
    NString d = std::move(b);
}

void TestConstructMoveCopy(NString&& a, const NString& b)
{
    NString c = std::move(a);
    NString d = b;
}

void TestConstructMoveMove(NString&& a, NString&& b)
{
    NString c = std::move(a);
    NString d = std::move(b);
}

void TestConstructInIn(cppspt::in<NString> a, cppspt::in<NString> b)
{
    cppspt::uninit<NString> c = std::move(a);
    cppspt::uninit<NString> d = std::move(b);
}

/*

    Writing to out

*/

using XString = construction_counter<std::string>;

void TestWriteToOutMove(cppspt::out<XString> s)
{
    s = XString();
}

void TestWriteToOutCopy(cppspt::out<XString> s)
{
    XString c;
    s = c;
}

void TestWriteToOutMoveRef()
{
    XString s;
    TestWriteToOutMove(s);
}

void TestWriteToOutMoveUninit()
{
    cppspt::uninit<XString> s;
    TestWriteToOutMove(s);
}

void TestWriteToOutCopyRef()
{
    XString s;
    TestWriteToOutCopy(s);
}

void TestWriteToOutCopyUninit()
{
    cppspt::uninit<XString> s;
    TestWriteToOutCopy(s);
}

TEST_CASE("Testing Out", "[CPPSPT::Out]")
{

}

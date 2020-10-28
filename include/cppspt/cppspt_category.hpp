// Copyright(C) 2020 Henry Bullingham
// This file is subject to the license terms in the LICENSE file
// found in the top - level directory of this distribution.

#pragma once

#if !defined (CPPSPT_INCLUDE_CPPSPT_CATEGORY_HPP)
#define CPPSPT_INCLUDE_CPPSPT_CATEGORY_HPP

/*

    some of the cppspt types support operations of Functors and Monads, so 
    the necessary functional-style operations are included here

*/

#include "cppspt/cppspt.hpp"

#include <functional>

namespace cppspt
{
    template<template<class> typename Functor, typename T>
    Functor<T> freturn(in<T> val);

    template<template<class> typename Functor, typename Ret, typename Arg>
    Functor<Ret> fapply(std::function<Ret(in<Arg>)> func, in<Functor<Arg>> val);

    /*
    
        Here, we have functor & monad operations for the uninit type
    
    */

    template<typename T>
    uninit<T> freturn(in<T> val)
    {
        return val;
    }

    template<typename Ret, typename Arg>
    uninit<Ret> fapply(std::function<Ret(in<Arg>)> func, in<uninit<Arg>> arg)
    {
        if (arg->was_initialized())
        {
            return func(*((const uninit<Arg>&)arg));
        }
        else
        {
            return unitialized_t;
        }
    }

    template<typename T>
    uninit<T> mjoin(in<uninit<uninit<T>>> val)
    {
        if (!val->was_initialized())
        {
            return unitialized_t;
        }
        else
        {
            return *(*val);
        }
    }

    template<typename Ret, typename Arg>
    uninit<Ret> mbind(uninit<Arg> arg, std::function<uninit<Ret>(in<Arg>)> func)
    {
        return mjoin<Ret>(fapply<uninit<Ret>, Arg>(func, arg));
    }
}

#endif //CPPSPT_INCLUDE_CPPSPT_CATEGORY_HPP
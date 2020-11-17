// Copyright(C) 2020 Henry Bullingham
// This file is subject to the license terms in the LICENSE file
// found in the top - level directory of this distribution.

#pragma once

#if !defined (CPPSPT_INCLUDE_CPPSPT_HPP)
#define CPPSPT_INCLUDE_CPPSPT_HPP

#if !defined (CPPSPT_DISABLE_ASSERTS)
#include <cassert>

#define CPPSPT_ASSERT(x_) assert(x_)
#else

#define CPPSPT_ASSERT(x_) 

#endif

#include <type_traits>
#include <ostream>

namespace cppspt
{
    /*
    
        Standard parameter types: in, out, inout, move and forward

        in: only used for reading. Can capture const reference and move

        out: only used for writing. Captures a reference

        inout: used for reading and writing. Captures a reference

        move: used for move semantics. Captures an rvalue reference

        forward: used for forwarding semantics. Captures a forwarding reference
    
    */

    namespace detail
    {
        template<typename T>
        class in;

        template<typename T>
        class out;

        template<typename T>
        class uninit;

        //Singular valued unitialized representation
        struct unititialized_t {};
    }

    /// <summary>
    /// A read-only input parameter. Captures a const reference or a move
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<typename T>
    using in = detail::in<T>;

    /// <summary>
    /// A write-only output parameter. Captures a direct reference or a reference to an uninitialized T
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<typename T>
    using out = detail::out<T>;

    /// <summary>
    /// A read-and-write input/output parameter. Captures a reference
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<typename T>
    using inout = T&;

    /// <summary>
    /// A move-only input parameter. Captures an rvalue reference
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<typename T>
    using move = typename std::decay_t<T> &&;

    /// <summary>
    /// A forwarding reference (only for template types)
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<typename T>
    using forward = T&&;

    /// <summary>
    /// A wrapper for an uninitialized type. Allows deferred construction. (Defers to manual construction, assignment or default construction on first read)
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<typename T>
    using uninit = detail::uninit<T>;

    /// <summary>
    /// A representation of uninitialized memory, for returning uninits from functions
    /// </summary>
    static const detail::unititialized_t unitialized_t;

    /*
    
        For an in parameter captured by move, we may want to move construct it into a final destination
        To do so, use the following function:
    */

    /// <summary>
    /// Captures the value from an in parameter and converts it into the underlying type
    /// If the in parameter is a const ref, calls a copy constructor
    /// If the in parameter was moved, calls a move constructor
    /// The in parameter is invalidated after calling this function and can no longer be read from
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="param"></param>
    /// <returns></returns>
    template<typename T, typename std::enable_if< std::is_copy_constructible<T>::value&& std::is_move_constructible<T>::value, int>::type = 0 >
    T resolve(inout<in<T>> param);

    template<typename T, typename std::enable_if< !std::is_copy_constructible<T>::value && std::is_move_constructible<T>::value, int>::type = 1 >
    move<T> resolve(inout<in<T>> param);

    template<typename T, typename std::enable_if< std::is_copy_constructible<T>::value && !std::is_move_constructible<T>::value, int>::type = 2 >
    const T& resolve(inout<in<T>> param);

#define cppspt_declare_copy_constructors_from_in(_type)\
public:\
    _type(const _type& other) : _type(cppspt::in<_type>(other)){}\
    _type(_type&& other) : _type(cppspt::in<_type>(std::move(other))){}

    //Implementation details
    namespace detail
    {
        template<typename T>
        using const_ref = const T&;

        template<typename T>
        class in final
        {
        private:
            /*
                The purpose of this struct is twofold:
                a) it allows us to put an rvalue reference in a union
                b) it allows us to construct an rvalue reference
            */
            struct move_type_container
            {
                move<T> val;
                move_type_container(move<T> ref) : val(std::move(ref)) { }
            };

            union {
                move_type_container m_temp;     //Captures a move
                const T* m_ptr;                 //Captures a reference (as a ptr)
            };
            const bool m_was_moved;

            const T& to_ref() const
            {
                return (m_was_moved) ? m_temp.val : *m_ptr;
            }

        public:

            in(const_ref<T> val) :
                m_was_moved(false),
                m_ptr(&val)
            {
            }

            in(move<T> val) :
                m_was_moved(true),
                m_temp(std::move(val))
            {
            }

            in(const_ref<in<T>> other) :
                m_was_moved(false)
            {
                if (other.m_was_moved)
                {
                    m_ptr = &other.m_temp.val;
                }
                else
                {
                    m_ptr = other.m_ptr;
                }
            }

            in(move<in<T>> other) :
                m_was_moved(other.m_was_moved)
            {
                if (other.m_was_moved)
                {
                    new (&m_temp) move_type_container(std::move(other.m_temp.val));
                }
                else
                {
                    m_ptr = other.m_ptr;
                }
            }

            //No assignment operators
            in& operator=(const_ref<T>) = delete;

            //Always convertable to a const ref
            operator const T& () const
            {
                return to_ref();
            }

            const T& operator*() const
            {
                return to_ref();
            }

            const T* operator->() const
            {
                return &to_ref();
            }

            //Need these to write good constructors
            bool was_moved() const { return m_was_moved; }
            const T & unmoved_ref() { return *m_ptr; }
            move<T> move_out() { return std::move(m_temp.val); }
        };



        /*
        
        Unitialized type
        
        */
        template<typename T>
        class uninit final
        {
        private:
            union
            {
                T m_val;
            };
            bool m_was_initialized;

        public:
            ~uninit()
            {
                if (m_was_initialized)
                {
                    m_val.~T();
                }
            }

            uninit() : m_was_initialized(false) {}

            uninit(unititialized_t) : m_was_initialized(false) {}

            uninit(in<T> value) : m_was_initialized(true)
            {
                if (value.was_moved())
                {
                    new (&m_val) T(value.move_out());
                }
                else
                {
                    new (&m_val) T(value.unmoved_ref());
                }
            }

            //We have to manually specify the copy constructors & move assignment, because 
            // using 'in' causes ambiguous overload resolution.
            uninit(const_ref<uninit<T>> other) : 
                m_was_initialized(other.m_was_initialized)
            {
                if (other.m_was_initialized)
                {
                    new (&m_val) T(other.m_val);
                }
            }

            uninit(move<uninit<T>> other) :
                m_was_initialized(other.m_was_initialized)
            {
                if (other.m_was_initialized)
                {
                    new (&m_val) T(std::move(other.m_val));
                }
            }

            uninit& operator=(in<T> val)
            {
                if (m_was_initialized)
                {
                    if (val.was_moved())
                    {
                        m_val = val.move_out();
                    }
                    else
                    {
                        m_val = val.unmoved_ref();
                    }
                }
                else
                {
                    if (val.was_moved())
                    {
                        new (&m_val) T(val.move_out());
                    }
                    else
                    {
                        new (&m_val) T(val.unmoved_ref());
                    }
                    m_was_initialized = true;
                }
                return *this;
            }

            uninit& operator=(const_ref<uninit<T>> other)
            {
                if (&other == this)
                {
                    return;
                }

                if (m_was_initialized)
                {
                    if (other.m_was_initialized)
                    {
                        m_val = other.m_val;
                    }
                    else
                    {
                        m_val.~T();
                    }
                }
                else
                {
                    if (other.m_was_initialized)
                    {
                        new (&m_val) T(other.m_val);
                    }
                    else
                    {
                        //Do nothing (both uninitialized)
                    }
                }

                m_was_initialized = other.m_was_initialized;
            }

            uninit& operator=(move<uninit<T>> other)
            {
                if (&other == this)
                {
                    return;
                }

                if (m_was_initialized)
                {
                    if (other.m_was_initialized)
                    {
                        m_val = std::move(other.m_val);
                    }
                    else
                    {
                        m_val.~T();
                    }
                }
                else
                {
                    if (other.m_was_initialized)
                    {
                        new (&m_val) T(std::move(other.m_val));
                    }
                    else
                    {
                        //Do nothing (both uninitialized)
                    }
                }

                m_was_initialized = other.m_was_initialized;
            }

            void init()
            {
                if (!m_was_initialized)
                {
                    new (&m_val)T();
                    m_was_initialized = true;
                }
            }

            template<typename ... Args>
            void init(Args ... args)
            {
                if (!m_was_initialized)
                {
                    new (&m_val)T(std::forward<Args...>(args...));
                    m_was_initialized = true;
                }
            }

            operator const T& () const
            {
                CPPSPT_ASSERT(m_was_initialized && "Attempting to read from uninit value!");

                return m_val;
            }

            operator T& ()
            {
                CPPSPT_ASSERT(m_was_initialized && "Attempting to read from uninit value!");

                return m_val;
            }

            T& operator*()
            {
                return static_cast<T&>(*this);
            }

            T* operator->()
            {
                return &static_cast<T&>(*this);
            }

            const T& operator*() const
            {
                return static_cast<const T&>(*this);
            }

            const T* operator->() const
            {
                return &static_cast<const T&>(*this);
            }

            bool was_initialized() const
            {
                return m_was_initialized;
            }

            public:
                friend std::ostream& operator<< (std::ostream& out, uninit<T> val)
                {
                    if (val.was_initialized())
                    {
                        return out << val.m_val;
                    }
                    return out << "[Uninitialized]";
                }
        };


        /*
        
            Output only-type

        */

        template<typename T>
        class out final
        {
        private:
            union {
                T* m_direct;
                uninit<T>* m_uninit;
            };

            const bool m_is_direct;
            bool m_was_written = false;

        public:
            ~out(){}

            out(T& direct) : m_is_direct(true), m_direct(&direct) {}
            out(uninit<T>& uninitialized) : m_is_direct(false), m_uninit(&uninitialized) {}

            //Unfortunately, we can't specify an 'in' copy constructor; 
            //We still have to manually specify the const ref & move constructors individually
            //If we don't, we get compilation errors for ambiguous overloads
            out(const out<T>& other) : m_is_direct(other.m_is_direct)
            {
                if (other.m_is_direct)
                {
                    m_direct = other.m_direct;
                }
                else
                {
                    m_uninit = other.m_uninit;
                }
            }

            out(out<T>&& other) : m_is_direct(other.m_is_direct)
            {
                if (other.m_is_direct)
                {
                    m_direct = other.m_direct;
                }
                else
                {
                    m_uninit = other.m_uninit;
                }
            }

            out<T>& operator=(in<T> val)
            {
                if (m_is_direct)
                {

                    if (val.was_moved())
                    {
                        *m_direct = val.move_out();
                    }
                    else
                    {
                        *m_direct = val.unmoved_ref();
                    }
                }
                else
                {
                    *m_uninit = std::move(val);
                }

                m_was_written = true;

                return *this;
            }

            operator T& ()
            {
                CPPSPT_ASSERT(m_was_written && "CPPSPT: reading from unwritten x!");
                if (m_is_direct)
                {
                    return *m_direct;
                }
                else
                {
                    return *m_uninit;
                }
            }

            T& operator*()
            {
                return static_cast<T&>(*this);
            }

            T* operator->()
            {
                return &static_cast<T&>(*this);
            }

        private:
            //In order to prevent ambiguous overloads with operator=
            //(Due to there being conversions from references to BOTH in AND out)
            //We are forced to do this:
            //Which only works when they are marked const.
            void operator=(out<T>&&) const = delete;
            void operator=(const out<T>&) const = delete;
        };
    }


    template<typename T, typename std::enable_if< std::is_copy_constructible<T>::value && std::is_move_constructible<T>::value , int>::type >
    T resolve(inout<in<T>> param)
    {
        if (param.was_moved())
        {
            return param.move_out();
        }
        else
        {
            return param.unmoved_ref();
        }
    }

    template<typename T, typename std::enable_if< std::is_copy_constructible<T>::value && !std::is_move_constructible<T>::value, int>::type >
    const T& resolve(inout<in<T>> param)
    {
        CPPSPT_ASSERT(!param.was_moved() && "Moved a solely copy constructible type!");

        return param.unmoved_ref();
    }

    template<typename T, typename std::enable_if< !std::is_copy_constructible<T>::value && std::is_move_constructible<T>::value, int>::type >
    move<T>&& resolve(inout<in<T>> param)
    {
        CPPSPT_ASSERT(param.was_moved() && "Copying a solely move constructible type!");

        return param.move_out();
    }

}

#endif
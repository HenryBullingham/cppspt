// Copyright(C) 2020 Henry Bullingham
// This file is subject to the license terms in the LICENSE file
// found in the top - level directory of this distribution.

#include <functional>
#include <string>

/*

Classes for testing purposes


*/

/*

    Construction / Assignment / Destruction counting classe

*/

struct construction_count
{
    int constructions = 0;
    int destructions = 0;
    int copy_constructions = 0;
    int move_constructions = 0;
    int copy_assignments = 0;
    int move_assignments = 0;
};

static construction_count s_construction_count;

template<typename T>
class construction_counter
{
private:
    T m_val;

public:
    construction_counter()
    {
        s_construction_count.constructions++;
    }
    construction_counter(const T& val) : m_val(val)
    {
        s_construction_count.constructions++;
    }

    construction_counter(const construction_counter<T>& other) : m_val(other.m_val)
    {
        s_construction_count.constructions++;
        s_construction_count.copy_constructions++;
    }

    construction_counter(construction_counter<T>&& other) : m_val(std::move(other.m_val))
    {
        s_construction_count.constructions++;
        s_construction_count.move_constructions++;
    }

    ~construction_counter()
    {
        s_construction_count.destructions++;
    }

    construction_counter<T>& operator= (const construction_counter<T>& other)
    {
        s_construction_count.copy_assignments++;
        m_val = other.m_val;
        return *this;
    }

    construction_counter<T>& operator= (construction_counter<T>&& other)
    {
        s_construction_count.move_assignments++;
        m_val = std::move(other.m_val);
        return *this;
    }

    bool operator==(const construction_counter<T>& other)
    {
        return m_val == other.m_val;
    }

    T& get()
    {
        return m_val;
    }
};

inline construction_count run_with_constructions(std::function<void()> func)
{
    s_construction_count = construction_count();
    func();
    return s_construction_count;
}

/*

    'Noisy' Operation sequence tracking class

*/

static std::string s_history;

template<typename T>
class noisy
{
private:
    T m_val;

public:
    noisy()
    {
        s_history += "ctor ";
    }
    noisy(const T& val) : m_val(val)
    {
        s_history += "ctor ";
    }

    noisy(const noisy<T>& other) : m_val(other.m_val)
    {
        s_history += "copy-ctor ";
    }

    noisy(noisy<T>&& other) : m_val(std::move(other.m_val))
    {
        s_history += "move-ctor ";
    }

    ~noisy()
    {
        s_history += "dtor ";
    }

    noisy& operator= (const noisy<T>& other)
    {
        s_history += "copy-assn ";
        m_val = other.m_val;
        return *this;
    }

    noisy& operator= (noisy<T>&& other)
    {
        s_history += "move-assn ";
        m_val = std::move(other.m_val);
        return *this;
    }

    bool operator==(const noisy<T>& other)
    {
        return m_val == other.m_val;
    }

    T& get()
    {
        return m_val;
    }
};

inline std::string run_with_history(std::function<void()> func)
{
    s_history = {};
    func();
    return s_history;
}

/* Some interesting types to deal with */

template<typename T>
class non_movable
{
private:
    T m_val;

public:
    non_movable(const T& val) : m_val(val) {}

    non_movable(const non_movable<T>& other) : m_val(other.m_val) {}
    non_movable<T> operator= (const non_movable<T>& other) { m_val = other.m_val; }

    non_movable(non_movable<T>&&) = delete;
    non_movable& operator=(non_movable<T>&&) = delete;

    T& val(){ return m_val; }
};

template<typename T>
class non_copyable
{
private:
    T m_val;

public:
    non_copyable(const T& val) : m_val(val) {}

    non_copyable(non_movable<T>&& other) : m_val(std::move(other.m_val)) {}
    non_copyable<T> operator= (non_copyable<T>&& other) { m_val = std::move(other.m_val); }

    non_copyable(const non_copyable<T>&) = delete;
    non_copyable& operator=(const non_copyable<T>&) = delete;

    T& val() { return m_val; }
};


/*

    Wrappers around std::string
*/

using NString = noisy<std::string>;
using XString = construction_counter<std::string>;
using NXString = noisy<XString>;

using ncString = non_copyable<NString>;
using nmString = non_movable<NString>;
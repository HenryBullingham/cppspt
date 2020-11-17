// Copyright(C) 2020 Henry Bullingham
// This file is subject to the license terms in the LICENSE file
// found in the top - level directory of this distribution.

#include "cppspt/cppspt.hpp"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

template<typename K, typename V>
class simple_map
{
public:
    using pair_type = std::pair<cppspt::uninit<K>, cppspt::uninit<V>>;

    using iterator = typename std::vector<pair_type>::iterator;

private:
    std::vector<pair_type> m_pairs;

public:

    void put(cppspt::in<K> key, cppspt::in<V> value)
    {
        auto it = find(key);
        if (it != end())
        {
            it->second = std::move(value);
            return;
        }

        m_pairs.emplace_back();
        m_pairs.back().first = std::move(key);
        m_pairs.back().second = std::move(value);
    }

    iterator begin()
    {
        return m_pairs.begin();
    }

    iterator end()
    {
        return m_pairs.end();
    }

    iterator find(cppspt::in<K> key)
    {
        return std::find_if(begin(), end(), [&](const pair_type& p) { return *p.first == *key; });
    }

    void erase(iterator it)
    {
        m_pairs.erase(it);
    }
};

using dictionary = simple_map<std::string, std::string>;

int main()
{
    dictionary dict;

    //Will construct dict entries from moved strings
    dict.put(std::string("hello"), std::string("world"));

    //Will construct dict entries from copies
    std::string foo = "foo";
    dict.put(foo, foo);

    //No method overload for move vs copy
    std::cout << (dict.find(std::string("whoop")) == dict.end()) << std::endl;
    std::cout << (dict.find(foo) == dict.end()) << std::endl;

    for (auto& pair : dict)
    {
        std::cout << pair.first << " -> " << pair.second << std::endl;
    }

    return 0;
}
/*
 * Copyright (c) 2019, Li-Wei Cheng. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the SimplePrint Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace cmd_disp
{

namespace internal
{

template<typename Iterator, typename T>
struct CommandGetter
{
    static T get(Iterator it) = delete;
};

template<typename Iterator, typename T>
struct CommandGetter<Iterator, T&>
{
    static T get(Iterator it)
    {
        return CommandGetter<Iterator, T>::get(it);
    }
};

template<typename Iterator, typename T>
struct CommandGetter<Iterator, T const>
{
    static T get(Iterator it)
    {
        return CommandGetter<Iterator, T>::get(it);
    }
};

template<typename Iterator>
struct CommandGetter<Iterator, char>
{
    static char get(Iterator it)
    {
        return it->at(0);
    }
};

template<typename Iterator>
struct CommandGetter<Iterator, std::string>
{
    static std::string get(Iterator it)
    {
        return *it;
    }
};

template<typename Iterator>
struct CommandGetter<Iterator, int>
{
    static int get(Iterator it)
    {
        return std::stoi(*it);
    }
};

template<typename Iterator>
struct CommandGetter<Iterator, double>
{
    static double get(Iterator it)
    {
        return std::stod(*it);
    }
};

#define DISP_DEFINE_CMD(name, ...) \
    void name(_DISP_PARAM_APPEND_TAG(__VA_ARGS__)) \
    { \
        name(_DISP_ARGS(__VA_ARGS__)); \
    }

#define _DISP_PARAM_APPEND_TAG(...) _DISP_PARAMS(cmd_disp::Tag, ##__VA_ARGS__)

#define _DISP_COUNT(...) _DISP_COUNT_I(0, ##__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define _DISP_COUNT_I(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N

#define _DISP_GLUE(A, B) _DISP_GLUE_I(A, B)
#define _DISP_GLUE_I(A, B) A##B

#define _DISP_PARAMS(...) _DISP_GLUE(_DISP_PARAMS_, _DISP_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define _DISP_PARAMS_0()
#define _DISP_PARAMS_1(t) t _p1
#define _DISP_PARAMS_2(t, ...) t _p2, _DISP_PARAMS_1(__VA_ARGS__)
#define _DISP_PARAMS_3(t, ...) t _p3, _DISP_PARAMS_2(__VA_ARGS__)
#define _DISP_PARAMS_4(t, ...) t _p4, _DISP_PARAMS_3(__VA_ARGS__)
#define _DISP_PARAMS_5(t, ...) t _p5, _DISP_PARAMS_4(__VA_ARGS__)
#define _DISP_PARAMS_6(t, ...) t _p6, _DISP_PARAMS_5(__VA_ARGS__)
#define _DISP_PARAMS_7(t, ...) t _p7, _DISP_PARAMS_6(__VA_ARGS__)
#define _DISP_PARAMS_8(t, ...) t _p8, _DISP_PARAMS_7(__VA_ARGS__)
#define _DISP_PARAMS_9(t, ...) t _p9, _DISP_PARAMS_8(__VA_ARGS__)

#define _DISP_ARGS(...) _DISP_GLUE(_DISP_ARGS_, _DISP_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define _DISP_ARGS_0()
#define _DISP_ARGS_1(t) _p1
#define _DISP_ARGS_2(t, ...) _p2, _DISP_ARGS_1(__VA_ARGS__)
#define _DISP_ARGS_3(t, ...) _p3, _DISP_ARGS_2(__VA_ARGS__)
#define _DISP_ARGS_4(t, ...) _p4, _DISP_ARGS_3(__VA_ARGS__)
#define _DISP_ARGS_5(t, ...) _p5, _DISP_ARGS_4(__VA_ARGS__)
#define _DISP_ARGS_6(t, ...) _p6, _DISP_ARGS_5(__VA_ARGS__)
#define _DISP_ARGS_7(t, ...) _p7, _DISP_ARGS_6(__VA_ARGS__)
#define _DISP_ARGS_8(t, ...) _p8, _DISP_ARGS_7(__VA_ARGS__)
#define _DISP_ARGS_9(t, ...) _p9, _DISP_ARGS_8(__VA_ARGS__)

class DefaultTokenizer
{
public:
    using Iterator = std::vector<std::string>::iterator;

public:
    DefaultTokenizer(const std::string& cmd)
    {
        std::stringstream ss(cmd);
        std::string s;

        while (std::getline(ss, s, ' '))
        {
            tokens.push_back(s);
        }
    }

    const std::string name() const
    {
        return tokens.at(0);
    }

    Iterator begin()
    {
        return tokens.begin() + 1;
    }

    Iterator end()
    {
        return tokens.begin();
    }

private:
    std::vector<std::string> tokens;
};

}

struct Tag {};

template<
    typename Tokenizer = internal::DefaultTokenizer,
    template<class, class> class Getter = internal::CommandGetter
>
class Dispatcher
{
    using Iterator = typename Tokenizer::Iterator;
public:
    template<typename... Args>
    bool add(const std::string& name, void(*f)(Tag, Args...))
    {
        if (disp.find(name) != disp.end())
        {
            return false;
        }

        disp.insert(std::make_pair(
            name,
            [f](Iterator it)
            {
                std::tuple<Tag, Args...> args {
                    Tag(), Getter<Iterator, Args>::get(it++)...
                };
                std::apply(f, args);
            }
        ));
        return true;
    }

    template<typename Class, typename... Args>
    bool add(const std::string& name, Class* c, void(Class::*f)(Tag, Args...))
    {
        if (disp.find(name) != disp.end())
        {
            return false;
        }

        disp.insert(std::make_pair(
            name,
            [c, f](Iterator it)
            {
                std::tuple<Class*, Tag, Args...> args {
                    c, Tag(), Getter<Iterator, Args>::get(it++)...
                };
                std::apply(f, args);
            }
        ));
        return true;
    }

    bool dispatch(const std::string& cmd)
    {
        Tokenizer tokenizer(cmd);

        auto it = disp.find(tokenizer.name());
        if (it == disp.end())
        {
            return false;
        }

        FuncWrapper f = it->second;
        f(tokenizer.begin());
        return false;
    }

private:
    using FuncWrapper = std::function<void(Iterator)>;
    std::map<std::string, FuncWrapper> disp;
};

}

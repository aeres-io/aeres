/*
  MIT License
  Copyright (c) 2018 aeres.io
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#pragma once

#include <memory>
#include <string>
#include <map>
#include <functional>
#include "AeresObject.h"

namespace aeres
{
  class AeresTypes
  {
  public:

    using NewFunc = std::function<std::shared_ptr<AeresObject>(const char *, const char *, const char *, const char *)>;

  public:

    template<typename T>
    static int Register(const char * type)
    {
      return Register(type,
        [](const char * _base, const char * _name, const char * _path, const char * _type)
        {
          return std::shared_ptr<AeresObject>(new T(_base, _name, _path, _type));
        }
      );

    }

    static int Register(const char * type, NewFunc callback);

    static std::shared_ptr<AeresObject> Create(const char * base, const char * name, const char * path, const char * type);

  private:

    static std::map<std::string, NewFunc> * registrations;
  };

  #define __AERES_TYPE_STRINGIFY(x) #x
  #define AERES_TYPE_STRINGIFY(x) __AERES_TYPE_STRINGIFY(x)

  #define AERES_TYPE_REG(name, type) \
    static int __aeres_type##name = aeres::AeresTypes::Register<type>(AERES_TYPE_STRINGIFY(name));
}

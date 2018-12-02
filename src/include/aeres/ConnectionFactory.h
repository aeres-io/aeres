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
#include <aeres/Connection.h>
#include <aeres/Socket.h>

namespace aeres
{
  class ConnectionFactory
  {
  public:

    ConnectionFactory();

    virtual ~ConnectionFactory();

    ConnectionFactory(const ConnectionFactory & rhs) = delete;

    ConnectionFactory & operator=(const ConnectionFactory & rhs) = delete;

    std::shared_ptr<Connection> CreateSocketConnection(Socket fd, bool autoStart = true) const;

    std::shared_ptr<Connection> CreateSocketConnection(struct sockaddr * addr, socklen_t len, bool autoStart = true) const;

  private:

    static void SetSocketOptions(Socket fd);
  };
}

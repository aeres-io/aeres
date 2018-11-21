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

#include "AeresObject.h"

#include "AeresSession.h"

namespace aeres
{

  AeresObject::AeresObject(const char * base, const char * name, const char * path, const char * type) :
    base(base),
    path(path),
    name(name),
    type(type)
  {
  }

  bool AeresObject::Call(const char * method, CArgs & args, Callback callback)
  { 
    std::shared_ptr<AeresSession> session = AeresSession::GetSession(base);
    if (session.get() == NULL) { return false; }
    return session->Call(this->path, method, args, callback);
  }

  bool AeresObject::Call(const char * method, CArgs & args, RawCallback callback)
  { 
    std::shared_ptr<AeresSession> session = AeresSession::GetSession(base);
    if (session.get() == NULL) { return false; }
    return session->Call(this->path, method, args, callback);
  }

  bool AeresObject::Call(const char * method, CArgs & args, const void * body, size_t len, Callback callback)
  { 
    std::shared_ptr<AeresSession> session = AeresSession::GetSession(base);
    if (session.get() == NULL) { return false; }
    return session->Call(this->path, method, args, body, len, callback);
  }


  uint32_t AeresObject::GetTimeout() const
  {
    std::shared_ptr<AeresSession> session = AeresSession::GetSession(base);
    if (session.get() == NULL) { return 1; }
    return session->GetTimeout();
  }
}

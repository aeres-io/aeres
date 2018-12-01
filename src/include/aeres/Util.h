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

#define return_if(__cond) if (__cond) return;
#define return_val_if(__val, __cond) if (__cond) return __val;
#define return_false_if(__cond) return_val_if(false, __cond)
#define return_true_if(__cond) return_val_if(true, __cond)
#define return_null_if(__cond) return_val_if(nullptr, __cond)

#define unused(expr) (void)(expr)
#define unused_result(expr) do { auto _v = (expr); (void)_v; } while(false)

#ifdef DEBUG
#define fail_if(__cond) assert(!(__cond))
#define fail_val_if(__val, __cond) fail_if(__cond)
#define fail_false_if(__val, __cond) fail_if(__cond)
#define fail_null_if(__val, __cond) fail_if(__cond)
#else
#define fail_if(__cond) if (__cond) { Log::Critical("ASSERT FAILURE: %s(%d)", __FILE__, __LINE__); return; }
#define fail_val_if(__val, __cond) if (__cond) { Log::Critical("ASSERT FAILURE: %s(%d)", __FILE__, __LINE__); return __val; }
#define fail_false_if(__cond) fail_val_if(false, __cond)
#define fail_null_if(__cond) fail_val_if(nullptr, __cond)
#endif
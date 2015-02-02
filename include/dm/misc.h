/*
 * Copyright 2014-2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_MISC_H_HEADER_GUARD
#define DM_MISC_H_HEADER_GUARD

#include <stdint.h>
#include <math.h> // logf()
#include <stdio.h> // FILE
#include <float.h> // FLT_EPSILON
#include <malloc.h> //alloca

#include "common/common.h" // DM_INLINE()
#include "check.h" // DM_CHECK()

#include "../../3rdparty/bx/os.h" //bx::pwd()

namespace dm
{
    // Compile time.
    //-----

    /// Usage: Log<2, 512>::Value
    template <uint8_t Base, uint32_t N>
    struct Log
    {
        enum { Value = 1 + Log<Base, N/Base>::Value };
    };
    template <uint8_t Base> struct Log<Base, 1> { enum { Value = 0 }; };
    template <uint8_t Base> struct Log<Base, 0> { enum { Value = 0 }; };

    // Value.
    //-----

    #define DM_MIN(_a, _b) (_a)<(_b)?(_a):(_b)
    #define DM_MAX(_a, _b) (_a)>(_b)?(_a):(_b)
    #define DM_CLAMP(_val, _min, _max) DM_MIN(DM_MAX(_val, _min), _max)

    template <typename Ty/*arithmetic type*/>
    DM_INLINE Ty min(Ty _a, Ty _b)
    {
        return  _a < _b ? _a : _b;
    }

    template <typename Ty/*arithmetic type*/>
    DM_INLINE Ty max(Ty _a, Ty _b)
    {
        return  _a > _b ? _a : _b;
    }

    /// Assumes _min < _max.
    template <typename Ty/*arithmetic type*/>
    DM_INLINE Ty clamp(Ty _val, Ty _min, Ty _max)
    {
        return ( _val > _max ? _max
               : _val < _min ? _min
               : _val
               );
    }

    template <typename Ty/*arithmetic type*/>
    DM_INLINE void swap(Ty _a, Ty _b)
    {
        Ty c = _a;
        _a = _b;
        _b = c;
    }

    DM_INLINE void swap(uint8_t* __restrict _a, uint8_t* __restrict _b, uint32_t _size)
    {
        uint8_t* c = (uint8_t*)alloca(_size);
        memcpy( c, _a, _size);
        memcpy(_a, _b, _size);
        memcpy(_b,  c, _size);
    }

    DM_INLINE void swap(uint8_t* __restrict _a, uint8_t* __restrict _b, uint8_t* __restrict _tmp, uint32_t _size)
    {
        uint8_t* c = _tmp;
        memcpy( c, _a, _size);
        memcpy(_a, _b, _size);
        memcpy(_b,  c, _size);
    }

    DM_INLINE void toggle(bool& _flag)
    {
        _flag = !_flag;
    }

    // Integer.
    //-----

    #define DM_KILOBYTES(_KB)     (_KB##ul<<10ul)
    #define DM_MEGABYTES(_MB)     (_MB##ul<<20ul)
    #define DM_GIGABYTES(_GB)     (_GB##ul<<30ul)
    #define DM_GIGABYTES_ULL(_GB) (_GB##ull<<30ull)

    /// Example: for input 12780 (12.492KB) returns 12.
    DM_INLINE uint64_t asKBInt(uint64_t _dataSize)
    {
        return _dataSize>>10;
    }

    /// Example: for input 12780 (12.492KB) returns 492.
    DM_INLINE uint64_t asKBDec(uint64_t _dataSize)
    {
        const uint64_t kb = asKBInt(_dataSize);
        return _dataSize-(kb<<10);
    }

    /// Example: for input 13450000 (12.846MB) returns 12.
    DM_INLINE uint64_t asMBInt(uint64_t _dataSize)
    {
        return _dataSize>>20;
    }

    /// Example: for input 13450000 (12.846MB) returns 826.
    DM_INLINE uint64_t asMBDec(uint64_t _dataSize)
    {
        const uint64_t mb = asMBInt(_dataSize);
        return (_dataSize-(mb<<20))>>10;
    }

    DM_INLINE bool inside(int32_t _px, int32_t _py, int32_t _minx, int32_t _miny, int32_t _width, int32_t _height)
    {
        return (_px > _minx)
            && (_py > _miny)
            && (_px < (_minx+_width))
            && (_py < (_miny+_height));
    }

    // Align.
    //-----

    DM_INLINE uint32_t align(uint32_t _val, uint32_t _alignPwrTwo)
    {
        const uint32_t mask = _alignPwrTwo-UINT32_C(1);
        return (_val+mask)&(~mask);
    }

    DM_INLINE uint32_t alignf(float _val, uint32_t _align)
    {
        return uint32_t(_val/float(int32_t(_align)))*_align;
    }

    DM_INLINE void* alignPtrNext(void* _ptr, size_t _alignPwrTwo)
    {
        union { void* p; size_t addr; } ptr;
        ptr.p = _ptr;
        const size_t mask = _alignPwrTwo-1;
        ptr.addr = (ptr.addr+mask)&(~mask);
        return ptr.p;
    }

    DM_INLINE void* alignPtrPrev(void* _ptr, size_t _alignPwrTwo)
    {
        union { void* p; size_t addr; } ptr;
        ptr.p = _ptr;
        const size_t mask = _alignPwrTwo-1;
        ptr.addr = ptr.addr&(~mask);
        return ptr.p;
    }

    DM_INLINE size_t alignSizeNext(size_t _size, size_t _alignPwrTwo)
    {
        const size_t mask = (_alignPwrTwo-1);
        return (_size+mask)&(~mask);
    }

    DM_INLINE size_t alignSizePrev(size_t _size, size_t _alignPwrTwo)
    {
        const size_t mask = (_alignPwrTwo-1);
        return _size&(~mask);
    }

    DM_INLINE void alignPtrAndSize(void*& _alignedPtr, size_t& _alignedSize, void* _ptr, size_t _size, size_t _alignPwrTwo)
    {
        _alignedPtr = alignPtrNext(_ptr, _alignPwrTwo);
        const size_t diff = (uint8_t*)_alignedPtr - (uint8_t*)_ptr;
        const size_t totalSize = _size + diff;
        _alignedSize = alignSizePrev(totalSize, _alignPwrTwo);
    }

    // Float.
    //-----

    DM_INLINE float utof(uint32_t _u32)
    {
        DM_CHECK((_u32&0x80000000) == 0, "Unsigned value is too big!");
        return float(int32_t(_u32));
    }

    DM_INLINE bool equals(float _a, float _b, float _epsilon = FLT_EPSILON)
    {
        return fabsf(_a - _b) < _epsilon;
    }

    /// Example: for input 5.34f returns 5.0f.
    DM_INLINE float integerPart(float _val)
    {
        return float(int32_t(_val));
    }

    /// Example: for input 5.34f returns 0.34f.
    DM_INLINE float decimalPart(float _val)
    {
        return _val - integerPart(_val);
    }

    DM_INLINE float signf(float _val)
    {
        return (_val > 0.0f) ? 1.0f : -1.0f;
    }

    DM_INLINE float squaref(float _x)
    {
        return _x*_x;
    }

    #if defined(_MSC_VER)
    DM_INLINE float fminf(float _a, float _b)
    {
        return _a < _b ? _a : _b;
    }

    DM_INLINE float fmaxf(float _a, float _b)
    {
        return _a > _b ? _a : _b;
    }

    DM_INLINE float log2f(float _val)
    {
        static const float invLog2 = 1.4426950408889634f;
        return logf(_val)*invLog2;
    }
    #endif //defined(_MSC_VER)

    // String.
    //----

    DM_INLINE void strscpy(char* _dst, const char* _src, size_t _dstSize)
    {
        _dst[0] = '\0';
        if (NULL != _src)
        {
            strncat(_dst, _src, _dstSize-1);
        }
    }
    #define strscpya(_charArray, _src) strscpy(_charArray, _src, sizeof(_charArray))

    DM_INLINE void strtolower(char* _out, char* _in)
    {
        while (*_in)
        {
            *_out++ = (char)tolower(*_in++);
        }
        *_out = '\0';
    }

    DM_INLINE void strtoupper(char* _out, char* _in)
    {
        while (*_in)
        {
            *_out++ = (char)toupper(*_in++);
        }
        *_out = '\0';
    }

    DM_INLINE void strtolower(char* _str)
    {
        for( ; *_str; ++_str)
        {
            *_str = (char)tolower(*_str);
        }
    }

    DM_INLINE void strtoupper(char* _str)
    {
        for( ; *_str; ++_str)
        {
            *_str = (char)toupper(*_str);
        }
    }

    /// Notice: do NOT use return value of this function for memory deallocation!
    DM_INLINE char* trim(char* _str)
    {
        // Advance ptr until a non-space character is reached.
        while (isspace(*_str))
        {
            ++_str;
        }

        // If end is reached (_str contained all spaces), return.
        if ('\0' == *_str)
        {
            return _str;
        }

        // Point to the last non-whitespace character.
        char* end = _str + strlen(_str)-1;
        while (isspace(*end--) && end > _str)
        {
            // Empty body.
        }

        // Add string terminator.
        end[1] = '\0';

        return _str;
    }

    // File system.
    //-----

    #define DM_PATH_LEN 4096

    DM_INLINE void realpath(char _abs[DM_PATH_LEN], const char _rel[DM_PATH_LEN])
    {
        // TODO:
        //realpath(_state.m_directory, path); //Linux
        //_fullpath(path, _state.m_directory, DM_PATH_LEN); //Windows
        char currentDir[DM_PATH_LEN];
        bx::pwd(currentDir, DM_PATH_LEN);

        bx::chdir(_rel);
        bx::pwd(_abs, DM_PATH_LEN);

        bx::chdir(currentDir);
    }

    /// Gets file name without extension from file path. Examples:
    ///     /tmp/foo.c -> foo
    ///     C:\\tmp\\foo.c -> foo
    DM_INLINE bool basename(char* _out, size_t _outSize, const char* _filePath)
    {
       const char *begin;
       const char *end;

       const char *ptr;
       begin = NULL != (ptr = strrchr(_filePath, '\\')) ? ++ptr
             : NULL != (ptr = strrchr(_filePath, '/' )) ? ++ptr
             : _filePath
             ;

       end = NULL != (ptr = strrchr(_filePath, '.')) ? ptr : strrchr(_filePath, '\0');

       if (NULL != begin && NULL != end)
       {
           const size_t size = dm::min(size_t(end-begin)+1, _outSize);
           dm::strscpy(_out, begin, size);
           return true;
       }

       return false;
    }

    DM_INLINE long int fsize(FILE* _file)
    {
        long int pos = ftell(_file);
        fseek(_file, 0L, SEEK_END);
        long int size = ftell(_file);
        fseek(_file, pos, SEEK_SET);
        return size;
    }

    // Inherit.
    //-----

    struct NoCopyNoAssign
    {
    protected:
        NoCopyNoAssign() { }
        ~NoCopyNoAssign() { }
    private:
        NoCopyNoAssign(const NoCopyNoAssign&);
        const NoCopyNoAssign& operator=(const NoCopyNoAssign&);
    };


    // Scope.
    //-----

    struct ScopeFclose : NoCopyNoAssign
    {
        ScopeFclose(FILE* _fp)
            : m_fp(_fp)
        {
        }

        ~ScopeFclose()
        {
            if (m_fp)
            {
                fclose(m_fp);
            }
        }

    private:
        FILE* m_fp;
    };

    template <typename Ty>
    struct ScopeUnload : NoCopyNoAssign
    {
        ScopeUnload(Ty& _ptr)
            : m_ptr(_ptr)
        {
        }

        ~ScopeUnload()
        {
            m_ptr.unload();
        }

    private:
        Ty& m_ptr;
    };

} // namespace dm

#endif // DM_MISC_H_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */

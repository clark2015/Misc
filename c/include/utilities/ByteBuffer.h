#ifndef BYTEBUFFER_H_INCLUDE_20111018__
#define BYTEBUFFER_H_INCLUDE_20111018__

#include <vector>
#include <string>
#include <map>
#include <list>
#include "array.h"
#include "log.h"
#include "../mmorpg/typedef.h"
#include "allocator.h"

class ByteBufferException
{
    public:
        ByteBufferException(bool _add, size_t _pos, size_t _esize, size_t _size)
            : add(_add), pos(_pos), esize(_esize), size(_size)
        {
            PrintPosError();
        }

        void PrintPosError() const
        {
            sLog.outError("Attempted to %s in ByteBuffer (pos: %u size: %u) value with size: %u",
                (add ? "put" : "get"), pos, size, esize);
        }
    private:
        bool add;
        size_t pos;
        size_t esize;
        size_t size;
};

template<class T>
struct Unused
{
    Unused() {}
};

class ByteBuffer : public Allocator< ByteBuffer, 10 >
{
private:
    // copy constructor
    ByteBuffer(const ByteBuffer &buf): _rpos(buf._rpos), _wpos(buf._wpos), _storage(buf._storage) { }
    //ByteBuffer(const ByteBuffer& source);
    ByteBuffer &operator=(const ByteBuffer &rhs);
public:
    const static size_t DEFAULT_SIZE = 0x1000;

    ByteBuffer(): _rpos(0), _wpos(0)
    {
        _storage.reserve(DEFAULT_SIZE);
    }

    ByteBuffer(size_t res): _rpos(0), _wpos(0)
    {
        _storage.reserve(res);
    }


    void clear()
    {
        _storage.clear();
        _rpos = _wpos = 0;
    }

    template <typename T> void put(size_t pos,T value)
    {
        put(pos,(uint8 *)&value,sizeof(value));
    }
    
    ByteBuffer &operator<<(bool value)
    {
        append<uint8>(value);
        return *this;
    }

    ByteBuffer &operator<<(uint8 value)
    {
        append<uint8>(value);
        return *this;
    }

    ByteBuffer &operator<<(uint16 value)
    {
        append<uint16>(value);
        return *this;
    }

    ByteBuffer &operator<<(uint32 value)
    {
        append<uint32>(value);
        return *this;
    }

    ByteBuffer &operator<<(uint64 value)
    {
        append<uint64>(value);
        return *this;
    }

    // signed as in 2e complement
    ByteBuffer &operator<<(int8 value)
    {
        append<int8>(value);
        return *this;
    }

    ByteBuffer &operator<<(int16 value)
    {
        append<int16>(value);
        return *this;
    }

    ByteBuffer &operator<<(int32 value)
    {
        append<int32>(value);
        return *this;
    }
        
    ByteBuffer &operator<<(long value)
    {
        append<long>(value);
        return *this;
    }

    ByteBuffer &operator<<(int64 value)
    {
        append<int64>(value);
        return *this;
    }

    ByteBuffer &operator<<(float value)
    {
        append<float>(value);
        return *this;
    }

    ByteBuffer &operator<<(double value)
    {
        append<double>(value);
        return *this;
    }

    ByteBuffer &operator<<(const std::string &value)
    {
        append((uint8 const *)value.c_str(), value.length());
        append((uint8)0);
        return *this;
    }

    ByteBuffer &operator<<(const char *str)
    {
        append((uint8 const *)str, str ? strlen(str) : 0);
        append((uint8)0);
        return *this;
    }

    ByteBuffer &operator>>(bool &value)
    {
        value = read<char>() > 0 ? true : false;
        return *this;
    }

    ByteBuffer &operator>>(uint8 &value)
    {
        value = read<uint8>();
        return *this;
    }

    ByteBuffer &operator>>(uint16 &value)
    {
        value = read<uint16>();
        return *this;
    }

    ByteBuffer &operator>>(uint32 &value)
    {
        value = read<uint32>();
        return *this;
    }

    ByteBuffer &operator>>(uint64 &value)
    {
        value = read<uint64>();
        return *this;
    }

    //signed as in 2e complement
    ByteBuffer &operator>>(int8 &value)
    {
        value = read<int8>();
        return *this;
    }

    ByteBuffer &operator>>(int16 &value)
    {
        value = read<int16>();
        return *this;
    }

    ByteBuffer &operator>>(int32 &value)
    {
        value = read<int32>();
        return *this;
    }
        
    ByteBuffer &operator>>(long value)
    {
        value = read<long>();
        return *this;
    }

    ByteBuffer &operator>>(int64 &value)
    {
        value = read<int64>();
        return *this;
    }

    ByteBuffer &operator>>(float &value)
    {
        value = read<float>();
        return *this;
    }

    ByteBuffer &operator>>(double &value)
    {
        value = read<double>();
        return *this;
    }

    ByteBuffer &operator>>(std::string& value)
    {
        value.clear();
        while (rpos() < size())                         // prevent crash at wrong string format in packet
        {
            char c = read<char>();
            if (c == 0)
                break;
            value += c;
        }
        return *this;
    }

    template<class T>
    ByteBuffer &operator>>(Unused<T> const&)
    {
        read_skip<T>();
        return *this;
    }


    uint8 operator[](size_t pos) const
    {
        return read<uint8>(pos);
    }

    size_t rpos() const { return _rpos; }
    size_t bytesAvailable() const { return size() - rpos(); }

    size_t rpos(size_t rpos_)
    {
        _rpos = rpos_;
        return _rpos;
    }

    size_t wpos() const { return _wpos; }

    size_t wpos(size_t wpos_)
    {
        _wpos = wpos_;
        return _wpos;
    }

    template<typename T>
    void read_skip() { read_skip(sizeof(T)); }

    void read_skip(size_t skip)
    {
        if(_rpos + skip > size())
        {
            sLog.outError("ByteBuffer::read_skip out of range");
            return;
            throw ByteBufferException(false, _rpos, skip, size());
        }
        _rpos += skip;
    }

    template <typename T> T read()
    {
        T r = read<T>(_rpos);
        _rpos += sizeof(T);
        return r;
    }

    template <typename T> T read(size_t pos) const
    {
        if(pos + sizeof(T) > size())
        {
            return T();
            throw ByteBufferException(false, pos, sizeof(T), size());
        }
        T val = *((T const*)&_storage[pos]);
        return val;
    }

    void read(uint8 *dest, size_t len)
    {
        if(_rpos  + len > size())
        {
            //避免死循环
            _rpos = size();
            sLog.outError("ByteBuffer::read out of range");
            return;
            throw ByteBufferException(false, _rpos, len, size());
        }
        memcpy(dest, &_storage[_rpos], len);
        _rpos += len;
    }

    const uint8 *contents() const { return &_storage[0]; }

    size_t size() const { return _storage.size(); }
    bool empty() const { return _storage.empty(); }

    void resize(size_t newsize)
    {
        if(newsize > _storage.max_size())
        {
            sLog.outError("ByteBuffer::reseize newseize > max_size()");
            return;
        }

        size_t oldsize = size();

        _storage.resize(newsize);
        _rpos = 0;
        _wpos = size();

        if(size() > oldsize)
        {
            memset(_storage.c_array() + oldsize, 0, size() - oldsize);
        }
    }

    void reserve(size_t ressize)
    {
        if (ressize > size())
            _storage.reserve(ressize);
    }

    void append(const std::string& str)
    {
        append((uint8 const*)str.c_str(), str.size() + 1);
    }

    void append(const char *src, size_t cnt)
    {
        return append((const uint8 *)src, cnt);
    }

    template<class T> void append(const T *src, size_t cnt)
    {
        return append((const uint8 *)src, cnt * sizeof(T));
    }

    void append(const uint8 *src, size_t cnt)
    {
        if (!cnt)
            return;

        if (_storage.max_size() < _wpos + cnt)
        {
            sLog.outError("ByteBuffer::append appendsize > max_size()");
            return;
        }

        _storage.resize(_wpos + cnt);
        memcpy(&_storage[_wpos], src, cnt);
        _wpos += cnt;
    }

    void append(ByteBuffer* buffer, uint32 offset, uint32 size)
    {
        if(buffer == NULL) return;
        
        if(offset >= buffer->wpos()) return;

        if(size == 0 || size > buffer->wpos() - offset)
            size = buffer->wpos() - offset;

        append(buffer->contents() + offset, size);
    }

    void put(size_t pos, const uint8 *src, size_t cnt)
    {
        if(pos + cnt > size())
        {
            sLog.outError("ByteBuffer::put pos out of range");
            return;
            throw ByteBufferException(true, pos, cnt, size());
        }
        memcpy(&_storage[pos], src, cnt);
    }
        
    void print_storage() const
    {
        if(!sLog.IsOutDebug())                          // optimize disabled debug output
            return;

        sLog.outDebug("STORAGE_SIZE: %lu", (unsigned long)size() );
        for(uint32 i = 0; i < size(); ++i)
            sLog.outDebugInLine("%u ", read<uint8>(i) );
        sLog.outDebug(" ");
    }

    void textlike() const
    {
        //if(!sLog.IsOutDebug())                          // optimize disabled debug output
        //    return;

        sLog.outString("STORAGE_SIZE: %lu", (unsigned long)size() );
        for(uint32 i = 0; i < size(); ++i)
            sLog.outStringInLine("%c", read<uint8>(i) );
        sLog.outDebug(" ");
    }

    void hexlike() const
    {
        if(!sLog.IsOutDebug())                          // optimize disabled debug output
            return;

        char buffer[1024];
        buffer[0] = 0;

#ifndef _WINDOWS
#define _snprintf_s snprintf
#endif
        _snprintf_s(buffer, sizeof(buffer), "STORAGE_SIZE: %lu\n", (unsigned long)size() );

        for(uint32 i = 0; i < size(); ++i)
        {
            if (i != 0 && i%16 == 0)
            {
                _snprintf_s(buffer, sizeof(buffer), "%s\n", buffer);
            }

            _snprintf_s(buffer, sizeof(buffer), "%s%02X ", buffer, read<uint8>(i));
        }

#undef snprintf
        sLog.outDebug(buffer);
        //sLog.outStringInLine("\n");
    }

    //供lua调用的
    const char* ReadString(uint32 count)
    {
        uint32 new_rpos = rpos() + count;

        const uint8* ret = _storage.c_array() + rpos();
        while (rpos() < size())                         // prevent crash at wrong string format in packet
        {
            char c = read<char>();
            if (c == 0)
                break;
        }

        if(new_rpos <= size() && new_rpos > rpos())
        {
            rpos(new_rpos);
        }

        return (const char*)ret;
    }
    // signed as in 2e complement
    void AppendInt8(int8 value)
    {
        append<int8>(value);
    }

    void AppendInt16(int16 value)
    {
        append<int16>(value);
    }

    void AppendInt32(int32 value)
    {
        append<int32>(value);
    }
    
    void AppendUInt8(int8 value)
    {
        append<uint8>(value);
    }

    void AppendUInt16(int16 value)
    {
        append<uint16>(value);
    }

    void AppendUInt32(int32 value)
    {
        append<uint32>(value);
    }

    void AppendInt64(int64 value)
    {
        append<int64>(value);
    }

    void AppendGuid(double value)
    {
        append<int64>((int64)value);
    }
    
    double ReadGuid( )
    {
        return (double) read<int64>( );
    }
    double ReadGuidAt(size_t pos) const
    {
        if(pos + sizeof(double) > size())
        {
            return double();
            throw ByteBufferException(false, pos, sizeof(double), size());
        }
        double val = (double)(*((int64 const*)&_storage[pos]));
        return val;
    }

    void AppendString(const char *str)
    {
        append((uint8 const *)str, str ? strlen(str) : 0);
        append((uint8)0);
    }
public:
    // limited for internal use because can "append" any unexpected type (like pointer and etc) with hard detection problem
    template <typename T> void append(T value)
    {
        append((uint8 *)&value, sizeof(value));
    }

protected:
    size_t _rpos, _wpos;
    mmo_server::darray<uint8, 0x1000> _storage;
};

template <typename T>
inline ByteBuffer &operator<<(ByteBuffer &b, std::vector<T> const& v)
{
    b << (uint32)v.size();
    for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); ++i)
    {
        b << *i;
    }
    return b;
}

template <typename T>
inline ByteBuffer &operator>>(ByteBuffer &b, std::vector<T> &v)
{
    uint32 vsize;
    b >> vsize;
    v.clear();
    while(vsize--)
    {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}

template <typename T>
inline ByteBuffer &operator<<(ByteBuffer &b, std::list<T> const& v)
{
    b << (uint32)v.size();
    for (typename std::list<T>::iterator i = v.begin(); i != v.end(); ++i)
    {
        b << *i;
    }
    return b;
}

template <typename T>
inline ByteBuffer &operator>>(ByteBuffer &b, std::list<T> &v)
{
    uint32 vsize;
    b >> vsize;
    v.clear();
    while(vsize--)
    {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}

template <typename K, typename V>
inline ByteBuffer &operator<<(ByteBuffer &b, std::map<K, V> &m)
{
    b << (uint32)m.size();
    for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); ++i)
    {
        b << i->first << i->second;
    }
    return b;
}

template <typename K, typename V>
inline ByteBuffer &operator>>(ByteBuffer &b, std::map<K, V> &m)
{
    uint32 msize;
    b >> msize;
    m.clear();
    while(msize--)
    {
        K k;
        V v;
        b >> k >> v;
        m.insert(make_pair(k, v));
    }
    return b;
}

template<>
inline void ByteBuffer::read_skip<char*>()
{
    std::string temp;
    *this >> temp;
}

template<>
inline void ByteBuffer::read_skip<char const*>()
{
    read_skip<char*>();
}

template<>
inline void ByteBuffer::read_skip<std::string>()
{
    read_skip<char*>();
}

#endif //BYTEBUFFER_H_INCLUDE_20111018__

#ifndef TYPE_DEF_H_INCLUDE_20111024__
#define TYPE_DEF_H_INCLUDE_20111024__

#ifndef _MSC_VER
	typedef long long __int64;
//	typedef unsigned long long unsigned __int64;
#endif

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
//typedef unsigned __int64    uint64;
typedef unsigned long long  uint64;
typedef uint16              Position_t;

typedef char                int8;
typedef short               int16;
typedef int                 int32;
//typedef __int64             int64;
typedef long long           int64;
typedef long long           time64_t;

typedef uint64              ObjectGuid;

struct IDBStorge;
struct ITrace;

class GameObject4Script;
class Player4Script;
class Creature4Script;
typedef uint16              Position_t;

struct Location
{
    Location()
        :x(0.0f),
        y(0.0f)
    {}
    Location(float x_, float y_)
        :x(x_),
        y(y_)
    {}

    float x;
    float y;

    bool operator == ( Location& l )
    {
        return x == l.x && y == l.y;
    }
};

struct Size_t
{
    long cx;
    long cy;
};

#ifndef M_PI
#  define M_PI          3.1415926535897932//定义和flash中的一致3846
#endif

#ifndef M_PI_F
#  define M_PI_F        float(M_PI)
#endif

enum TimeConstants
{
    TIME_MINUTE = 60,
    TIME_HOUR   = TIME_MINUTE*60,
    TIME_DAY    = TIME_HOUR*24,
    TIME_WEEK   = TIME_DAY*7,
    TIME_MONTH  = TIME_DAY*30,
    TIME_YEAR   = TIME_MONTH*12,
    IN_MILLISECONDS = 1000
};

#ifndef _WINDOWS
#define strncpy_s strncpy
#define _snprintf snprintf
#define _snprintf_s snprintf
#endif


#endif //TYPE_DEF_H_INCLUDE_20111024__

#ifndef LOG_H_INCLUDE_20111025__
#define LOG_H_INCLUDE_20111025__

#include <stdio.h>
#include <stdarg.h>
#include <fstream>
#include <time.h>
#include <ctime>
#include "../ILogger.h"
#include "Singleton.h"


class Log : public CSingleton< Log >
{
public:
    enum
    {
        LOG_LEVEL_OUT_STRING,
        LOG_LEVEL_OUT_ERROR,
        LOG_LEVEL_OUT_BASIC,
        LOG_LEVEL_OUT_DETAIL,
        LOG_LEVEL_OUT_DEBUG,
    };
private:
    friend class CSingleton< Log >;
    Log()
        : m_logLevel(LOG_LEVEL_OUT_STRING)
    {}
    ~Log()
    {
    }
private:
    uint32          m_logLevel;
public:
    static void LuaLogString(const char* message)
    {
        if(message == NULL)
            return;

        Log::Instance().Log2Stdout(message);
    }
    
    static void LuaLogError(const char* message)
    {
        if(message == NULL)
            return;

        if(Log::Instance().IsOutError())
            Log::Instance().Log2Stdout(message);
    }

    static void LuaLogDebug(const char* message)
    {
        if(message == NULL)
            return;
        
        if(Log::Instance().IsOutDebug())
            Log::Instance().Log2Stdout(message);
    }
    
    static void LuaSetLogLevel(uint32 level) { Log::Instance().SetLogLevel(level);}
    void SetLogLevel(uint32 level) {m_logLevel = level;}
    bool IsOutDebug() const { return m_logLevel >= LOG_LEVEL_OUT_DEBUG; }
    bool IsOutBasic() const { return m_logLevel >= LOG_LEVEL_OUT_BASIC; }
    bool IsOutDetail() const { return m_logLevel >= LOG_LEVEL_OUT_DETAIL; }
    bool IsOutError() const { return m_logLevel >= LOG_LEVEL_OUT_ERROR; }

    void Log2StdoutInLine(const char* contents)
    {
        g_Loger->DirectLog(contents, true);
        //g_Loger->ntLogLn(contents);
    }
    void Log2Stdout(const char* contents)
    {
        g_Loger->DirectLog(contents, true);
        //Trace(contents);
    }

    void outString( const char * fmt, ... ){
        char dest[1024 * 16];
        va_list argptr;
        va_start(argptr, fmt);
#ifdef _WINDOWS
        vsprintf_s(dest, fmt, argptr);
#else
        vsprintf(dest, fmt, argptr);
#endif
        va_end(argptr);
        Log2Stdout(dest);
    }
    void outError( const char * fmt, ... ){
        if(!IsOutError())
            return;

        char dest[1024 * 16];
        va_list argptr;
        va_start(argptr, fmt);
#ifdef _WINDOWS
        vsprintf_s(dest, fmt, argptr);
#else
        vsprintf(dest, fmt, argptr);
#endif
        va_end(argptr);
        Log2Stdout(dest);
    }
    void outBasic( const char * fmt, ... ){
        if(!IsOutBasic())
            return;

        char dest[1024 * 16];
        va_list argptr;
        va_start(argptr, fmt);
#ifdef _WINDOWS
        vsprintf_s(dest, fmt, argptr);
#else
        vsprintf(dest, fmt, argptr);
#endif
        va_end(argptr);
        Log2Stdout(dest);
    }
    void outDetail( const char * fmt, ... ){
        if(!IsOutDetail())
            return;

        char dest[1024 * 16];
        va_list argptr;
        va_start(argptr, fmt);
#ifdef _WINDOWS
        vsprintf_s(dest, fmt, argptr);
#else
        vsprintf(dest, fmt, argptr);
#endif
        va_end(argptr);
        Log2Stdout(dest);
    }
    void outStringInLine( const char * fmt, ... ){
        char dest[1024 * 16];
        va_list argptr;
        va_start(argptr, fmt);
#ifdef _WINDOWS
        vsprintf_s(dest, fmt, argptr);
#else
        vsprintf(dest, fmt, argptr);
#endif
        va_end(argptr);
        Log2StdoutInLine(dest);
    }
    void outDebugInLine( const char * fmt, ... ){
        if(!IsOutDebug())
            return;

        char dest[1024 * 16];
        va_list argptr;
        va_start(argptr, fmt);
#ifdef _WINDOWS
        vsprintf_s(dest, fmt, argptr);
#else
        vsprintf(dest, fmt, argptr);
#endif
        va_end(argptr);
        Log2StdoutInLine(dest);
    }
    void outDebug( const char * fmt, ... ){
        if(!IsOutDebug())
            return;

        char dest[1024 * 16];
        va_list argptr;
        va_start(argptr, fmt);
#ifdef _WINDOWS
        vsprintf_s(dest, fmt, argptr);
#else
        vsprintf(dest, fmt, argptr);
#endif
        va_end(argptr);
        Log2Stdout(dest);
    }
};


#define sLog Log::Instance()
#endif //LOG_H_INCLUDE_20111025__

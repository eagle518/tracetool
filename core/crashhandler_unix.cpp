#include "crashhandler.h"

#include <cassert>

#include <signal.h>

using namespace std;

static const int g_caughtSignals[] = {
    SIGSEGV
    , SIGABRT
    , SIGILL
    , SIGFPE
#if defined(SIGBUS)
    , SIGBUS
#endif
};

static TRACELIB_NAMESPACE_IDENT(CrashHandler) g_handler;

extern "C"
{

static void crashHandler( int sig )
{
    for ( unsigned int i = 0; i < sizeof( g_caughtSignals ) / sizeof( g_caughtSignals[0] ); ++i ) {
        signal( g_caughtSignals[i], SIG_DFL );
    }
    (*g_handler)();
}

}

TRACELIB_NAMESPACE_BEGIN

void installCrashHandler( CrashHandler handler )
{
    static bool crashHandlerInstalled = false;
    if ( !crashHandlerInstalled ) {
        crashHandlerInstalled = true;
        g_handler = handler;
        for ( unsigned int i = 0; i < sizeof( g_caughtSignals ) / sizeof( g_caughtSignals[0] ); ++i ) {
            signal( g_caughtSignals[i], crashHandler );
        }
    }
}

TRACELIB_NAMESPACE_END

/* tracetool - a framework for tracing the execution of C++ programs
 * Copyright 2010-2016 froglogic GmbH
 *
 * This file is part of tracetool.
 *
 * tracetool is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * tracetool is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with tracetool.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tracelib.h"
#include "filter.h"

#include <iostream>

using namespace std;

int g_failureCount = 0;
int g_verificationCount = 0;

template <typename T>
static void verify( const char *what, T expected, T actual )
{
    if ( !( expected == actual ) ) {
        cout << "FAIL: " << what << "; expected '" << boolalpha << expected << "', got '" << boolalpha << actual << "'" << endl;
        ++g_failureCount;
    }
    ++g_verificationCount;
}

TRACELIB_NAMESPACE_BEGIN

static void testStrictPathFilter()
{
    static TracePoint tpLowerCase( TracePointType::Log, "c:\\foo\\bar\\mysrc.cpp", 0, NULL, 0 );
    static TracePoint tpUpperCase( TracePointType::Log, "C:\\Foo\\Bar\\mysrc.cpp", 0, NULL, 0 );

    PathFilter emptyPathFilter;
    emptyPathFilter.setPath( StrictMatch, "" );
    verify( "emptyPathFilter on tpLowerCase", false, emptyPathFilter.acceptsTracePoint( &tpLowerCase ) );
    verify( "emptyPathFilter on tpUpperCase", false, emptyPathFilter.acceptsTracePoint( &tpUpperCase ) );

    PathFilter otherFileFilter;
    otherFileFilter.setPath( StrictMatch, "C:\\Foo\\Bar\\someotherfile.cpp" );
    verify( "otherFileFilter on tpLowerCase", false, otherFileFilter.acceptsTracePoint( &tpLowerCase ) );
    verify( "otherFileFilter on tpUpperCase", false, otherFileFilter.acceptsTracePoint( &tpUpperCase ) );

    PathFilter samePathLCFilter;
    samePathLCFilter.setPath( StrictMatch, "c:\\foo\\bar\\mysrc.cpp" );
    verify( "samePathLCFilter on tpLowerCase", true, samePathLCFilter.acceptsTracePoint( &tpLowerCase ) );
#ifdef _WIN32
    verify( "samePathLCFilter on tpUpperCase", true, samePathLCFilter.acceptsTracePoint( &tpUpperCase ) );
#else
    verify( "samePathLCFilter on tpUpperCase", false, samePathLCFilter.acceptsTracePoint( &tpUpperCase ) );
#endif

    PathFilter samePathUCFilter;
    samePathUCFilter.setPath( StrictMatch, "C:\\FOO\\BAR\\MYSRC.CPP" );
#ifdef _WIN32
    verify( "samePathUCFilter on tpLowerCase", true, samePathUCFilter.acceptsTracePoint( &tpLowerCase ) );
    verify( "samePathUCFilter on tpUpperCase", true, samePathUCFilter.acceptsTracePoint( &tpUpperCase ) );
#else
    verify( "samePathUCFilter on tpLowerCase", false, samePathUCFilter.acceptsTracePoint( &tpLowerCase ) );
    verify( "samePathUCFilter on tpUpperCase", false, samePathUCFilter.acceptsTracePoint( &tpUpperCase ) );
#endif

    PathFilter samePathFilter;
    samePathFilter.setPath( StrictMatch, "C:\\Foo\\Bar\\mysrc.cpp" );
#ifdef _WIN32
    verify( "samePathFilter on tpLowerCase", true, samePathFilter.acceptsTracePoint( &tpLowerCase ) );
#else
    verify( "samePathFilter on tpLowerCase", false, samePathFilter.acceptsTracePoint( &tpLowerCase ) );
#endif
    verify( "samePathFilter on tpUpperCase", true, samePathFilter.acceptsTracePoint( &tpUpperCase ) );
}

static void testWildcardPathFilter()
{
    static TracePoint tpLowerCase( TracePointType::Log, "c:\\foo\\bar\\mysrc.cpp", 0, NULL, 0 );
    static TracePoint tpUpperCase( TracePointType::Log, "C:\\Foo\\Bar\\mysrc.cpp", 0, NULL, 0 );

    PathFilter emptyPathFilter;
    emptyPathFilter.setPath( WildcardMatch, "" );
    verify( "emptyPathFilter on tpLowerCase", false, emptyPathFilter.acceptsTracePoint( &tpLowerCase ) );
    verify( "emptyPathFilter on tpUpperCase", false, emptyPathFilter.acceptsTracePoint( &tpUpperCase ) );

    PathFilter everyPathFilter;
    everyPathFilter.setPath( WildcardMatch, "*" );
    verify( "everyPathFilter on tpLowerCase", true, everyPathFilter.acceptsTracePoint( &tpLowerCase ) );
    verify( "everyPathFilter on tpUpperCase", true, everyPathFilter.acceptsTracePoint( &tpUpperCase ) );

    PathFilter everyPathFilter2;
    everyPathFilter2.setPath( WildcardMatch, "***" );
    verify( "everyPathFilter2 on tpLowerCase", true, everyPathFilter2.acceptsTracePoint( &tpLowerCase ) );
    verify( "everyPathFilter2 on tpUpperCase", true, everyPathFilter2.acceptsTracePoint( &tpUpperCase ) );

    PathFilter otherFileFilter;
    otherFileFilter.setPath( WildcardMatch, "*\\someotherfile.cpp" );
    verify( "otherFileFilter on tpLowerCase", false, otherFileFilter.acceptsTracePoint( &tpLowerCase ) );
    verify( "otherFileFilter on tpUpperCase", false, otherFileFilter.acceptsTracePoint( &tpUpperCase ) );

    PathFilter samePathLCFilter;
    samePathLCFilter.setPath( WildcardMatch, "*\\mysrc.cpp" );
    verify( "samePathLCFilter on tpLowerCase", true, samePathLCFilter.acceptsTracePoint( &tpLowerCase ) );
    verify( "samePathLCFilter on tpUpperCase", true, samePathLCFilter.acceptsTracePoint( &tpUpperCase ) );

    PathFilter samePathUCFilter;
    samePathUCFilter.setPath( WildcardMatch, "*\\Bar\\mysrc.cpp" );
#ifdef _WIN32
    verify( "samePathUCFilter on tpLowerCase", true, samePathUCFilter.acceptsTracePoint( &tpLowerCase ) );
#else
    verify( "samePathUCFilter on tpLowerCase", false, samePathUCFilter.acceptsTracePoint( &tpLowerCase ) );
#endif
    verify( "samePathUCFilter on tpUpperCase", true, samePathUCFilter.acceptsTracePoint( &tpUpperCase ) );

    PathFilter questionMarkFilter;
    questionMarkFilter.setPath( WildcardMatch, "*\\?ar\\mysrc.cpp" );
    verify( "questionMarkFilter on tpLowerCase", true, questionMarkFilter.acceptsTracePoint( &tpLowerCase ) );
    verify( "questionMarkFilter on tpUpperCase", true, questionMarkFilter.acceptsTracePoint( &tpUpperCase ) );
}

static void testPathFilter()
{
    testStrictPathFilter();
    testWildcardPathFilter();
}

static void testGroupFilter()
{
    static TracePoint consoleIOTP( TracePointType::Log, "S:\\hello\\main.cpp", 13, "main()", "ConsoleIO" );
    static TracePoint noGroupTP( TracePointType::Log, "S:\\hello\\main.cpp", 13, "main()", NULL );
    static TracePoint noGroupTP2( TracePointType::Log, "S:\\hello\\main.cpp", 13, "main()", "" );

    GroupFilter f;
    f.setMode( GroupFilter::Blacklist );
    f.addGroupName( "ConsoleIO" );
    verify( "f (blacklisting) on consoleIOTP", false, f.acceptsTracePoint( &consoleIOTP ) );
    verify( "f (blacklisting) on noGroupTP", true, f.acceptsTracePoint( &noGroupTP ) );
    verify( "f (blacklisting) on noGroupTP2", true, f.acceptsTracePoint( &noGroupTP2 ) );

    f.setMode( GroupFilter::Whitelist );
    verify( "f (whitelisting) on consoleIOTP", true, f.acceptsTracePoint( &consoleIOTP ) );
    verify( "f (whitelisting) on noGroupTP", false, f.acceptsTracePoint( &noGroupTP ) );
    verify( "f (whitelisting) on noGroupTP2", false, f.acceptsTracePoint( &noGroupTP2 ) );

    GroupFilter f2;
    f2.setMode( GroupFilter::Whitelist );
    f2.addGroupName( "ConsoleIO" );
    f2.addGroupName( "" );
    verify( "f2 (whitelisting) on consoleIOTP", true, f2.acceptsTracePoint( &consoleIOTP ) );
    verify( "f2 (whitelisting) on noGroupTP", true, f2.acceptsTracePoint( &noGroupTP ) );
    verify( "f2 (whitelisting) on noGroupTP2", true, f2.acceptsTracePoint( &noGroupTP2 ) );

    f2.setMode( GroupFilter::Blacklist );
    verify( "f2 (blacklisting) on consoleIOTP", false, f2.acceptsTracePoint( &consoleIOTP ) );
    verify( "f2 (blacklisting) on noGroupTP", false, f2.acceptsTracePoint( &noGroupTP ) );
    verify( "f2 (blacklisting) on noGroupTP2", false, f2.acceptsTracePoint( &noGroupTP2 ) );
}

TRACELIB_NAMESPACE_END

int main()
{
    TRACELIB_NAMESPACE_IDENT(testPathFilter)();
    TRACELIB_NAMESPACE_IDENT(testGroupFilter)();
    cout << g_verificationCount << " verifications; " << g_failureCount << " failures found." << endl;
    return g_failureCount;
}


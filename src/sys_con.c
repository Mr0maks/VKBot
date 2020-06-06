#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

// thanks Uncle Mike for this code (xash3d)

#define MAX_PRINT_MSG	8192

static FILE *logfile = NULL;

void Sys_Reg_LogFile()
{
    if(logfile)
        return;

    logfile = fopen("./engine.log", "w");
}

void Sys_PrintLog( const char *pMsg )
{
    time_t		crt_time;
    const struct tm	*crt_tm;
    char logtime[32] = "";
    static char lastchar;

    time( &crt_time );
    crt_tm = localtime( &crt_time );

    if( !lastchar || lastchar == '\n')
        strftime( logtime, sizeof( logtime ), "[%H:%M:%S] ", crt_tm ); //short time

    printf("%s %s", logtime, pMsg);
    fflush(stdout);

    if( !logfile )
        return;

    if( !lastchar || lastchar == '\n')
        strftime( logtime, sizeof( logtime ), "[%Y:%m:%d|%H:%M:%S]", crt_tm ); //full time

    // save last char to detect when line was not ended
    lastchar = pMsg[strlen(pMsg)-1];

    fprintf( logfile, "%s %s", logtime, pMsg );
    fflush( logfile );
}

void Con_Printf( const char *szFmt, ... )
{
        static char     buffer[MAX_PRINT_MSG];
        va_list         args;

        va_start( args, szFmt );
        vsnprintf( buffer, sizeof( buffer ), szFmt, args );
        va_end( args );

        Sys_PrintLog( buffer );
}

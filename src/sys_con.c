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

#ifdef XASH_COLORIZE_CONSOLE
    {
        char colored[4096];
        const char *msg = pMsg;
        int len = 0;
        while( *msg && ( len < 4090 ) )
        {
            static char q3ToAnsi[ 8 ] =
            {
                '0', // COLOR_BLACK
                '1', // COLOR_RED
                '2', // COLOR_GREEN
                '3', // COLOR_YELLOW
                '4', // COLOR_BLUE
                '6', // COLOR_CYAN
                '5', // COLOR_MAGENTA
                0 // COLOR_WHITE
            };

            if( IsColorString( msg ) )
            {
                int color;

                msg++;
                color = q3ToAnsi[ *msg++ % 8 ];
                colored[len++] = '\033';
                colored[len++] = '[';
                if( color )
                {
                    colored[len++] = '3';
                    colored[len++] = color;
                }
                else
                    colored[len++] = '0';
                colored[len++] = 'm';
            }
            else
                colored[len++] = *msg++;
        }
        colored[len] = 0;
        printf( "\033[34m%s\033[0m%s\033[0m", logtime, colored );

    }
#else
    printf("%s %s", logtime, pMsg);
    fflush(stdout);
#endif


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

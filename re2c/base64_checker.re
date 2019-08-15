#include <stdio.h>

/*!max:re2c*/

int base64_string(const char *base64_str)
{
	const char *YYCURSOR = base64_str;
	loop:
    /*!stags:re2c format = 'const char *@@;'; */
    /*!re2c
        re2c:define:YYCTYPE = char;
        re2c:yyfill:enable = 0;

        end = "\x00";
        string = [0-9a-zA-Z+=/ \n];

        * { return 0; }
        end { return 1; }
        string { goto loop; }
    */
}

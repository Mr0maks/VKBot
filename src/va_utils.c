/*
va_utils.c - Variantic arguments utils
Copyright (C) 2020  Mr0maks <mr.maks0443@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdarg.h>
#include <stdio.h>

char* va(const char* format, ...)
{
    va_list argptr;
    static char string[256][1024];
    static int stringindex = 0;
    char* s = NULL;

    s = string[stringindex];
    stringindex = (stringindex + 1) & 255;
    va_start(argptr, format);
    vsnprintf(s, sizeof(string[0]), format, argptr);
    va_end(argptr);

    return s;
}

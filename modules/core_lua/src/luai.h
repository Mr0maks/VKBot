#pragma once

#if defined(_WIN32) && defined(_MSC_VER)
#pragma comment(lib, "lua.lib")
#endif

#include <lauxlib.h>
#include <lua.h>
#include <luaconf.h>
#include <luajit.h>
#include <lualib.h>


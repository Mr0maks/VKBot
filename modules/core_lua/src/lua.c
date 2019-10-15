#include <enginecallbacks.h>
#include "luai.h"

lua_State *Main_Lua_State = NULL;

void Lua_Register_Funcs(lua_State *L);

int Lua_Init(void)
{
    Main_Lua_State = luaL_newstate();

    if(!Main_Lua_State)
        return 1;

    Lua_Register_Funcs(Main_Lua_State);

    if(luaL_loadfile(Main_Lua_State, "lua/main.lua"))
    {
        ALERT("[LUA] Error while load main file: %s\n", lua_tostring(Main_Lua_State, -1));
        lua_close(Main_Lua_State);
        return 1;
    }

    if(lua_pcall(Main_Lua_State, 0, 0 , 0))
    {
        ALERT("[LUA] Error while calling file: %s\n", lua_tostring(Main_Lua_State, -1));
        lua_close(Main_Lua_State);
        return 1;
    }

    ALERT("[LUA] OK!\n");
    return 0;
}

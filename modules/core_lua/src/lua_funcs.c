#include <enginecallbacks.h>
#include "luai.h"

extern module_info_t module_info;
extern lua_State *Main_Lua_State;

int Lua_print(lua_State *L)
{
    const char *str = luaL_checkstring(L, 1);
    ALERT("%s\n", str);
    return 0;
}

static void Lua_cmd_handler(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    lua_getglobal(Main_Lua_State, "engine_events");
    lua_getfield(Main_Lua_State, -1, argv[0]);

    if(lua_isfunction(Main_Lua_State, -1))
    {
        lua_newtable(Main_Lua_State);
        lua_pushstring(Main_Lua_State, "peer_id");
        lua_pushinteger(Main_Lua_State, message->peer_id);
        lua_settable(Main_Lua_State, -3);
        lua_pushstring(Main_Lua_State, "from_id");
        lua_pushinteger(Main_Lua_State, message->from_id);
        lua_settable(Main_Lua_State, -3);
        lua_pushstring(Main_Lua_State, "private_message");
        lua_pushboolean(Main_Lua_State, message->private_message);
        lua_settable(Main_Lua_State, -3);

        lua_pushnumber(Main_Lua_State, argc);

        lua_newtable(Main_Lua_State);

        for(int i = 0; i < argc; i++)
        {
            lua_pushnumber(Main_Lua_State, i + 1); // Key
            lua_pushstring(Main_Lua_State, argv[i]); // Value
            lua_settable(Main_Lua_State, -3);
        }

        lua_pushstring(Main_Lua_State, args);

        if(lua_pcall(Main_Lua_State, 4, 0 ,0))
        {
            ALERT("[LUA] CALLBACK FOR %s ERROR: %s!\n", argv[0], lua_tostring(Main_Lua_State, -1));
            return;
        }
    } else {
        lua_pop(Main_Lua_State, -1);
        return;
    }
}

int Lua_register_cmd(lua_State *L)
{
    REGISTER_COMMAND(&module_info, luaL_checkstring(L, 1), luaL_checkstring(L, 2), Lua_cmd_handler );
    return 0;
}

int Lua_Call_Method(lua_State *L)
{
    string_t special_params = NULL;
    string_t data = NULL;
    if(lua_isstring(L, 2))
    {
        special_params = STRING_INIT();
        STRING_COPY(special_params, luaL_checkstring(L, 2));

        data = VKAPI_CALL_METHOD(luaL_checkstring(L, 1), special_params, lua_toboolean(L, 3));

        if(data)
        {
            lua_pushstring(L, data->ptr);
            STRING_DESTROY(data);
            STRING_DESTROY(special_params);
            return 1;
        }

        STRING_DESTROY(special_params);
    } else if(lua_isboolean(L, 2)) {
        data = VKAPI_CALL_METHOD(luaL_checkstring(L, 1), NULL, lua_toboolean(L, 2));

        if(data)
        {
            lua_pushstring(L, data->ptr);
            STRING_DESTROY(data);
            STRING_DESTROY(special_params);
            return 1;
        }
    }

    return 0;
}

int Lua_Message_Send(lua_State *L)
{
    int toho = luaL_checkint(L, 1);
    const char *msg = luaL_checkstring(L, 2);

    VKAPI_SEND_MESSAGE(toho, msg, NULL, 0);
    return 0;
}

void Lua_Register_Funcs(lua_State *L)
{
    lua_register(L, "print", Lua_print);
    
    lua_register(L, "register_command", Lua_register_cmd);
    
    lua_register(L, "vkapi_send", Lua_Message_Send );
    lua_register(L, "vkapi_call", Lua_Call_Method);
    
//    lua_register(L, "db_open", 0);
//    lua_register(L, "db_exec", 0);
//    lua_register(L, "db_close", 0);
}

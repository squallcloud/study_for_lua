#include <iostream>
#include <lua.hpp>

int main()
{
    lua_State* L = luaL_newstate();

    luaL_openlibs(L);

    luaL_dostring(L, "print('Hello, World!')");

    luaL_dofile(L, "hello_world.lua");

    lua_close(L);
    return 0;
}

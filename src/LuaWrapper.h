#ifndef LUA_WRAPPER_H
#define LUA_WRAPPER_H

#include "Arduino.h"
#include "FS.h"
#define LUA_USE_C89
#include "lua/lua.hpp"


class LuaWrapper {
  public:
    LuaWrapper();
    String Lua_dostring(const String *script);
    String Lua_doFile(const char *file);
    void Lua_register(const String name, const lua_CFunction function);
    lua_State* L();
    String addConstants();

  private:
    lua_State *_state;
};

#endif

#include "LuaWrapper.h"

extern "C" {
  static int lua_wrapper_print (lua_State *L) {
    int n = lua_gettop(L);  /* number of arguments */
    int i;
    lua_getglobal(L, "tostring");
    for (i=1; i<=n; i++) {
      const char *s;
      size_t l;
      lua_pushvalue(L, -1);  /* function to be called */
      lua_pushvalue(L, i);   /* value to print */
      lua_call(L, 1, 1);
      s = lua_tolstring(L, -1, &l);  /* get result */
      if (s == NULL)
        return luaL_error(L, "'tostring' must return a string to 'print'");
      if (i>1) Serial.write("\t");
      Serial.write(s);
      lua_pop(L, 1);  /* pop result */
    }
    Serial.println();
    return 0;
  }

  static int lua_wrapper_pinMode(lua_State *lua_state) {
    int a = luaL_checkinteger(lua_state, 1);
    int b = luaL_checkinteger(lua_state, 2);
    pinMode(a, b);
    return 0;
  }

  static int lua_wrapper_digitalWrite(lua_State *lua_state) {
    int a = luaL_checkinteger(lua_state, 1);
    int b = luaL_checkinteger(lua_state, 2);
    digitalWrite(a, b);
    return 0;
  }
    
  static int lua_wrapper_delay(lua_State *lua_state) {
    int a = luaL_checkinteger(lua_state, 1);
    long time = esp_timer_get_time();
    long next = time + a*1000;
    while(time < next) {
      time = esp_timer_get_time();
      portYIELD();
    }
    return 0;
  }

  static int lua_wrapper_millis(lua_State *lua_state) {
    lua_pushnumber(lua_state, (lua_Number) millis());
    return 1;
  } 
  

  static int lua_wrapper_mikros(lua_State *lua_state) {
    lua_pushnumber(lua_state, (lua_Number) esp_timer_get_time());
    return 1;
  } 
  

} 

lua_State* LuaWrapper::L() {
  return _state;
}

LuaWrapper::LuaWrapper() { 
  _state = luaL_newstate();
  luaopen_base(_state);
  luaopen_table(_state);
  luaopen_string(_state);
  luaopen_math(_state);
  lua_register(_state, "print", lua_wrapper_print);
  lua_register(_state, "pinMode", lua_wrapper_pinMode);
  lua_register(_state, "digitalWrite", lua_wrapper_digitalWrite);
  lua_register(_state, "delay", lua_wrapper_delay);
  lua_register(_state, "millis", lua_wrapper_millis); 
  lua_register(_state, "mikros", lua_wrapper_mikros);
}

String LuaWrapper::Lua_dostring(const String *script) {
  String scriptWithConstants = *script;
  String result;
  if (luaL_dostring(_state, scriptWithConstants.c_str())) {
    result += "# lua error:\n" + String(lua_tostring(_state, -1));
    lua_pop(_state, 1);
  }
  return result;
}

String LuaWrapper::Lua_doFile(const char *file) { 
  String result; 
  if (luaL_dofile(_state, file)) {
    result += "# lua error:\n" + String(lua_tostring(_state, -1));
    lua_pop(_state, 1);
  }
  return result;
}

void LuaWrapper::Lua_register(const String name, const lua_CFunction function) {
  lua_register(_state, name.c_str(), function);
}

String LuaWrapper::addConstants() {
  String constants = "INPUT = " + String(INPUT) + "\r\n";
  constants += "OUTPUT = " + String(OUTPUT) + "\r\n";
  constants += "LOW = " + String(LOW) + "\r\n";
  constants += "HIGH = " + String(HIGH) + "\r\n";
  return constants;
}

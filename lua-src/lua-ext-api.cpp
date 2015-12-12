#include "lua-ext-api.h"
#include "lua.h"

lua_State *L;

/*----------------------get lua and lua ext version-------------------*/
LUA_EXT_API LUA_VER_STR e_get_lua_ver_str(){
    return VER_STR;
}

LUA_EXT_API LUA_VER_INT e_get_lua_ver_int(){
    return VER_INT;
}

LUA_EXT_API LUA_VER_INT e_get_lua_ext_ver_int(){
    return LUA_VERSION_NUM;
}

LUA_EXT_API LUA_EXT_VER_STR e_get_lua_ext_ver_str(){
    return LUA_VERSION;
}

/*------------------------make c call lua args or make lua call c args*/
LUA_EXT_API ARG* a_make_args(...);

LUA_EXT_API ARG* a_first_arg(ARG_PTR arg);

LUA_EXT_API ARG* a_eof(ARG_PTR arg);

LUA_EXT_API ARG* a_next(ARG_PTR arg);

LUA_EXT_API RESULT e_init_vm();

LUA_EXT_API RESULT e_uninit_vm();

LUA_EXT_API LOADER_KEY e_register_loader();

LUA_EXT_API RESULT e_unregister_loader(LOADER_KEY key);

/*----------------------------c call lua-------------------------------*/
LUA_EXT_API RESULT c_register_usertype(TYPE_NAME tname);

LUA_EXT_API RESULT c_unregister_usertype(TYPE_NAME tname);

LUA_EXT_API RESULT c_call_fun(FUN_NAME fun,ARG_PTR arg);

LUA_EXT_API RESULT c_call_static(CLS_NAME cls,FUN_NAME fun,ARG_PTR arg);

LUA_EXT_API RESULT c_call_instance(INSTANCE ids,CLS_NAME cls,FUN_NAME fun,ARG_PTR arg);

/*------------------------------for lua can call c---------------------*/
LUA_EXT_API RESULT L_register_lua_api__usertype(TYPE_NAME tname);

LUA_EXT_API RESULT L_register_lua_api_fun(FUN_NAME fun,ARG_PTR arg);

LUA_EXT_API RESULT L_register_lua_api_static_lua_api(CLS_NAME cls,FUN_NAME fun,ARG_PTR arg);

LUA_EXT_API RESULT L_register_lua_api__instance_lua_api(INSTANCE ids,CLS_NAME cls,FUN_NAME fun,ARG_PTR arg);

/*-----------------------------for debuger-------------------------------*/
LUA_EXT_API const char* d_get_last_error(){
    //todo::get lua err;
    lua_error(L);
    return NULL;
}

LUA_EXT_API RESULT d_print(const char* msg){
    //TODO:: lua  debuge helper
    return -1;
}

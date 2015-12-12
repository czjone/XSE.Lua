#ifndef _LUA_EXT_API_
#define _LUA_EXT_API_ 1

/*result types define*/
#define RES_OK 				1
#define RES_ERROR 			-1
#define RES__ERROR_INIT_VM	1

#define VER_STR 			"0.0.1"
#define	VER_INT 			001

/*api difine keyword.*/
#define LUA_EXT_API 
#define __INLINE__ 			inline

/*types redfine*/
typedef int 				RESULT;
typedef int 				LOADER_KEY;
typedef const char*			FUN_NAME;
typedef const char*			CLS_NAME;
typedef const char*			TYPE_NAME;
typedef void*				INSTANCE;
typedef const char*         LUA_VER_STR;
typedef int 				LUA_VER_INT;
typedef const char* 		LUA_EXT_VER_STR;
typedef int 				LUA_EXT_VER_INT;

typedef struct __ARG
{
	void* val;
	void* type;
	__ARG* next;
} ARG;

typedef ARG*  ARG_PTR;

/*----------------------get lua and lua ext version-------------------*/
LUA_EXT_API LUA_VER_STR e_get_lua_ver_str();

LUA_EXT_API LUA_VER_INT e_get_lua_ver_int();

LUA_EXT_API LUA_VER_INT e_get_lua_ext_ver_int();

LUA_EXT_API LUA_EXT_VER_STR e_get_lua_ext_ver_str();

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
LUA_EXT_API const char* d_get_last_error();

LUA_EXT_API RESULT d_print(const char* msg);

#endif /* end _LUA_EXT_API_*/
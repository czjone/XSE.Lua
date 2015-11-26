
#ifndef __LUA_INTERFACE__H__
#define __LUA_INTERFACE__H__ 	1

#define LUA_EXT_RESULT 			int
#define LUA_EXT_RESULT_OK 		1
#define LUA_EXT_RESULT_ERROR 	-1

/** lua extention interfaces*/
LUA_EXT_RESULT 			Init();
LUA_EXT_RESULT 			unInit();
LUA_EXT_RESULT_ERROR 	getError();

/** lua extention definition*/

#end /*end __LUA_INTERFACE__H__*/
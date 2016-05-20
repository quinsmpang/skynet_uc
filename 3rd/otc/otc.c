#include <lua.h>
#include <lauxlib.h>


static int
lname(lua_State *L) {
	lua_pushstring(L, "OTC!!!");
	return 1;
}

int
luaopen_otc(lua_State *L) {
	luaL_checkversion(L);

	luaL_Reg l[] = {
		{ "name", lname },
		{ NULL, NULL },
	};

	luaL_newlib(L,l);

	return 1;
}

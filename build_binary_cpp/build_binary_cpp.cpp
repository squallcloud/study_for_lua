#include <iostream>
#include <memory>
#include <lua.hpp>

class LuaCtrl
{
public:
    LuaCtrl() = default;
    ~LuaCtrl() {
        Close();
    }

    bool Setup() {
        lua_state_ = luaL_newstate();
        return !!lua_state_;
    }

    void Close() {
        if (lua_state_) {
            lua_close(lua_state_);
        }
    }

    lua_State* lua_state() {
        return lua_state_;
    }
    const lua_State* lua_state() const {
        return lua_state_;
    }


private:
    lua_State* lua_state_{};
};

class LuaBinBuilder
{
public:
    LuaBinBuilder(LuaCtrl& in_ctrl)
    : ctrl_(in_ctrl)
    {
    }

    bool Build(const char* in_src, const char* in_dst) {
        FILE* fin{};
        FILE* fout{};

        fopen_s(&fin, in_src, "r");

        fopen_s(&fout, in_dst, "wb");

        lua_load(ctrl_.lua_state(), (lua_Reader)Read, fin, "", nullptr);
        lua_dump(ctrl_.lua_state(), (lua_Writer)Write, fout, false);

        if (fout) {
            fclose(fout);
        }
        if (fin) {
            fclose(fin);
        }

        return true;
    }

private:
    static const char* Read(lua_State* L, FILE* fp, size_t* size) {
        static char g_buf[1024];
        if (feof(fp)) {
            *size = 0;
            return nullptr;
        } else {
            *size = fread(g_buf, 1, sizeof(g_buf), fp);
            return g_buf;
        }
    }

    static int Write(lua_State* ls, const void* data, size_t size, FILE* fp) {
        fwrite(data, size, 1, fp);
        return 0; // no error
    }


private:
    LuaCtrl& ctrl_;
};

int main()
{
    int retval = -1;

    if (LuaCtrl lua_ctrl; lua_ctrl.Setup()) {
        if (LuaBinBuilder builder(lua_ctrl); builder.Build("test.lua", "./test.luab")) {
            retval = 0;
        }
    }

    return retval;
}
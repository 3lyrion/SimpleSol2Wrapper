/*

██╗░░░░░██╗░░░██╗░█████╗░  ░░░░░░  ░██████╗██╗███╗░░░███╗██████╗░██╗░░░░░███████╗
██║░░░░░██║░░░██║██╔══██╗  ░░░░░░  ██╔════╝██║████╗░████║██╔══██╗██║░░░░░██╔════╝
██║░░░░░██║░░░██║███████║  █████╗  ╚█████╗░██║██╔████╔██║██████╔╝██║░░░░░█████╗░░
██║░░░░░██║░░░██║██╔══██║  ╚════╝  ░╚═══██╗██║██║╚██╔╝██║██╔═══╝░██║░░░░░██╔══╝░░
███████╗╚██████╔╝██║░░██║  ░░░░░░  ██████╔╝██║██║░╚═╝░██║██║░░░░░███████╗███████╗
╚══════╝░╚═════╝░╚═╝░░╚═╝  ░░░░░░  ╚═════╝░╚═╝╚═╝░░░░░╚═╝╚═╝░░░░░╚══════╝╚══════╝

░██████╗░█████╗░██╗░░░░░██████╗░  ░██╗░░░░░░░██╗██████╗░░█████╗░██████╗░██████╗░███████╗██████╗░
██╔════╝██╔══██╗██║░░░░░╚════██╗  ░██║░░██╗░░██║██╔══██╗██╔══██╗██╔══██╗██╔══██╗██╔════╝██╔══██╗
╚█████╗░██║░░██║██║░░░░░░░███╔═╝  ░╚██╗████╗██╔╝██████╔╝███████║██████╔╝██████╔╝█████╗░░██████╔╝
░╚═══██╗██║░░██║██║░░░░░██╔══╝░░  ░░████╔═████║░██╔══██╗██╔══██║██╔═══╝░██╔═══╝░██╔══╝░░██╔══██╗
██████╔╝╚█████╔╝███████╗███████╗  ░░╚██╔╝░╚██╔╝░██║░░██║██║░░██║██║░░░░░██║░░░░░███████╗██║░░██║
╚═════╝░░╚════╝░╚══════╝╚══════╝  ░░░╚═╝░░░╚═╝░░╚═╝░░╚═╝╚═╝░░╚═╝╚═╝░░░░░╚═╝░░░░░╚══════╝╚═╝░░╚═╝

Header-only C++20 simple sol2 wrapper https://github.com/Mouseunder/SimpleSol2Wrapper

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2024 Mouseunder

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files( the "Software" ), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#include <sol/sol.hpp>

#define LUA_GET auto& lua = Lua::get

class Lua
{
public:
	using Object = sol::object;
	using nil_t  = sol::nil_t;

	/*inline static*/ const nil_t& nil = sol::lua_nil;

	class Table
	{
	public:
		Table(const sol::table& table) :
			m_table(table) { }

		constexpr void setMethod(const std::string& name, auto&& value)
		{
			if constexpr (std::is_function<decltype(value)>::value)
				m_table.set_function(name, value);

			else
				m_table.set_function(name, function(value));
		}

		constexpr void setField(const std::string& name, auto&& value)
		{
			m_table[name] = value;
		}

	private:
		sol::table m_table;
	};

	template <typename T>
	class Class
	{
	public:
		Class(const sol::usertype<T>& usertype) :
			m_usertype(usertype) { }

		constexpr void set(const std::string& name, auto&& value)
		{
			m_usertype[name] = value;
		}

	private:
		sol::usertype<T> m_usertype;
	};

	inline static Lua& get()
	{
		static Lua instance;

		return instance;
	}

	Lua(const Lua&)              = delete;
	Lua& operator = (const Lua&) = delete;

	inline void shutdown()
	{
		m_view.collect_garbage();
	}
	
	inline Table createStaticObject(const std::string& name)
	{
		return m_view[name].get_or_create<sol::table>();
	}

	inline Table createObject(const std::string& name = "")
	{
		if (!name.empty()) return m_view.create_named_table(name);

		return m_view.create_table();
	}

	template <typename YourClass>
	constexpr Class<YourClass> bindClass(const std::string& name)
	{
		return m_view.new_usertype<YourClass>(name);
	}

	constexpr Object toLuaObject(auto&& object)
	{
		return sol::make_object(m_view.lua_state(), object);
	}

	inline Object exec(const std::string& script)
	{
		return m_view.script(script);
	}
	
	inline Object execFile(const std::string& path)
	{
		return m_view.script_file(path);
	}
	
	inline lua_State* getState()
	{
		return m_view.lua_state();
	}

private:
	sol::state m_view;

	Lua()
	{
		m_view.open_libraries();
	}

	~Lua() { }
};


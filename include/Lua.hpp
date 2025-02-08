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

Header-only C++20 simple sol2 wrapper https://github.com/3lyrion/SimpleSol2Wrapper

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2025 3lyrion

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

#define LUA_GET auto& lua = el::Lua::get

#define LUA_SELF el::Lua::Object _self

namespace el
{

class Lua
{
public:
	using Object = sol::object;
	using nil_t  = sol::nil_t;

	/*inline static*/ const nil_t& nil = sol::lua_nil;

	class Table
	{
	public:
		explicit Table(sol::table&& table) :
			m_table(std::move(table)) { }

		constexpr void setMethod(std::string const& name, auto&& value)
		{
			if constexpr (std::is_function<decltype(value)>::value)
				m_table.set_function(name, value);

			else
				m_table.set_function(name, std::function(std::forward<decltype(value)>(value)));
		}

		constexpr void setField(std::string const& name, auto&& value)
		{
			m_table[name] = std::forward<decltype(value)>(value);
		}

	private:
		sol::table m_table;
	};

	template <typename T>
	class Class
	{
	public:
		explicit Class(sol::usertype<T>&& usertype) :
			m_usertype(std::move(usertype)) { }

		template <typename... Args>
		constexpr void setConstructor()
		{
			m_usertype["new"] = sol::constructors<Args...>();
		}

		constexpr void set(std::string const& name, auto&& value)
		{
			m_usertype[name] = std::forward<decltype(value)>(value);
		}

	private:
		sol::usertype<T> m_usertype;
	};

public:
	inline static Lua& get()
	{
		static Lua instance;
		return instance;
	}

	Lua(Lua const&)              = delete;
	Lua& operator = (Lua const&) = delete;

	inline void shutdown()
	{
		m_view.open_libraries();
	}

	inline Table createStaticObject(std::string const& name)
	{
		return Table(std::move(m_view[name].get_or_create<sol::table>()));
	}

	inline Table createObject(std::string const& name = "")
	{
		if (!name.empty())
			return Table(std::move(m_view.create_named_table(name)));

		return Table(std::move(m_view.create_table()));
	}

	template <typename YourClass>
	constexpr Class<YourClass> bindClass(std::string const& name)
	{
		return Class<YourClass>(std::move(m_view.new_usertype<YourClass>(name)));
	}

	constexpr Object toLuaObject(auto&& object)
	{
		return sol::make_object(m_view.lua_state(), std::forward<decltype(object)>(object));
	}

	inline Object exec(std::string const& script)
	{
		return m_view.script(script);
	}
	
	inline Object execFile(std::string const& path)
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

	~Lua() = default;
};

} // namespace el
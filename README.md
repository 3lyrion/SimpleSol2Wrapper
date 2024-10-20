# Header-only C++20 simple sol2 wrapper

## Description

• Lua is a singleton.

## Example

**Case 1**
```cpp
void hello()
{
	std::cout << "Hello!";
}

int main()
{
	LUA_GET(); // auto& lua = Lua::get();

	Lua::Table obj = lua.createObject("test");
	obj.setMethod("hi", hello);

	lua.exec("test:hi()");

	Lua::Object result = lua.execFile ("Data/MyScript.lua")
	int num = result.as<int>();

	return 0;
}
```

**Example №2**
```cpp
class Weapon
{
public:
	static void bindLua()
	{
		LUA_GET();

		auto wpn = lua.bindClass<Weapon>("Weapon");
		wpn.set("damage",    &Weapon::m_damage);
		wpn.set("getName",   &Weapon::getName);  // name is read-only
	}

	Weapon(const std::string& name, int damage) :
		m_name   (name),
		m_damage (damage) { }

	const std::string& getName   () const { return m_name; }
	int                getDamage () const { return m_damage; }

private:
	std::string m_name;
	int         m_damage;
};

Lua::Object findWeapon(const string& name, Lua::Object container)
{
	LUA_GET();

	std::vector<Weapon> weapons = container;

	for (const auto& wpn : weapons)
	{
		if (wpn.getName() == name)
			return lua.toLuaObject(wpn);
	}
}

struct Human
{
	using WeaponPtr = std::unique_ptr<Weapon>;

	std::vector<WeaponPtr> sharedWeapons;

	void registerLua()
	{
		LUA_GET();

		Lua::Table player = lua.createStaticObject("Player");

		player.setField("weapon", Weapon("Pistol", 10));

		player.setMethod("getSharedWeapons", [this, &lua] // -> vector<Lua::Object>
			{
				vector<Lua::Object> luaWpns;

				for (auto& wpn : sharedWeapons)
				{
					luaWpns.emplace_back(
						lua.toLuaObject(*wpn)
					);
				}

				return luaWpns;
			}
		);
	}
};

int main()
{
	Weapon::bindLua();

	Human human;
	human.registerLua();

	auto utility = lua.createStaticObject("util");
	utility.setMethod("findWeapon", findWeapon);

	lua.exec(R"(
		local weapons = Player:getSharedWeapons()

		local wpn1  = weapons[0]
		wpn1.damage = 100

		local wpn  = util:findWeapon('Rifle', weapons)
	)");

	return 0;
}
```

## License

This project is under MIT License.

Copyright (c) 2024 3lyrion

> Permission is hereby granted, free of charge, to any person obtaining a copy  
> of this software and associated documentation files (the "Software"), to deal  
> in the Software without restriction, including without limitation the rights  
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell  
> copies of the Software, and to permit persons to whom the Software is  
> furnished to do so, subject to the following conditions:  
> 
> 
> The above copyright notice and this permission notice shall be included in all  
> copies or substantial portions of the Software.  
> 
> 
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  
> SOFTWARE.

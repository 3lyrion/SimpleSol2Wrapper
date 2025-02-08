# Header-only C++20 simple sol2 wrapper

## Description

- Lua is a singleton.

- You can create static objects, bind classes and do much more using my intuitive interface.

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

	el::Lua::Table LUtil = lua.createStaticObject("util");
	LUtil.setMethod("print",
		[&lua](LUA_SELF, string const& msg)
		{
			std::cout << msg << '\n';
		}
	);

	el::Lua::Table obj = lua.createObject("test");
	obj.setMethod("hi", hello);

	lua.exec("test:hi()");

    el::Lua::Object result = lua.execFile("Data/MyScript.lua");
    int num = result.as<int>();

	lua.shutdown();

    return 0;
}
```

**Case 2**
```cpp
template<typename T>
class Range
{
public:
	T min{};
	T max{};

	Range() = default;

	Range(T const& min_, T const& max_)
	{
		min = min_;
		max = max_;
	}
};

using RangeI = Range<int>;

class Weapon
{
public:
	Weapon(std::string const& name, RangeI damage) :
		m_name		(name),
		m_damage	(damage)
	{
		LUA_GET(); // auto& lua = el::Lua::get();

		auto LWeapon = lua.bindClass<Weapon>("Weapon");
		LWeapon.setConstructor<Weapon(std::string const&, RangeI)>();
		LWeapon.set("damage",    &Weapon::m_damage);
		LWeapon.set("getName",   &Weapon::getName);  // name is read-only
	}

	std::string const&	getName		() const { return m_name; }
	int					getDamage	() const { return m_damage; }

private:
	inline static s_staticRegsCompleted = false;

	std::string	m_name{};
	RangeI		m_damage{};
};

Weapon* findWeapon(string const& name, Lua::Object container)
{
	LUA_GET();

	std::vector<Weapon*> weapons = container;

	for (auto wpn : weapons)
	{
		if (wpn.getName() == name)
			return wpn;
	}

	return nullptr;
}

int main()
{
	LUA_GET();

	auto LRange = lua.bindClass<RangeI>("Range");
	LRange.setConstructor<RangeI(int const&, int const&)>();
	LRange.set("min", &RangeI::x);
	LRange.set("max", &RangeI::y);

	auto pistol = new Weapon("Pistol", { 10, 20 });

	auto LPlayer = lua.createStaticObject("Player");
	LPlayer.setField("weapon", pistol);
	LPlayer.setField("arsenal", new std::vector<Weapon*>{ pistol });

    auto utility = lua.createStaticObject("util");
	utility.setMethod("findWeapon", findWeapon);

    lua.exec(R"(
		Player.arsenal:add(Weapon.new('Rifle', Range.new(20, 30)))
		Player.weapon.damage.min = 5;

        local wpn = util:findWeapon('Pistol', weapons)
    )");

	lua.shutdown();

    return 0;
}
```

## License

This project is under MIT License.

Copyright (c) 2025 3lyrion

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

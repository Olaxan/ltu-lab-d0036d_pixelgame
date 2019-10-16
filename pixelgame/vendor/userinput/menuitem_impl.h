#include "menu.h"

////Implement hashing functions
//namespace std
//{
//	template <typename T>
//	struct hash<efiilj::Menu<T>::MenuItem>
//	{
//		size_t operator()(const efiilj::Menu<T>::MenuItem& i) const
//		{
//			//Return hash of all (important) fields.
//			return hash<string>()(i.name);
//		}
//	};
//}

namespace efiilj
{
	template <typename T>
	inline Menu<T>::MenuItem::MenuItem() 
		: parent(nullptr), name(""), hasSubmenu(false), hasData(false) { }

	template <typename T>
	inline Menu<T>::MenuItem::MenuItem(const Menu* parent, std::string name, bool(*func)())
		: parent(parent), name(name), func(func), hasSubmenu(false), hasData(false) { }

	template <typename T>
	inline Menu<T>::MenuItem::MenuItem(const Menu* parent, std::string name, Menu* subMenu) 
		: parent(parent), name(name), subMenu(subMenu), hasSubmenu(true), hasData(false) { }

	template<typename T>
	inline Menu<T>::MenuItem::MenuItem(const Menu* parent, std::string name, bool(*func)(T data), T data)
		: parent(parent), name(name), func_T(func), hasSubmenu(false), hasData(true), data(data) { }

	template <typename T>
	bool Menu<T>::MenuItem::Invoke() const
	{
		///Execute menu option - show submenu if one, otherwise run provided function.
		if (hasSubmenu)
		{
			this->subMenu->Show();
			return false;
		}
		else return (hasData ? this->func_T(data) : this->func());
	}
}
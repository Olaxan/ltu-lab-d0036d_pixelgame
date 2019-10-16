#pragma once

#include <string>
#include <vector>

namespace efiilj
{

	template <typename T = int>
	class Menu
	{
	private:

		class MenuItem
		{
		private:
			Menu* subMenu;
			const Menu* parent;
			bool(*func)();
			bool(*func_T)(T data);
			bool hasSubmenu;
			bool hasData;
			T data;

		public:
			MenuItem();
			MenuItem(const Menu* parent, std::string name, bool(*func)());
			MenuItem(const Menu* parent, std::string name, Menu* subMenu);
			MenuItem(const Menu* parent, std::string name, bool(*func)(T data), T data);

			std::string name;

			bool Invoke() const;

		};

		std::vector<MenuItem> _items;
		bool _isSubmenu = false;

	public:
		Menu(std::string title = "", std::string prompt = "> ");

		std::string title;	//Menu title
		std::string prompt;	//Menu prompt
		
		bool allowExit = true;	//Allow exit with '0'.

		void AddItem(std::string name, bool(*func)());	//Add a function pointer to the menu.
		void AddItem(std::string name, bool(*func)(T data), T data);
		void AddItem(std::string name, Menu* subMenu);	//Add a submenu to the menu.
		void RemoveItem(std::string item); //TODO?
		void ListItems() const;
		
		bool Show() const;	//Show menu, loop until the user exits from the top-level menu.

		~Menu();

		//Required to implement hashing function.
		friend std::hash<MenuItem>;
	};

}

#include "menu_impl.h"
#include "menuitem_impl.h"

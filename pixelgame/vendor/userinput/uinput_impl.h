#pragma once

#include "uinput.h"

#include <typeinfo>

namespace efiilj
{

	template<typename T>
	UserInput<T>::UserInput(std::string query, std::string prompt) : _query(query), _prompt(prompt)
	{
		//static_assert(is_stream_extractable<std::stringstream, T>, "Type is not streamable");
		//Assertion is not currently working.
		//Supposed to give a more understandable error message when compiled for an invalid type (non-stringstream convertible).
		//Compilation fails regardless, but with a weird error message.

		_state = false;
	}

	template<typename T>
	template<typename>
	inline UserInput<T>::UserInput(std::string query, std::string prompt, T min, T max) : _query(query), _prompt(prompt)
	{
		Limits(min, max);
	}

	template<typename T>
	inline bool UserInput<T>::Show()
	{
		//Show the query until a valid input is given, or user exits with exit string.

		std::string input;

		std::cout << _query;

		if (exit != "")
			std::cout << " ('" << exit << "' to exit)";
		std::cout << "\n" << _prompt;

		while (true)
		{
			std::getline(std::cin, input);

			if (input.length() == 0)
				continue;

			if (input == exit)
				return false;

			if (Validate(input) && TestLimits())
				return true;

			std::cout << "Invalid input - enter '" << typeid(T).name() << "'";
			if (!TestLimits()) std::cout << " between " << _min << " and " << _max;
			std::cout << ": ";
		}
	}

	template<typename T>
	UserInput<T>::~UserInput() { }

}


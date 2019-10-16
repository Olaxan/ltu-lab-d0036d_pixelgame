#pragma once

#include <limits>
#include <type_traits>
#include <sstream>
#include <string>
#include <iostream>

namespace efiilj
{

	//Type trait used for testing which types can be used in Input template.

	template<typename S, typename T, typename = void> 
	struct is_stream_extractable : std::false_type {}; 

	//Use decltype to test stream operation, "true" if object can be stream extracted.
	template<typename S, typename T>
	struct is_stream_extractable<S, T,
		std::void_t<  decltype(std::declval<S&>() >> std::declval<T>())>> 
		: std::true_type {};

	template<typename S, typename T, typename = void>
	struct is_stream_insertable : std::false_type {};

	//Use decltype to test stream operation, "true" if object can be stream inserted.
	template<typename S, typename T>
	struct is_stream_insertable<S, T,
		std::void_t<  decltype(std::declval<S&>() << std::declval<T>())  > >
		: std::true_type {};

	template <typename T>
	class UserInput
	{
	private:
		std::string _error;		//Error to be printed when user enters an invalid input (unused?)
		std::string _prompt;
		std::string _query;		//Query/title to be printed once.
		bool _state;			//Has the input been converted successfully?
		T _value, _min, _max;

		template<typename U = T>
		typename std::enable_if<!std::is_same<std::string, U>::value, bool>::type
			Validate(std::string input) // Validate, type is not string
			{
				std::stringstream ss;

				ss << input;
				ss >> _value;

				_state = !ss.fail();
				return _state;
			}

		template<typename U = T>
		typename std::enable_if<std::is_same<std::string, U>::value, bool>::type
			Validate(std::string input) // Validate, type is string
			{
				_value = input;
				return _state = true;
			}

		//SFINAE: Use this variant if T is arithmetic (can use greater-than/smaller-than operators).
		template<typename U = T>
		typename std::enable_if<std::is_arithmetic<U>::value, bool>::type
			TestLimits() { return (_value >= _min && _value <= _max); }

		//SFINAE: If T is not arithmetic, TestLimits always returns true.
		template<typename U = T>
		typename std::enable_if<!std::is_arithmetic<U>::value, bool>::type
			TestLimits() { return true; }

	public:
		UserInput(std::string query, std::string prompt);

		//Constructor only available when template is compiled for an arithmetic type.
		template <typename = std::enable_if_t<std::is_arithmetic<T>::value>>
		UserInput(std::string query, std::string prompt, T min = INT_MIN, T max = INT_MAX);

		//Limits setter only available when compiled for arithmetic type.
		template <typename = std::enable_if_t<std::is_arithmetic<T>::value>>
		inline void Limits(T min, T max)
		{
			_min = min;
			_max = max;
		}

		std::string exit; //If this is set, the user can enter it to exit the query.

		bool Show(); //Show the query until a valid input is given, or user exits with exit string.

		T Value() { return _value; };

		bool operator == (bool state) { return _state; }
		bool operator != (bool state) { return !_state; }
		explicit operator bool() const { return _state; }

		~UserInput();
	};
}

//Include template implementation file (for organization purposes).
#include "uinput_impl.h"


#pragma once
#include <string>
#include <vector>
#include <climits>

inline std::vector<std::string> split(std::string input, const std::string& splitter)
{
	size_t index = input.find_first_of(splitter);
	std::vector<std::string> split;
	
	while (index != std::string::npos)
	{
		std::string sub = input.substr(0, index);
		if (!sub.empty())
			split.push_back(sub);
		
		input.erase(0, 1 + index);
		index = input.find_first_of(splitter);
	}
	
	if (!input.empty())
		split.push_back(input);
	
	return split;
}

inline std::vector<std::string> split4(const std::string& input, const std::string& delims = " ")
{
	std::size_t previous = 0;
	std::vector<std::string> split;
	
	std::size_t current = input.find_first_of(delims);
	
	while (current != std::string::npos) {
		split.push_back(input.substr(previous, current - previous));
		previous = current + 1;
		current = input.find_first_of(delims, previous);
	}
	split.push_back(input.substr(previous, current - previous));

	return split;
}

// 
template <typename T>
T swap_endian(T u)
{
	static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

	union
	{
		T u;
		unsigned char u8[sizeof(T)];
	} source, dest;

	source.u = u;

	for (size_t k = 0; k < sizeof(T); k++)
		dest.u8[k] = source.u8[sizeof(T) - k - 1];

	return dest.u;
}
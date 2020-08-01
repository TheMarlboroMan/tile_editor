#pragma once

#include <string>

namespace tile_editor {

enum class property_links {
	nothing,
	w,
	h,
	color_red,
	color_green,
	color_blue,
	color_alpha
};

//!This is a property definition, which can be used to fill for
template<typename T>
struct property_definition {

	public:

	//Allow this thing to be default constructible.
	                    property_definition():
		linked_to{property_links::nothing} 
	{

	}

	                    property_definition(
		const std::string& _name,
		const T& _value,
		const std::string& _description,
		property_links _link
	):
		name{_name},
		default_value{_value},
		description{_description},
		linked_to{_link}
	{

	}

	std::string         name;
	T                   default_value;
	std::string         description;
	property_links      linked_to;
};

}

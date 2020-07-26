#pragma once

#include <string>

namespace tile_editor {

//!This is a property definition, which can be used to fill for
template<typename T>
struct property_definition {

	public:

	//Allow this thing to be default constructible.
	                    property_definition() {

	}

	                    property_definition(
		const std::string& _name,
		const T& _value,
		const std::string& _description
	):
		name{_name},
		default_value{_value},
		description{_description}
	{

	}

	std::string         name;
	T                   default_value;
	std::string         description;
};

}

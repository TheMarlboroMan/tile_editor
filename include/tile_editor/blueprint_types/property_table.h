#pragma once

#include "property_definition.h"

#include <string>
#include <map>

namespace tile_editor {

//!A table of properties that may be used for a map or thing, contains the
//!blueprints for the properties, but not the properties themselves.
struct property_table {

	public:

	bool                            property_exists(const std::string&) const;

	//TODO: sort of a waste, the property already knows its name...
	std::map<std::string, property_definition<int>>         int_properties;
	std::map<std::string, property_definition<double>>      double_properties;
	std::map<std::string, property_definition<std::string>> string_properties;
};

}

#pragma once

#include <string>
#include <map>

namespace tile_editor {

//!A container for the different properties that a thing or a map might have.
class property_manager {

	private:

	std::map<std::string, int>          int_properties;
	std::map<std::string, double>       double_properties;
	std::map<std::string, std::string>  string_properties;

};

}
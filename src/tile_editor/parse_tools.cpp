#include "parser/parse_tools.h"

#include <sstream>

using namespace tile_editor;

tile_editor::color tile_editor::parse_color(const std::string& _str) {

	int r{}, g{}, b{}, a{};
	std::stringstream ss{_str};
	ss>>r>>g>>b>>a;

	if(ss.fail()) {

		throw std::runtime_error("invalid color schema, values are red, green, blue and alpha, all from 0 to 255 and separated by spaces");
	}

	return {r, g, b, a};
}

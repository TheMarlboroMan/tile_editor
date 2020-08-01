#include "parser/property_parser.h"
#include "parser/parse_tools.h"

#include <tools/file_utils.h>

using namespace tile_editor;

property_parser::property_parser(
	bool _map_mode
):
	map_mode{_map_mode}
{
}

property_table property_parser::read_file(const std::string& _path) {

	if(!tools::file_exists(_path)) {

		throw std::runtime_error(std::string{"cannot find properties file '"}+_path+"'");
	}

	int flags=tools::text_reader::ltrim | tools::text_reader::rtrim | tools::text_reader::ignorewscomment;
	tools::text_reader reader{_path, '#', flags};
	property_table result;
	const std::string beginprop{"beginproperty"};

	while(true) {

		std::stringstream ss{reader.read_line()};
		if(reader.is_eof()) {
			break;
		}

		std::string tag;
		ss>>std::skipws>>tag;

		if(beginprop != tag) {

			throw std::runtime_error(std::string{"unexpected '"+tag+"', expected beginproperty"});
		}

		read(reader, result);
	}

	return result;
}

void property_parser::read(
	tools::text_reader& _reader, 
	property_table& _table
) {

	std::vector<std::string> propnames={"name", "type", "default", "comment"};
	if(!map_mode) {
		propnames.push_back({"linkedto"});
	}

	auto propmap=generic_first_level(_reader, "endproperty", propnames);

	if(_table.property_exists(propmap["name"])) {

		throw std::runtime_error(std::string{"property '"}+propmap["name"]+"' already exists");
	}

	property_links linked_to=property_links::nothing;
	if(!map_mode) {

		if(propmap["linkedto"]=="w") {

			linked_to=property_links::w;
		}
		else if(propmap["linkedto"]=="h") {

			linked_to=property_links::h;
		}
		else if(propmap["linkedto"]=="colorred") {

			linked_to=property_links::color_red;
		}
		else if(propmap["linkedto"]=="colorgreen") {

			linked_to=property_links::color_green;
		}
		else if(propmap["linkedto"]=="colorblue") {

			linked_to=property_links::color_blue;
		}
		else if(propmap["linkedto"]=="coloralpha") {

			linked_to=property_links::color_alpha;
		}
		else if(propmap["linkedto"]=="nothing") {

			linked_to=property_links::nothing;
		}
		else {

			throw std::runtime_error(std::string{"invalid link type '"+propmap["linkedto"]+"'"});
		}
	}

	if(propmap["type"]=="int") {

		insert(propmap["name"], propmap["default"], propmap["comment"], linked_to, _table.int_properties);
	}
	else if(propmap["type"]=="string") {

		if(linked_to!=property_links::nothing) {

			throw std::runtime_error(std::string{"string property '"}+propmap["name"]+"' cannot be linked");
		}

		insert(propmap["name"], propmap["default"], propmap["comment"], linked_to, _table.string_properties);
	}
	else if(propmap["type"]=="double") {

		if(linked_to!=property_links::nothing) {

			throw std::runtime_error(std::string{"double property '"}+propmap["name"]+"' cannot be linked");
		}

		insert(propmap["name"], propmap["default"], propmap["comment"], linked_to, _table.double_properties);
	}
	else {

		throw std::runtime_error(std::string{"invalid property type '"+propmap["type"]+"', expected int, double or string"});
	}
}

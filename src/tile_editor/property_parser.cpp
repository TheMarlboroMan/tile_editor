#include "property_parser.h"

#include <sstream>

using namespace tile_editor;

property_table property_parser::read_file(const std::string& _path) {

	if(!tools::file_exists(_path)) {

		throw std::runtime_error(std::string{"cannot find properties file '"}+_path+"'");
	}

	int flags=tools::text_reader::ltrim | tools::text_reader::rtrim | tools::text_reader::ignorewscomment;
	tools::text_reader reader{_path, '#', flags};
	property_table result;
	const std::string beginprop{"beginproperty"},

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

	auto propmap=generic_first_level(_reader, "endproperty", {"name", "type", "default", "comment"});

	if(propmap["type"]=="int") {

		//TODO: convert and insert.
	}
	else if(propmap["type"]=="string") {

		//TODO: convert and insert.
	}
	else if(propmap["type"]=="double") {

		//TODO: convert and insert.
	}
	else {

		throw std::runtime_error(std::string{"invalid property type '"+propmap["type"]+"', expected int, double or string"});
	}
}

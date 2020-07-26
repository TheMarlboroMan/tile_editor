#include "parser/thing_parser.h"
#include "parser/parse_tools.h"

#include <tools/file_utils.h>

#include <sstream>

using namespace tile_editor;

thing_definition_table thing_parser::read_file(const std::string& _filename) {

	if(!tools::file_exists(_filename)) {

		throw std::runtime_error(std::string{"cannot find file '"}+_filename+"'");
	}

	const std::string   begin{"beginobject"};

	thing_definition_table result;
	int flags=tools::text_reader::ltrim | tools::text_reader::rtrim | tools::text_reader::ignorewscomment;
	tools::text_reader reader{_filename, '#', flags};
	bool properties_set=false;

	try {
		while(true) {

			std::stringstream ss{reader.read_line()};
			if(reader.is_eof()) {
				break;
			}
			
			std::string tag;
			ss>>std::skipws>>tag;

			if(tag!=begin) {

				throw std::runtime_error(std::string{"unexpected '"+tag+"', expected beginobject"});
			}

			parse_object(reader, result);
		}
	}
	catch(std::exception& e) {

		throw std::runtime_error(
			std::string{e.what()}
			+" on file "+_filename
			+" line "+std::to_string(reader.get_line_number())
		);
	}

	return result;
}

void thing_parser::parse_object(
	tools::text_reader& _reader, 
	thing_definition_table& _result
) {

	//TODO:
	//this will extract ONLY pairs, which will fuck the whole thing.
	//as soon as a property appears... A way to make it better 
	//is to instruct a "skip" pair: skip from x to Y, but these would not
	//get parsed. I don't think we can do much more unless read line
	//by line.
	//our tools are not good enough now...
	auto propmap=generic_first_level(_reader, "endobject", {"id", "name", "w", "h", "size", "color", "beginproperty"});



}

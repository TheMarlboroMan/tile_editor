#include "config_parser.h"
#include "parse_tools.h"

#include <tools/file_utils.h>
#include <tools/string_utils.h>

#include <stdexcept>
#include <fstream>
#include <string>
#include <sstream>

using namespace tile_editor;

map_blueprint config_parser::read(const std::string& _filename) {

	if(!tools::file_exists(_filename)) {

		throw std::runtime_error(std::string{"cannot find file '"}+_filename+"'");
	}

	const std::string   beginprop{"beginmapproperties"},
	                    begintile{"begintileset"},
	                    beginobj{"beginobjectset"};

	map_blueprint mb;
	int flags=tools::text_reader::ltrim | tools::text_reader::rtrim | tools::text_reader::ignorewscomment;
	tools::text_reader reader{_filename, '#', flags};
	bool properties_set=false;

	try {
		while(true) {

			std::stringstream ss{reader.read_line()};
			if(reader.is_eof()) {
				break;
			}
			
			//We can only expect beginmapproperties, begintileset, beginobjectset...
			//Skip all whitespace in the extraction operations that will follow.
			std::string tag;
			ss>>std::skipws>>tag;

			if(tag==beginprop) {

				if(properties_set) {

					throw std::runtime_error("only one mapproperty node can be specified");
				}

				map_property_mode(reader, mb);
				properties_set=true;
			}
			else if(tag==begintile) {

				tile_mode(reader, mb);
			}
			else if(tag==beginobj) {

				thing_mode(reader, mb);
			}
			else {

				throw std::runtime_error(std::string{"unexpected '"+tag+"', expected beginmapproperties, begintileset or beginobjectset"});
			}
		}
	}
	catch(std::exception& e) {

		throw std::runtime_error(
			std::string{e.what()}
			+" on file "+_filename
			+" line "+std::to_string(reader.get_line_number())
		);
	}

	return mb;
}

void config_parser::map_property_mode(
	tools::text_reader& _reader,
	map_blueprint& _blueprint
) {

	auto propmap=generic_first_level(_reader, "endmapproperties", {"file"});

	property_parser pp;
	_blueprint.properties=pp.read_file(propmap["file"]);
}

void config_parser::tile_mode(
	tools::text_reader& _reader,
	map_blueprint& _blueprint
) {

	auto propmap=generic_first_level(_reader, "endtileset", {"file", "id", "image"});

	//TODO: check the types...
	//TODO: add to caché.
}

void config_parser::thing_mode(
	tools::text_reader& _reader,
	map_blueprint& _blueprint
) {

	auto propmap=generic_first_level(_reader, "endobjectset", {"file", "id"});
	//TODO: check the types...
	//TODO: add to caché.
}



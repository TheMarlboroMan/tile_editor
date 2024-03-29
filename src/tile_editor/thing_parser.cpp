#include "parser/thing_parser.h"
#include "parser/parse_tools.h"
#include "parser/property_parser.h"
#include "blueprint_types/property_table.h"

#include <tools/file_utils.h>

#include <sstream>
#include <map>

using namespace tile_editor;

thing_definition_table::table_type thing_parser::read_file(const std::string& _filename) {

	if(!tools::filesystem::exists(_filename)) {

		throw std::runtime_error(std::string{"cannot find file '"}+_filename+"'");
	}

	const std::string   begin{"beginobject"};

	thing_definition_table::table_type result;
	int flags=tools::text_reader::ltrim | tools::text_reader::rtrim | tools::text_reader::ignorewscomment;
	tools::text_reader reader{_filename, '#', flags};

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
	thing_definition_table::table_type& _result
) {

	//Given the nested structure of this file, we cannot use exactly the same
	//tools as we used in other parsers. It is, however, not that different.

	const std::string   end{"endobject"},
	                    propstart{"beginproperty"};

	property_table pt;
	std::map<std::string, std::string> properties{
		{"id", ""},
		{"name", ""},
		{"w", ""},
		{"h", ""},
		{"color", ""}
	};

	while(true) {

		const auto pair=from_reader(_reader);

		if(pair.eof) {

			throw std::runtime_error{"unexpected file end before 'endobject'"};
		}

		if(pair.name==propstart) {

			property_parser pp(false);
			pp.read(_reader, pt);
			continue;
		}

		if(pair.name==end) {

			break;
		}

		if(pair.failed) {

			throw std::runtime_error(std::string{"missing property value for '"}+pair.name+"'");
		}

		if(!properties.count(pair.name)) {

			throw std::runtime_error(std::string{"unknown property name '"}+pair.name+"'");
		}

		if(properties[pair.name].size()) {

			throw std::runtime_error(std::string{"repeated property '"}+pair.name+"'");
		}

		properties[pair.name]=pair.value;
	}

	for(const auto& pair : properties) {

		if(!pair.second.size()) {

			throw std::runtime_error{std::string{"missing property '"}+pair.first+"'"};
		}
	}

	std::size_t id=convert_value<std::size_t>(properties, "id");
	if(_result.count(id)) {

		throw std::runtime_error("repeated thing definition id");
	}

	int w=convert_value<int>(properties, "w"),
		h=convert_value<int>(properties, "h");

	auto color=parse_color(properties["color"]);

	_result[id]={
		id,
		w,
		h,
		properties["name"],
		color,
		pt
	};
}

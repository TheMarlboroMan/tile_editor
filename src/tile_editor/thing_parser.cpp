#include "parser/thing_parser.h"
#include "parser/parse_tools.h"
#include "parser/property_parser.h"
#include "blueprint_types/property_table.h"

#include <tools/file_utils.h>

#include <sstream>
#include <map>

using namespace tile_editor;

thing_definition_table thing_parser::read_file(const std::string& _filename) {

	if(!tools::file_exists(_filename)) {

		throw std::runtime_error(std::string{"cannot find file '"}+_filename+"'");
	}

	const std::string   begin{"beginobject"};

	thing_definition_table result;
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
	thing_definition_table& _result
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
		{"size", ""},
		{"color", ""}
	};

	while(true) {

		const auto pair=from_reader(_reader);

		if(pair.eof) {

			throw std::runtime_error{"unexpected file end"};
		}

		if(pair.name==propstart) {

			property_parser pp;
			pp.read(_reader, pt);
			continue;
		}

		if(pair.name==end) {

			break;
		}

		if(pair.failed) {

			throw std::runtime_error("missing property value");
		}

		if(!properties.count(pair.name)) {

			throw std::runtime_error("unknown property name");
		}

		if(properties[pair.name].size()) {

			throw std::runtime_error("repeated property");
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

	thing_definition::size_type size=thing_definition::size_type::fixed;
	if(properties["size"]=="fixed") {

		//Noop
	}
	else if(properties["size"]=="resizable") {

		size=thing_definition::size_type::resizable;
	}
	else {

		throw std::runtime_error("invalid size type, valid values are 'fixed' and 'resizable'");
	}

	int r{}, g{}, b{};
	std::stringstream ss{properties["color"]};
	ss>>r>>g>>b;

	if(ss.fail()) {
	
		throw std::runtime_error("invalid color schema, values are red, green and blue separated by spaces");
	}

	_result[id]={
		id,
		w, 
		h,
		size,
		properties["name"],
		{r,g,b},
		pt
	};
}

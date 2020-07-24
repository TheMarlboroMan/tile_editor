#include "config_file_parser.h"

#include <tools/file_utils.h>
#include <tools/string_utils.h>

#include <stdexcept>
#include <fstream>
#include <string>
#include <sstream>

//TODO: remove!!!
#include <iostream>

using namespace tile_editor;

map_blueprint config_file_parser::read(const std::string& _filename) {

	if(!tools::file_exists(_filename)) {

		throw std::runtime_error(std::string{"cannot find file '"}+_filename+"'");
	}

	const std::string   beginprop{"beginmapproperties"},
	                    begintile{"begintileset"},
	                    beginobj{"beginobjectset"};

	map_blueprint mb;
	int flags=tools::text_reader::ltrim | tools::text_reader::rtrim | tools::text_reader::ignorewscomment;
	tools::text_reader reader{_filename, '#', flags};

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

				map_property_mode(reader);
			}
			else if(tag==begintile) {

				tile_mode(reader);
			}
			else if(tag==beginobj) {

				thing_mode(reader);
			}
			else {

				throw std::runtime_error(std::string{"unexpected '"+tag+"'"});
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

void config_file_parser::map_property_mode(tools::text_reader& _reader) {

	std::string line;
	const std::string   end{"endmapproperties"},
	                    file{"file"};

	auto filepair=from_reader(_reader);
	if(filepair.eof) {
		throw std::runtime_error("unexpected end of file, expected 'file'");
	}

	if(filepair.name!=file) {
	
		throw std::runtime_error(std::string{"unexpected '"}+filepair.name+"', expected 'file'");
	}

	if(filepair.failed) {

		throw std::runtime_error("syntax error: file #filename#");
	}

	auto endpair=from_reader(_reader);
	if(endpair.eof) {

		throw std::runtime_error("unexpected end of file, expected 'endmapproperties'");
	}

	if(endpair.name!=end) {
	
		throw std::runtime_error(std::string{"unexpected '"}+endpair.name+"', expected 'endmapproperties'");
	}
}

void config_file_parser::tile_mode(tools::text_reader& _reader) {

	const std::string   end{"endtileset"},
	                    file{"file"},
	                    id{"id"},
	                    image{"image"};

	std::string fileval,
	            idval,
	            imgval;

	std::vector<std::string> appeared;

	while(true) {

		auto pair=from_reader(_reader, {end, file, id, image});

		if(pair.eof) {

			throw std::runtime_error("unexpected end of file, expected 'endtileset'");
		}

		if(pair.name==end) {

			break;
		}

		if(pair.disallowed) {

			throw std::runtime_error{std::string{"unexpected '"}+pair.name+"'"};
		}

		if(pair.failed) {

			throw std::runtime_error("syntax error: expected property value");
		}

		if(appeared.end() != std::find(std::begin(appeared), std::end(appeared), pair.name)) {

			throw std::runtime_error("invalid repeated name");
		}

		appeared.push_back(pair.name);

		if(pair.name==file) {

			fileval=pair.value;
		}
		else if(pair.name==id) {

			idval=pair.value;
		}
		else if(pair.name==image) {

			imgval=pair.value;
		}
	}

	if(!fileval.size()) {

		throw new std::runtime_error("missing 'file'");
	}
	
	if(!idval.size()) {

		throw new std::runtime_error("missing 'id'");
	}

	if(!imgval.size()) {

		throw new std::runtime_error("missing 'image'");
	}

	//TODO: check the types...
	//TODO: add to caché.
}

void config_file_parser::thing_mode(tools::text_reader& _reader) {

	const std::string   end{"endobjectset"};

	while(true) {

		std::stringstream ss{_reader.read_line()};

		if(_reader.is_eof()) {
			throw std::runtime_error("unexpected end of file, expected 'endobjectset'");
		}

		std::string tag;
		ss>>std::skipws>>tag;

		if(tag==end) {

			//TODO: Check all data has been set.
			//TODO: add to caché.
			return;
		}
	}
}

config_file_parser::config_pair config_file_parser::from_reader(
	tools::text_reader& _reader,
	const std::vector<std::string>& _allowed) {

	std::stringstream ss{_reader.read_line()};
	config_pair result;

	if(_reader.is_eof()) {
	
		result.eof=true;
		return result;
	}

	ss>>std::skipws>>result.name;
	result.value=ss.str();

	if(_allowed.end()!=std::find(std::begin(_allowed), std::end(_allowed), result.value)) {

		result.disallowed=true;
	}

	if(ss.fail()) {
		result.failed=true;
	}

	return result;
}

config_file_parser::config_pair config_file_parser::from_reader(
	tools::text_reader& _reader
) {

	config_pair result;

	std::stringstream ss{_reader.read_line()};

	if(_reader.is_eof()) {
	
		result.eof=true;
		return result;
	}

	ss>>std::skipws>>result.name;
	result.value=ss.str();

	if(ss.fail()) {
		result.failed=true;
	}

	return result;
}

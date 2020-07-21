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

		throw std::runtime_error(std::string{"cannot find config file '"}+_filename+"'");
	}

	std::ifstream file{_filename};
	std::string line;
	const std::string   beginprop{"beginmapproperties"},
	                    begintile{"begintileset"},
	                    beginobj{"beginobjectset"};

	bool eof=false;

	while(true) {

		std::stringstream ss{preprocess_line(file, eof)};

		if(eof) {
			break;
		}

		//We can only expect beginmapproperties, begintileset, beginobjectset...
		//Skip all whitespace in the extraction operations that will follow.
		std::string tag;
		ss>>std::skipws>>tag;

		if(tag==beginprop) {

			property_mode(file);
		}
		else if(tag==begintile) {

			tile_mode(file);
		}
		else if(tag==beginobj) {

			thing_mode(file);
		}
		else {

			throw std::runtime_error(std::string{"unexpected '"+tag+"' on config file"});
		}
	}

	return map_blueprint{};
}

std::string config_file_parser::preprocess_line(std::ifstream& _file, bool& _eof) const {

	while(true) {

		std::string line;
		std::getline(_file, line);
		if(_file.eof()) {
			_eof=true;
			return "";
		}

		//Is it empty=
		tools::trim(line);
		if(!line.size()) {
			continue;
		}

		//Is it a comment???
		if('#'==line[0]) {
			continue;
		}

		return line;
	}
}

void config_file_parser::property_mode(std::ifstream& _file) {

	std::string line;
	const std::string   end{"endmapproperties"};
	bool eof=false;

	while(true) {

		std::stringstream ss{preprocess_line(_file, eof)};

		if(eof) {

			throw std::runtime_error("unexpected end of file, expected 'endmapproperties'");
		}

		std::string tag;
		ss>>std::skipws>>tag;

		//TODO: use some sort of pair that reads "key" (first word) and "value"
		//(the rest) to extract shit. Then check keys and values here.

		if(tag==end) {

			//TODO: Check all data has been set.
			//TODO: add to caché.
			return;
		}
	}
}

void config_file_parser::tile_mode(std::ifstream& _file) {

	const std::string   end{"endtileset"};
	bool eof=false;

	while(true) {

		std::stringstream ss{preprocess_line(_file, eof)};

		if(eof) {
			throw std::runtime_error("unexpected end of file, expected 'endtileset'");
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

void config_file_parser::thing_mode(std::ifstream& _file) {

	const std::string   end{"endobjectset"};
	bool eof=false;

	while(true) {

		std::stringstream ss{preprocess_line(_file, eof)};

		if(eof) {
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
/*
This file test the configuration file parsers.
*/

#include "parser/config_parser.h"

#include <iostream>
#include <stdexcept>
#include <map>

void fail(const std::string& _msg);
void assert(bool _thing, const std::string& _msg);
void check_thing(const tile_editor::thing_definition_table&, std::size_t, const std::string&, int, int, const std::string&, int, int, int, std::size_t);
template<typename T> void must_throw(
	T _whatever, 
	const std::string& _errmsg,
	const std::string& _type
) {

	try {
		_whatever();
		std::cerr<<"'"<<_type<<"' should have failed..."<<std::endl;
		std::exit(1);
	}
	catch(std::exception &e) {

		if(std::string{e.what()}.find(_errmsg)==std::string::npos) {

			std::cerr<<"expected '"<<_errmsg<<"', got '"<<e.what()<<"'"<<std::endl;
			std::exit(1);
		}
	}
}

//Generic property content checks.
template<typename T, typename V>
void check_property_values(
	const T& _property, 
	const std::string& _name,
	const std::string& _comment,
	const V& _default
) {

	assert(_property.name==_name, _name+" does not match property name");
	assert(_property.description==_comment, _comment+" does not match property description");
	assert(_property.default_value==_default, "property default value does not match");
}

template<typename T> struct can_be_property {static const bool value=false;};
template<> struct can_be_property<int>{static const bool value=true;};
template<> struct can_be_property<double>{static const bool value=true;};
template<> struct can_be_property<std::string>{static const bool value=true;};

//Generic property check, will fail.
template<typename T> 
void check_property(
	const tile_editor::property_table&, 
	const std::string&, 
	T, 
	const std::string&
) {

	static_assert(can_be_property<T>::value, "invalid check_property template was instantiated");
}

//Int property check.
template<> void check_property(
	const tile_editor::property_table& _properties, 
	const std::string& _name, 
	int _default, 
	const std::string& _comment
) {

	assert(1==_properties.int_properties.count(_name), _name+" is undefined as int property");
	check_property_values(_properties.int_properties.at(_name), _name, _comment, _default);
}

//Double property check.
template<> void check_property(
	const tile_editor::property_table& _properties, 
	const std::string& _name, 
	double _default, 
	const std::string& _comment
) {

	assert(1==_properties.double_properties.count(_name), _name+" is undefined as double property");
	check_property_values(_properties.double_properties.at(_name), _name, _comment, _default);
}

//String property check.
template<> void check_property(
	const tile_editor::property_table& _properties, 
	const std::string& _name, 
	const std::string& _default, 
	const std::string& _comment
) {

	assert(1==_properties.string_properties.count(_name), _name+" is undefined as string property");
	check_property_values(_properties.string_properties.at(_name), _name, _comment, _default);
}

int main(int /*argc*/, char ** /*argv*/) {

	std::cout<<"testing complete valid config file"<<std::endl;
	tile_editor::config_parser cfp;
	auto blueprint=cfp.read("data/good_config.txt");

	try {

		//Tilesets are easy to assert: we very much know they work by their own 
		//tests. We need only to check a few values...
		std::cout<<"testing tileset contents..."<<std::endl;

		assert(2==blueprint.tilesets.size(), "failed to assert that there are 2 tilesets");

		assert(1==blueprint.tilesets.count(1), "failed to assert that tileset 1 exists");
		assert(3==blueprint.tilesets[1].table.size(), "failed to assert that tileset 1 has 3 tiles");
		assert(64==blueprint.tilesets[1].table.get(3).x, "failed to assert contents of tileset 1");

		assert(1==blueprint.tilesets.count(2), "failed to assert that tileset 2 exists");
		assert(12==blueprint.tilesets[2].table.size(), "failed to assert that tileset 2 has 12 tiles");
		assert(32==blueprint.tilesets[2].table.get(11).y, "failed to assert contents of tileset 2");

		//Thingsets must be thoroughly tested...
		std::cout<<"testing thingset contents..."<<std::endl;
		assert(3==blueprint.thingsets.size(), "failed to assert that there are 3 thingsets");

		assert(4==blueprint.thingsets[1].size(), "failed to assert item count on thingset 1");
		check_thing(blueprint.thingsets[1], 1, "extra_life", 16, 16, "fixed", 255, 255, 0, 0);
		check_thing(blueprint.thingsets[1], 2, "health", 16, 16, "fixed", 0, 255, 255, 0);
		check_thing(blueprint.thingsets[1], 3, "enemy", 32, 16, "fixed", 255, 255, 128, 1);
		check_thing(blueprint.thingsets[1], 4, "friend", 32, 16, "fixed", 255, 200, 128, 4);

		check_property<int>(blueprint.thingsets[1][3].properties, "type_id", 1, "Enemy type id");

		check_property<int>(blueprint.thingsets[1][4].properties, "type_id", 1, "Friend type id");
		check_property<int>(blueprint.thingsets[1][4].properties, "health", 100, "Basic friend health level");
		check_property<std::string>(blueprint.thingsets[1][4].properties, "name", "Unnamed", "Friend name to be displayed");
		check_property<double>(blueprint.thingsets[1][4].properties, "factor", 2.5, "Movement factor");

		assert(2==blueprint.thingsets[2].size(), "failed to assert item count on thingset 2");
		check_thing(blueprint.thingsets[2], 1, "start", 32, 32, "fixed", 0, 255, 0, 2);
		check_thing(blueprint.thingsets[2], 2, "exit", 32, 32, "resizable", 0, 0, 255, 2);

		check_property<int>(blueprint.thingsets[2][1].properties, "id", 0, "Unique id for the start");
		check_property<int>(blueprint.thingsets[2][1].properties, "bearing", 90, "Exit bearing, 0 points right, 90 up.");

		check_property<int>(blueprint.thingsets[2][2].properties, "map_id", 0, "Destination map id");
		check_property<int>(blueprint.thingsets[2][2].properties, "start_id", 0, "Start id on the destination map");

		assert(1==blueprint.thingsets[3].size(), "failed to assert item count on thingset 3");
		check_thing(blueprint.thingsets[3], 33, "Touch trigger", 64, 64, "resizable", 255, 0, 255, 0);

		//Same goes for map properties
		std::cout<<"testing map property contents..."<<std::endl;
		assert(3==blueprint.properties.size(), "failed to assert map property count");
		check_property<std::string>(blueprint.properties, "name", "unnamed-map", "The name of the map");
		check_property<int>(blueprint.properties, "map_id", 0, "An unique identifier for the map");
		check_property<int>(blueprint.properties, "special_effect_flags", 0, "Special effects, 0 means none, 1 means darkened, 2 means underwater.");
	}
	catch(std::exception& e) {

		std::cerr<<"failed: "<<e.what()<<std::endl;
		std::exit(1);
	}

	std::cout<<"testing invalid configuration files..."<<std::endl;

	//config parser with non existing file.
	must_throw([&cfp](){cfp.read("data/this-is-not-a-file");}, "cannot find file", "config parser with non existing file");

	//config parser with invalid opening tag

	//config parser without file value in property set

	//config parser with invalid map property definition

	//config parser with repeated property definition

	//config parser with two map property sets.

	//config parser with unclosed property set.

	//config parser with invalid map property file

	//config parser with invalid tileset definition

	//config parser with unclosed tileset definition

	//config parser with invalid tileset id

	//config parser with repeated tileset id

	//config parser with repeated tileset property definition

	//config parser with invalid thingset definition

	//config parser with unclosed thingset definition

	//config parser with invalid thingset id

	//config parser with repeated thingset id

	//config parser with repeated thingset property definition

	//unclosed property definition

	//malformed property definition, missing parameters

	//malformed property definition, missing values

	//malformed property definition, invalid types

	//unclosed thing definition

	//malformed thing definition, missing parameters

	//malformed thing definition, missing values

	//malformed thing definition, malformed size

	//malformed thing definition, malformed color

	//malformed thing definition, repeated id

	//malformed thing definition, repeated property

	std::cout<<"done"<<std::endl;

	return 0;
}

void fail(const std::string& _msg) {

	throw std::runtime_error(_msg);
}

void assert(bool _thing, const std::string& _msg) {

	if(!_thing) {
		fail(_msg);
	}
}

void check_thing(
	const tile_editor::thing_definition_table& _things, 
	size_t _index, 
	const std::string& _name, 
	int _w, 
	int _h, 
	const std::string& _sizetype, 
	int _r, 
	int _g, 
	int _b, 
	std::size_t _propcount
) {

	assert(1==_things.count(_index), std::string{"undefined thing index '"}+std::to_string(_index)+"'");
	
	const auto& thing=_things.at(_index);
	assert(_name==thing.name, _name+"does not match thing name");
	assert(_w==thing.w, std::to_string(_w)+" does not match thing width");
	assert(_h==thing.h, std::to_string(_h)+" does not match thing height");

	if(_sizetype=="fixed") {

		assert(tile_editor::thing_definition::size_type::fixed==thing.sizetype, "mismatched size type");
	}
	else if(_sizetype=="resizable") {

		assert(tile_editor::thing_definition::size_type::resizable==thing.sizetype, "mismatched size type");
	}
	else {

		fail("invalid sizetype");
	}
	
	assert(_r==thing.color.r, std::to_string(_r)+" does not match thing red");
	assert(_g==thing.color.g, std::to_string(_g)+" does not match thing green");
	assert(_b==thing.color.b, std::to_string(_b)+" does not match thing blue");
	assert(_propcount==thing.properties.size(), "propsize does not match");
}


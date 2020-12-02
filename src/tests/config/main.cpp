/*
This file test the configuration file parsers.
*/

#include "parser/blueprint_parser.h"
#include "../tests.h"

#include <iostream>
#include <stdexcept>
#include <map>

void check_thing(const tile_editor::thing_definition_table&, std::size_t, const std::string&, int, int, int, int, int, int, std::size_t);
void check_poly(const tile_editor::poly_definition_table&, std::size_t, const std::string&, int, int, int, int, std::size_t);

//Generic property content checks.
template<typename T, typename V>
void check_property_values(
	const T& _property,
	const std::string& _name,
	const std::string& _comment,
	const V& _default,
	const std::string& _link
) {

	assert(_property.name==_name, _name+" does not match property name");
	assert(_property.description==_comment, _comment+" does not match property description");
	assert(_property.default_value==_default, std::string{"property default value does not match for '"}+_name+"'");

	tile_editor::property_links linktype{tile_editor::property_links::nothing};

	if(_link=="nothing") {

		linktype=tile_editor::property_links::nothing;
	}
	else if(_link=="w") {

		linktype=tile_editor::property_links::w;
	}
	else if(_link=="h") {

		linktype=tile_editor::property_links::h;
	}
	else if(_link=="color_red") {

		linktype=tile_editor::property_links::color_red;
	}
	else if(_link=="color_green") {

		linktype=tile_editor::property_links::color_green;
	}
	else if(_link=="color_blue") {

		linktype=tile_editor::property_links::color_blue;
	}
	else if(_link=="color_alpha") {

		linktype=tile_editor::property_links::color_alpha;
	}
	else {

		assert(false, "invalid colortype in test");
	}

	assert(linktype==_property.linked_to, "mismatched link type");
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
	const std::string&,
	const std::string&
) {

	static_assert(can_be_property<T>::value, "invalid check_property template was instantiated");
}

//Int property check.
template<> void check_property(
	const tile_editor::property_table& _properties,
	const std::string& _name,
	int _default,
	const std::string& _comment,
	const std::string& _link
) {

	assert(1==_properties.int_properties.count(_name), _name+" is undefined as int property");
	check_property_values(_properties.int_properties.at(_name), _name, _comment, _default, _link);
}

//Double property check.
template<> void check_property(
	const tile_editor::property_table& _properties,
	const std::string& _name,
	double _default,
	const std::string& _comment,
	const std::string& _link
) {

	assert(1==_properties.double_properties.count(_name), _name+" is undefined as double property");
	check_property_values(_properties.double_properties.at(_name), _name, _comment, _default, _link);
}

//String property check.
template<> void check_property(
	const tile_editor::property_table& _properties,
	const std::string& _name,
	const std::string& _default,
	const std::string& _comment,
	const std::string& _link
) {

	assert(1==_properties.string_properties.count(_name), _name+" is undefined as string property");
	check_property_values(_properties.string_properties.at(_name), _name, _comment, _default, _link);
}

int main(int /*argc*/, char ** /*argv*/) {

	std::cout<<"testing complete valid config file"<<std::endl;
	tile_editor::blueprint_parser cfp;

	try {

		auto blueprint=cfp.parse_file("data/good_config.txt");

		//Tilesets are easy to assert: we very much know they work by their own
		//tests. We need only to check a few values...
		std::cout<<"testing tileset contents..."<<std::endl;

		assert(2==blueprint.tilesets.size(), "failed to assert that there are 2 tilesets");

		assert(1==blueprint.tilesets.count(1), "failed to assert that tileset 1 exists");
		assert("tileset 1"==blueprint.tilesets[1].name, "failed to assert that tileset 1 has the given name");
		assert(3==blueprint.tilesets[1].table.size(), "failed to assert that tileset 1 has 3 tiles");
		assert(64==blueprint.tilesets[1].table.get(3).x, "failed to assert contents of tileset 1");

		assert(1==blueprint.tilesets.count(2), "failed to assert that tileset 2 exists");
		assert("tileset 2"==blueprint.tilesets[2].name, "failed to assert that tileset 2 has the given name");
		assert(12==blueprint.tilesets[2].table.size(), "failed to assert that tileset 2 has 12 tiles");
		assert(32==blueprint.tilesets[2].table.get(11).y, "failed to assert contents of tileset 2");

		//Thingsets must be thoroughly tested...
		std::cout<<"testing thingset contents..."<<std::endl;
		assert(3==blueprint.thingsets.size(), "failed to assert that there are 3 thingsets");

		assert(4==blueprint.thingsets[1].table.size(), "failed to assert item count on thingset 1");
		assert("thingset 1"==blueprint.thingsets[1].name, "failed to assert that thingset 1 has the given name");
		check_thing(blueprint.thingsets[1], 1, "extra_life", 16, 16, 255, 255, 0, 0, 0);
		check_thing(blueprint.thingsets[1], 2, "health", 16, 16, 0, 255, 255, 0, 0);
		check_thing(blueprint.thingsets[1], 3, "enemy", 32, 16, 255, 255, 128, 0, 1);
		check_thing(blueprint.thingsets[1], 4, "friend", 32, 16, 255, 200, 128, 0, 4);

		check_property<int>(blueprint.thingsets[1].table[3].properties, "type_id", 1, "Enemy type id", "nothing");

		check_property<int>(blueprint.thingsets[1].table[4].properties, "type_id", 1, "Friend type id", "nothing");
		check_property<int>(blueprint.thingsets[1].table[4].properties, "health", 100, "Basic friend health level", "nothing");
		check_property<std::string>(blueprint.thingsets[1].table[4].properties, "name", "Unnamed", "Friend name to be displayed", "nothing");
		check_property<double>(blueprint.thingsets[1].table[4].properties, "factor", 2.5, "Movement factor", "nothing");

		assert(2==blueprint.thingsets[2].table.size(), "failed to assert item count on thingset 2");
		assert("thingset 2"==blueprint.thingsets[2].name, "failed to assert that thingset 2 has the given name");
		check_thing(blueprint.thingsets[2], 1, "start", 32, 32, 0, 255, 0, 0, 2);
		check_thing(blueprint.thingsets[2], 2, "exit", 32, 32, 0, 0, 255, 0, 4);

		check_property<int>(blueprint.thingsets[2].table[1].properties, "id", 0, "Unique id for the start", "nothing");
		check_property<int>(blueprint.thingsets[2].table[1].properties, "bearing", 90, "Exit bearing, 0 points right, 90 up.", "nothing");

		check_property<int>(blueprint.thingsets[2].table[2].properties, "map_id", 0, "Destination map id", "nothing");
		check_property<int>(blueprint.thingsets[2].table[2].properties, "start_id", 0, "Start id on the destination map", "nothing");
		check_property<int>(blueprint.thingsets[2].table[2].properties, "w", 64, "Object width", "w");
		check_property<int>(blueprint.thingsets[2].table[2].properties, "h", 64, "Object height", "h");

		assert(1==blueprint.thingsets[3].table.size(), "failed to assert item count on thingset 3");
		assert("thingset 3"==blueprint.thingsets[3].name, "failed to assert that thingset 3 has the given name");
		check_thing(blueprint.thingsets[3], 33, "Touch trigger", 64, 64, 255, 0, 255, 0, 2);
		check_property<int>(blueprint.thingsets[3].table[33].properties, "w", 64, "Object width", "w");
		check_property<int>(blueprint.thingsets[3].table[33].properties, "h", 64, "Object height", "h");

		//as do poly sets...
		std::cout<<"testing polyset contents..."<<std::endl;
		assert(2==blueprint.polysets.size(), "failed to assert that there are 2 polysets");

		assert(1==blueprint.polysets.count(1), "failed to assert that polyset 1 exists");
		assert(2==blueprint.polysets[1].table.size(), "failed to assert size of polyset 1");
		assert("polyset 1"==blueprint.polysets[1].name, "failed to assert that polyset 1 has the given name");
		check_poly(blueprint.polysets[1], 1, "collisionable", 255, 255, 0, 0, 0);
		check_poly(blueprint.polysets[1], 2, "touch_trigger", 255, 255, 255, 128, 6);
		check_property<int>(blueprint.polysets[1].table[2].properties, "trigger_id", 1, "Trigger id", "nothing");
		check_property<int>(blueprint.polysets[1].table[2].properties, "repeatable", -1, "-1 for infinite, a number for the exact number of times", "nothing");
		check_property<int>(blueprint.polysets[1].table[2].properties, "colorred", 255, "Red channel", "color_red");
		check_property<int>(blueprint.polysets[1].table[2].properties, "colorgreen", 255, "Green channel", "color_green");
		check_property<int>(blueprint.polysets[1].table[2].properties, "colorblue", 255, "Blue channel", "color_blue");
		check_property<int>(blueprint.polysets[1].table[2].properties, "coloralpha", 128, "Alpha channel", "color_alpha");

		assert(1==blueprint.polysets.count(2), "failed to assert that polyset 2 exists");
		assert(2==blueprint.polysets[2].table.size(), "failed to assert size of polyset 2");
		assert("polyset 2"==blueprint.polysets[2].name, "failed to assert that polyset 2 has the given name");
		check_poly(blueprint.polysets[2], 1, "background", 255, 0, 0, 0, 4);
		check_property<int>(blueprint.polysets[2].table[1].properties, "colorred", 128, "Red channel", "color_red");
		check_property<int>(blueprint.polysets[2].table[1].properties, "colorgreen", 128, "Green channel", "color_green");
		check_property<int>(blueprint.polysets[2].table[1].properties, "colorblue", 128, "Blue channel", "color_blue");
		check_property<int>(blueprint.polysets[2].table[1].properties, "coloralpha", 0, "Alpha channel", "color_alpha");

		check_poly(blueprint.polysets[2], 2, "foreground", 0, 255, 0, 0, 4);
		check_property<int>(blueprint.polysets[2].table[2].properties, "colorred", 255, "Red channel", "color_red");
		check_property<int>(blueprint.polysets[2].table[2].properties, "colorgreen", 255, "Green channel", "color_green");
		check_property<int>(blueprint.polysets[2].table[2].properties, "colorblue", 255, "Blue channel", "color_blue");
		check_property<int>(blueprint.polysets[2].table[2].properties, "coloralpha", 0, "Alpha channel", "color_alpha");

		//Same goes for map properties
		std::cout<<"testing map property contents..."<<std::endl;
		assert(3==blueprint.properties.size(), "failed to assert map property count");
		check_property<std::string>(blueprint.properties, "name", "unnamed-map", "The name of the map", "nothing");
		check_property<int>(blueprint.properties, "map_id", 0, "An unique identifier for the map", "nothing");
		check_property<int>(blueprint.properties, "special_effect_flags", 0, "Special effects, 0 means none, 1 means darkened, 2 means underwater.", "nothing");

		std::cout<<"testing session data..."<<std::endl;
		assert(tile_editor::map_blueprint::thing_centers::top_right==blueprint.thing_center, "failed to assert center type");
		assert(1==blueprint.bg_color.r, "failed to assert red component of background color");
		assert(2==blueprint.bg_color.g, "failed to assert green component of background color");
		assert(32==blueprint.bg_color.b, "failed to assert blue component of background color");
		assert(0==blueprint.bg_color.a, "failed to assert alpha component of background color");
		assert(4==blueprint.grid_data.vertical_ruler, "failed to assert grid vertical ruler");
		assert(6==blueprint.grid_data.horizontal_ruler, "failed to assert grid horizontal ruler");
		assert(2==blueprint.grid_data.color.r, "failed to assert red component of grid color");
		assert(32==blueprint.grid_data.color.g, "failed to assert green component of grid color");
		assert(1==blueprint.grid_data.color.b, "failed to assert blue component of grid color");
		assert(0==blueprint.grid_data.color.a, "failed to assert alpha component of grid color");
		assert(32==blueprint.grid_data.ruler_color.r, "failed to assert red component of grid ruler color");
		assert(2==blueprint.grid_data.ruler_color.g, "failed to assert green component of grid ruler color");
		assert(1==blueprint.grid_data.ruler_color.b, "failed to assert blue component of grid ruler color");
		assert(0==blueprint.grid_data.ruler_color.a, "failed to assert alpha component of grid ruler color");

		//Assert that session data properties are not compulsory
		assert(32==blueprint.grid_data.size, "failed to assert grid size");
	}
	catch(std::exception& e) {

		std::cerr<<"failed: "<<e.what()<<std::endl;
		std::exit(1);
	}

	std::cout<<"testing invalid configuration files..."<<std::endl;

	must_throw([&cfp](){cfp.parse_file("data/this-is-not-a-file");}, "cannot find file", "config parser with non existing file");

	std::string contents=R"str(
beginlelproperties
file data/good_map_properties.txt
endmapproperties
)str";

	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "expected beginmapproperties, begintileset or beginobjectset", "config parser with invalid opening tag");

	contents=R"str(
beginmapproperties
#there should be a file property here.
endmapproperties
)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "missing value for 'file'", "config parser without file value in property set");

	contents=R"str(
beginmapproperties
file data/bad-003-b.txt
endmapproperties
)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unrecognised 'nothing'", "config parser with invalid map property definition");

	contents=R"str(
beginmapproperties
file data/bad-051-b.txt
endmapproperties
)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unrecognised 'linkedto'", "Config parser with linked property.");

	contents=R"str(
beginmapproperties
file data/bad-004-b.txt
endmapproperties
)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "repeated property 'default'", "config parser with repeated property definition");

	contents=R"str(
beginmapproperties
file data/good_map_properties.txt
endmapproperties

beginmapproperties
file data/good_map_properties.txt
endmapproperties
)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "only one mapproperty node can be specified", "config parser with two map property sets");

	contents=R"str(
beginmapproperties
file data/good_map_properties.txt
#ouch

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unexpected end of file before 'endmapproperties'", "config parser with unclosed property set");

	contents=R"str(
beginmapproperties
file data/not-really-a-file
endmapproperties

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "cannot find properties file 'data/not-really-a-file'", "config parser with invalid map property file");

	contents=R"str(
begintileset
	file data/good_tileset_01.txt
	id 1
#	image ouch.png
endtileset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "missing value for 'image'", "config parser with invalid tileset definition");

	contents=R"str(
begintileset
	file data/good_tileset_01.txt
	id 1
	image ouch.png
#   name lol
endtileset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "missing value for 'name'", "config parser with invalid tileset definition, no name");


	contents=R"str(
begintileset
	file data/good_tileset_01.txt
	id 1
	image ouch.png

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unexpected end of file before 'endtileset'", "config parser with unclosed tileset definition");

	contents=R"str(
begintileset
	file data/good_tileset_01.txt
	id ouch
	name lol
	image ouch.png
endtileset
)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "invalid id value", "config parser with invalid tileset id");

	contents=R"str(
begintileset
	file data/good_tileset_01.txt
	id 1
	image ouch.png
	name lol
endtileset

begintileset
	file data/good_tileset_01.txt
	id 1
	image ouch.png
	name lol
endtileset



)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "repeated id value", "config parser with repeated tileset id");

	contents=R"str(
begintileset
	file data/good_tileset_01.txt
	id 1
	image ouch.png
	file data/good_tileset_01.txt
endtileset
)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "repeated property 'file'", "config parser with repeated tileset property definition");

	contents=R"str(
beginobjectset
	file data/good_objectset_01.txt
#	id 1
endobjectset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "missing value for 'id'", "config parser with invalid thingset definition");

	contents=R"str(
beginobjectset
	file data/good_objectset_01.txt
	id 1
	#name lol
endobjectset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "missing value for 'name'", "config parser with invalid thingset definition, no name");


	contents=R"str(
beginobjectset
	file data/good_objectset_01.txt
	id 1

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unexpected end of file before 'endobjectset'", "config parser with unclosed thingset definition");

	contents=R"str(
beginobjectset
	file data/good_objectset_01.txt
	id oops
	name lol
endobjectset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "invalid id value", "config parser with invalid thingset id");

	contents=R"str(
beginobjectset
	file data/good_objectset_01.txt
	id 1
	name lol
endobjectset

beginobjectset
	file data/good_objectset_01.txt
	id 1
	name lol
endobjectset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "repeated id value", "config parser with repeated thingset id");

	contents=R"str(
beginobjectset
	file data/good_objectset_01.txt
	id 1
	id 1
	name lol
endobjectset
)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "repeated property 'id'", "config parser with repeated thingset property definition");

	contents=R"str(
beginobjectset
	file data/bad-018-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unexpected file end before 'endobject'", "unclosed thing definition");

	contents=R"str(
beginobjectset
	file data/bad-019-b.txt
	id 1
	name lol
endobjectset
)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "missing property 'color'", "malformed thing definition, missing color parameter");

	contents=R"str(
beginobjectset
	file data/bad-020-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "missing property value for 'h'", "malformed thing definition, missing values");

	contents=R"str(
beginobjectset
	file data/bad-021-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unknown property name 'ouch'", "malformed thing definition, unknown property");

	contents=R"str(
beginobjectset
	file data/bad-023-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "invalid color schema, values are red, green, blue and alpha", "malformed thing definition, malformed color");

	contents=R"str(
beginobjectset
	file data/bad-024-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "repeated thing definition id", "malformed thing definition, repeated id");

	contents=R"str(
beginobjectset
	file data/bad-025-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "repeated property 'name'", "malformed thing definition, repeated property");

	contents=R"str(
beginobjectset
	file data/bad-026-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unexpected end of file before 'endproperty'", "unclosed property definition");

	contents=R"str(
beginobjectset
	file data/bad-027-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "missing value for 'type'", "malformed property definition, missing parameters");

	contents=R"str(
beginobjectset
	file data/bad-028-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "syntax error: expected property value", "malformed property definition, missing values");

	contents=R"str(
beginobjectset
	file data/bad-029-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "invalid property type 'float', expected int, double or string", "malformed property definition, invalid types");

	contents=R"str(
beginobjectset
	file data/bad-053-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "invalid link type 'meh'", "invalid linkedto property name");

	contents=R"str(
beginobjectset
	file data/bad-054-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "double property 'meh' cannot be linked", "invalid linkedto property type (double)");

	contents=R"str(
beginobjectset
	file data/bad-055-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "string property 'meh' cannot be linked", "invalid linkedto property type (string)");

	contents=R"str(
beginobjectset
	file data/bad-030-b.txt
	id 1
	name lol
endobjectset


)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "property 'prop' already exists", "repeated property name, even of different types");

	contents=R"str(
beginsession
	thingcenter topright
	bgcolor 1 2 32 0
	gridsize 64
	gridvruler 4
	gridhruler 6
	gridcolor 2 32 1 0
	gridrulercolor 32 2 1 0
#endsession

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unexpected end of file before 'endsession'", "unclosed beginsession");

	contents=R"str(
beginsession
	thingcenter topright
	bgcolor 1 2 32 0
	gridsize 64
	gridvruler 4
	gridhruler 6
	gridcolor 2 32 1 0
	ouch 32 2 1 0
endsession

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unrecognised 'ouch'", "unknown property in session data");

	contents=R"str(
beginsession
	thingcenter topright
	bgcolor 1 2 32 0
	gridsize 64
	gridvruler 4
	gridhruler
	gridcolor 2 32 1 0
endsession

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "syntax error: expected property value", "missing value for session data property");

	contents=R"str(
beginsession
	thingcenter center
	bgcolor 1 2 a 0
endsession

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "invalid color schema", "bad bg color in session data");

	contents=R"str(
beginsession
	thingcenter center
	gridcolor 1 2 a 0
endsession

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "invalid color schema", "bad grid color in session data");

	contents=R"str(
beginsession
	thingcenter center
	gridrulercolor 1 2 a 0
endsession

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "invalid color schema", "bad ruler color in session data");

	contents=R"str(
beginsession
	thingcenter nowhere
endsession

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "invalid value for thingcenter, valid values are", "bad thingcenter value");

	contents=R"str(
beginsession
	gridsize ouch
endsession

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "invalid int value for 'gridsize'", "bad gridsize");

	contents=R"str(
beginpolyset
	id 1
#file lol
endpolyset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "missing value for 'file'", "config parser with invalid polyset definition");

contents=R"str(
beginpolyset
	id 1
	file lol
	#name lol
endpolyset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "missing value for 'name'", "config parser with invalid polyset definition, no name");

	contents=R"str(
beginpolyset
	file data/good_polyset_02.txt
	id 2
	name lol
endpolyset

beginpolyset
	file data/good_polyset_02.txt
	id 2
	name lol
endpolyset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "repeated id value", "config parser with repeated polyset id");

	contents=R"str(
beginpolyset
	file not-a-file
	id 1
	name lol
endpolyset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "cannot find file 'not-a-file'", "config parser with non existing polyset file");

	contents=R"str(
beginpolyset
	file data/good_polyset_02.txt
	id 1
	name lol

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unexpected end of file before 'endpolyset'", "config parser with unclosed polyset definition");

	contents=R"str(
beginpolyset
	file data/bad-043-b.txt
	id 1
	name lol
endpolyset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unexpected file end before 'endpoly'", "poly parser with unclosed poly definition");

	contents=R"str(
beginpolyset
	file data/bad-044-b.txt
	id 1
	name lol
endpolyset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "unknown property name 'ouch'", "poly parser with unexpected tag");

	contents=R"str(
beginpolyset
	file data/bad-045-b.txt
	id 1
	name lol
endpolyset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "missing property 'name'", "poly parser with missing property");

	contents=R"str(
beginpolyset
	file data/bad-046-b.txt
	id 1
	name lol
endpolyset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "missing property value for 'name'", "poly parser with missing values");

	contents=R"str(
beginpolyset
	file data/bad-048-b.txt
	id 1
	name lol
endpolyset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "invalid color schema", "poly parser with bad color");

	contents=R"str(
beginpolyset
	file data/bad-049-b.txt
	id 1
	name lol
endpolyset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "invalid id value", "poly parser with bad id");

	contents=R"str(
beginpolyset
	file data/bad-050-b.txt
	id 1
	name lol
endpolyset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "repeated poly definition id", "poly parser with repeated id");

	contents=R"str(
beginpolyset
	file data/bad-052-b.txt
	id 1
	name lol
endpolyset

)str";
	must_throw([&cfp, contents](){cfp.parse_string(contents);}, "polygon properties cannot be linked to width or height", "invalid linkedto property for a polygon");

	std::cout<<"done, all good"<<std::endl;

	return 0;
}

void check_thing(
	const tile_editor::thing_definition_table& _things,
	size_t _index,
	const std::string& _name,
	int _w,
	int _h,
	int _r,
	int _g,
	int _b,
	int _a,
	std::size_t _propcount
) {

	assert(1==_things.table.count(_index), std::string{"undefined thing index '"}+std::to_string(_index)+"'");

	const auto& thing=_things.table.at(_index);
	assert(_name==thing.name, _name+"does not match thing name");
	assert(_w==thing.w, std::to_string(_w)+" does not match thing width");
	assert(_h==thing.h, std::to_string(_h)+" does not match thing height");

	assert(_r==thing.color.r, std::to_string(_r)+" does not match thing red");
	assert(_g==thing.color.g, std::to_string(_g)+" does not match thing green");
	assert(_b==thing.color.b, std::to_string(_b)+" does not match thing blue");
	assert(_a==thing.color.a, std::to_string(_a)+" does not match thing alpha+");
	assert(_propcount==thing.properties.size(), "propsize does not match");
}

void check_poly(
	const tile_editor::poly_definition_table& _polys,
	std::size_t _index,
	const std::string& _name,
	int _r,
	int _g,
	int _b,
	int _a,
	std::size_t _propcount
) {

	assert(1==_polys.table.count(_index), std::string{"undefined poly index '"}+std::to_string(_index)+"'");

	const auto& poly=_polys.table.at(_index);
	assert(_name==poly.name, _name+"does not match poly name");

	assert(_r==poly.color.r, std::to_string(_r)+" does not match poly red");
	assert(_g==poly.color.g, std::to_string(_g)+" does not match poly green");
	assert(_b==poly.color.b, std::to_string(_b)+" does not match poly blue");
	assert(_a==poly.color.a, std::to_string(_a)+" does not match poly alpha+");
	assert(_propcount==poly.properties.size(), "propsize does not match");
}


#include "parser/config_parser.h"

#include <iostream>
#include <stdexcept>

void fail(const std::string& _msg);
void assert(bool _thing, const std::string& _msg);

int main(int argc, char ** argv) {

	std::cout<<"testing complete valid config file"<<std::endl;
	tile_editor::config_parser cfp;
	auto blueprint=cfp.read(std::string{"data/good_config.txt"});

	try {

		//Tilesets are easy to assert: we very much know they work by their own 
		//tests. We need only to check a few values...
		std::cout<<"testing tileset contents..."<<std::endl;

		assert(2==blueprint.tilesets.size(), "failed to assert that there are 2 tilesets");

		assert(1==blueprint.tilesets.count(1), "failed to assert that tileset 1 exists");
		assert(3==blueprint.tilesets[1].size(), "failed to assert that tileset 1 has 3 tiles");
		assert(64==blueprint.tilesets[1].get(1).x, "failed to assert contents of tileset 1");

		assert(1==blueprint.tilesets.count(2), "failed to assert that tileset 2 exists");
		assert(12==blueprint.tilesets[2].size(), "failed to assert that tileset 2 has 12 tiles");
		assert(32==blueprint.tilesets[2].get(11).y, "failed to assert contents of tileset 2");

		//Thingsets must be thoroughly tested...
		std::cout<<"testing thingset contents..."<<std::endl;
		assert(3==blueprint.thingsets.size(), "failed to assert that there are 3 thingsets");

		assert(4==blueprint.thingsets[1].size(), "failed to assert item count on thingset 1");
		//TODO: write this: set, id, name, w, h, size, r, g, b, prop count.
		check_thing(blueprint.thingsets[1], 1, "extra_life", 16, 16, "fixed", 255, 255, 0, 0);
		check_thing(blueprint.thingsets[1], 2, "health", 16, 16, "fixed", 0, 255, 255, 0);
		check_thing(blueprint.thingsets[1], 3, "enemy", 32, 16, "fixed", 255, 255, 128, 1);
		//TODO: write this: set, id, name, type, default, description
		check_property(blueprint.thingsets[1], 3, "type_id", "int", 1, "Enemy type id");

		check_thing(blueprint.thingsets[1], 4, "friend", 32, 16, "fixed", 255, 200, 128, 4);
		//TODO: Must actually be a template.
		check_property(blueprint.thingsets[1], 4, "type_id", "int", 1, "Friend type id");
		check_property(blueprint.thingsets[1], 4, "health", "int", 100, "Basic friend health level");
		check_property(blueprint.thingsets[1], 4, "name", "string", "Unnamed", "Friend name to be displayed");
		check_property(blueprint.thingsets[1], 4, "factor", "double", 2.5, "Movement factor");

		assert(2==blueprint.thingsets[2].size(), "failed to assert item count on thingset 2");
		//TODO: Go on...

		assert(1==blueprint.thingsets[3].size(), "failed to assert item count on thingset 3");
		//TODO: Go on...

		//TODO: Test map properties, count them and test them.
	}
	catch(std::exception& e) {

		std::cerr<<"failed: "<<e.what()<<std::endl;
		return 1;
	}
	
	//TODO: Test the contents...

	//TODO: test all possible invalid configurations, of which there
	//are A LOT.

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

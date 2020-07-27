#include "parser/config_parser.h"

#include <iostream>

int main(int argc, char ** argv) {

	std::cout<<"testing complete valid config file"<<std::endl;
	tile_editor::config_parser cfp;
	cfp.read(std::string{"config.txt"});

	//TODO: test all possible invalid configurations, of which there
	//are A LOT.

	std::cout<<"done"<<std::endl;

	return 0;
}

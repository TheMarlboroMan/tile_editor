#include "parser/config_parser.h"

#include <iostream>

int main(int argc, char ** argv) {

	if(argc==1) {

		std::cout<<"a.out config_file"<<std::endl;
		return 1;
	}

	tile_editor::config_parser cfp;
	cfp.read(std::string{argv[1]});

	return 0;
}

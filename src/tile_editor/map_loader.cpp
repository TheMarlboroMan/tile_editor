#include "app/map_loader.h"
#include "parser/map_parser.h"

#include <lm/sentry.h>

using namespace tile_editor;

map_loader::map_loader(
	lm::logger& _logger,
	tools::message_manager& _message_manager
):
	log(_logger),
	message_manager{_message_manager} {

}

tile_editor::map map_loader::load_from_file(const std::string& _path) {

	tile_editor::map_parser mp;
	tile_editor::map result=mp.parse_file(_path);

	for(const auto& msg : mp.get_errors()) {

		lm::log(log, lm::lvl::notice)<<msg<<std::endl;
		message_manager.add(msg);
	}

	if(mp.get_errors().size()) {

		message_manager.add("there were errors loading the map, please check the log file");
	}
	else {

		std::string msg=std::string{"loaded map "}
			+_path+
			+"with "
			+std::to_string(result.tile_layers.size())+" tile layers,"
			+std::to_string(result.thing_layers.size())+" thing layers,"
			+std::to_string(result.poly_layers.size())+" polygon layers and "
			+std::to_string(result.properties.size())+" properties";

		lm::log(log, lm::lvl::info)<<msg<<std::endl;

		message_manager.add(msg);
	}

	return result;
}

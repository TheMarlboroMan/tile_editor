#include "app/map_saver.h"
#include "serializer/map_serializer.h"

#include <lm/sentry.h>
#include <iostream>

using namespace tile_editor;

map_saver::map_saver(
	lm::logger& _logger,
	tools::message_manager& _mm
):
	log{_logger},
	message_manager{_mm}
{

}

void map_saver::save(
	const tile_editor::map& _map,
	const std::string& _filename
) {

	lm::log(log, lm::lvl::info)<<"saving map into "<<_filename<<std::endl;

	tile_editor::map_serializer serializer;
	//TODO: Where does the version reside????
	serializer.to_file(_map, "1.0,0", _filename);

	//TODO: Errors and shit, log dem?
	message_manager.add("map saved");
	lm::log(log, lm::lvl::info)<<"map saved into "<<_filename<<std::endl;
}

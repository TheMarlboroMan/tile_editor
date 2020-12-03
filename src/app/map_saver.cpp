#include "app/map_saver.h"

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
	const tile_editor::map&,
	const std::string& _filename
) {

	lm::log(log, lm::lvl::info)<<"saving map into "<<_filename<<std::endl;


	//TODO: do your shit, do not forget to log crap!
}

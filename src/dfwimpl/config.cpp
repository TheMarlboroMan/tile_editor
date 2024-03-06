#include "../../include/dfwimpl/config.h"

using namespace dfwimpl;

config::config(
	const tile_editor::env& _env
)
	:dfw::base_config(get_file_path(_env))
{}


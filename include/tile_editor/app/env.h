#pragma once

#include <string>

namespace tile_editor {

struct env {

	/**
	* Returns the string where the installed application data can be
	* found (configuration, mostly).
	*/
	
	std::string	get_app_path() const;
};

}

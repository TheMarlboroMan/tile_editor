#pragma once

#include <string>

namespace tile_editor {

struct env {

	env(const std::string&, const std::string);

/**
* Returns the string where the installed application data can be
* found (configuration, mostly).
*/
//	std::string	get_app_path() const;
	std::string build_log_path(const std::string&) const;
	std::string build_assets_path(const std::string&) const;
	std::string build_data_path(const std::string&) const;


	std::string build_user_path(const std::string&) const;

	private:

	const std::string exec_dir;
	const std::string userdir;
};

}

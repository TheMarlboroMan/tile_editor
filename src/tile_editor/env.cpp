#include "tile_editor/app/env.h"

using namespace tile_editor;

env::env(
	const std::string& _exec, 
	const std::string _home
):
	exec_dir(_exec),
	userdir{_home+"/.tile_editor/"}
{}

std::string env::build_log_path(const std::string& _file) const {

	return exec_dir+std::string{"logs/"}+_file;
}

std::string env::build_data_path(const std::string& _file) const {

	return exec_dir+std::string{"data/"}+_file;
}

std::string env::build_assets_path(const std::string& _file) const {

	return exec_dir+std::string{"assets/"}+_file;
}

std::string env::build_user_path(const std::string& _file) const {

	return userdir+"/"+_file;
}

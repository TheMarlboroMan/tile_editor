#include "dfwimpl/config.h"
#include "dfwimpl/state_driver.h"
#include "app/env.h"

#include <lm/file_logger.h>
#include <lm/void_logger.h>
#include <lm/sentry.h>

#include <dfw/kernel.h>

#include <tools/arg_manager.h>
#include <tools/file_utils.h>

#include <ldt/sdl_tools.h>
#include <ldt/log.h>

#include <stdexcept>
#include <unistd.h>
#include <stdlib.h>
#include <filesystem>


tile_editor::env setup_env();

int main(int argc, char ** argv)
{

	tile_editor::env env=setup_env();

	//Argument controller.
	tools::arg_manager carg(argc, argv);

	if(carg.exists("-h")) {

		std::cout<<"sprite_table "
			<<MAJOR_VERSION<<"."<<MINOR_VERSION<<"."<<PATCH_VERSION
			<<" built on "<<__DATE__<<" "<<__TIME__
			<<std::endl<<std::endl
			<<tools::dump_file(env.build_data_path("help.txt"))<<std::endl;
		return 0;
	}

	if(carg.exists("-v")) {

		std::cout<<"sprite_table "
			<<MAJOR_VERSION<<"."<<MINOR_VERSION<<"."<<PATCH_VERSION
			<<" built on "<<__DATE__<<" "<<__TIME__<<std::endl;
		return 0;
	}

	if(!carg.exists("-c") || !carg.arg_follows("-c")) {

		std::cerr<<"./tile_editor -c config_file [-f filename] [-w 200x200]"<<std::endl;
		return 1;
	}

	//Init libdansdl2 log.
	ldt::log_lsdl::set_type(ldt::log_lsdl::types::null);

	//Init application log.
	std::string log_path{env.build_log_path("app.log")};
	lm::file_logger log_app(log_path.c_str());
	lm::log(log_app, lm::lvl::info)<<"starting main process..."<<std::endl;

	//Init...
	try {
		lm::log(log_app, lm::lvl::info)<<"init sdl2..."<<std::endl;
		if(!ldt::sdl_init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK)) {
			throw std::runtime_error("unable to init sdl2");
		}

		lm::log(log_app, lm::lvl::info)<<"creating kernel..."<<std::endl;
		dfw::kernel kernel(log_app, carg);

		lm::log(log_app, lm::lvl::info)<<"init app config..."<<std::endl;
		dfwimpl::config config{env};

		lm::log(log_app, lm::lvl::info)<<"create state driver..."<<std::endl;
		dfwimpl::state_driver sd(kernel, config, env);

		lm::log(log_app, lm::lvl::info)<<"init state driver..."<<std::endl;
		sd.init(kernel);

		lm::log(log_app, lm::lvl::info)<<"finish main proccess"<<std::endl;
	}
	catch(std::exception& e) {
		std::cerr<<"Interrupting due to exception: "<<e.what()<<std::endl;
		lm::log(log_app, lm::lvl::error)<<"an error happened "<<e.what()<<std::endl;
		lm::log(log_app, lm::lvl::info)<<"stopping sdl2..."<<std::endl;
		ldt::sdl_shutdown();

		return 1;
	}

	lm::log(log_app, lm::lvl::info)<<"stopping sdl2..."<<std::endl;
	ldt::sdl_shutdown();
	return 0;
}

tile_editor::env setup_env() {

	std::string executable_path, executable_dir;
	std::array<char, 1024> buff;

	int bytes=readlink("/proc/self/exe", buff.data(), 1024);
	if(-1==bytes) {

		std::cerr<<"could not locate proc/self/exe, error "<<errno<<std::endl;
		throw std::runtime_error("could not locate proc/self/exe");
	}

	executable_path=std::string{std::begin(buff), std::begin(buff)+bytes};
	auto last_slash=executable_path.find_last_of("/");
	executable_dir=executable_path.substr(0, last_slash)+"/";

	return tile_editor::env(executable_dir, getenv("HOME"));
}

#include "dfwimpl/config.h"
#include "dfwimpl/state_driver.h"
#include "app/env.h"

#include <lm/file_logger.h>
#include <lm/void_logger.h>
#include <lm/sentry.h>

#include <dfw/kernel.h>

#include <tools/arg_manager.h>

#include <ldt/sdl_tools.h>
#include <ldt/log.h>

int main(int argc, char ** argv)
{
	//Init libdansdl2 log.
	tile_editor::env env;
	ldt::log_lsdl::set_type(ldt::log_lsdl::types::null);
//	ldt::log_lsdl::set_filename(env.get_app_path()+"logs/libdansdl2.log");

	//Argument controller.
	tools::arg_manager carg(argc, argv);

	if(!carg.exists("-c") || !carg.arg_follows("-c")) {

		std::cerr<<"./tile_editor -c config_file [-f filename] [-w 200x200]"<<std::endl;
		return 1;
	}

	//Init application log.
	std::string log_path{env.get_app_path()+"logs/app.log"};
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

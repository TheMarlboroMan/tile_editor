#include "include/dfwimpl/config.h"
#include "include/dfwimpl/state_driver.h"

#include <lm/file_logger.h>
#include <lm/sentry.h>

#include <dfw/kernel.h>

#include <tools/arg_manager.h>

#include <ldt/sdl_tools.h>
#include <ldt/log.h>

int main(int argc, char ** argv)
{
	//Init libdansdl2 log.
	ldt::log_lsdl::set_type(ldt::log_lsdl::types::file);
	ldt::log_lsdl::set_filename("logs/libdansdl2.log");

	//Argument controller.
	tools::arg_manager carg(argc, argv);

	//Init application log.
	lm::file_logger log_app("logs/app.log");
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
		dfwimpl::config config;

		lm::log(log_app, lm::lvl::info)<<"create state driver..."<<std::endl;
		dfwimpl::state_driver sd(kernel, config);

		lm::log(log_app, lm::lvl::info)<<"init state driver..."<<std::endl;
		sd.init(kernel);

		lm::log(log_app, lm::lvl::info)<<"finish main proccess"<<std::endl;
	}
	catch(std::exception& e) {
		std::cout<<"Interrupting due to exception: "<<e.what()<<std::endl;
		lm::log(log_app, lm::lvl::error)<<"an error happened "<<e.what()<<std::endl;
		lm::log(log_app, lm::lvl::info)<<"stopping sdl2..."<<std::endl;
		ldt::sdl_shutdown();

		return 1;
	}

	lm::log(log_app, lm::lvl::info)<<"stopping sdl2..."<<std::endl;
	ldt::sdl_shutdown();
	return 0;
}

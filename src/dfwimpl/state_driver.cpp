#include "dfwimpl/state_driver.h"
#include "input/input.h"
#include "controller/states.h"
#include "tile_editor/parser/blueprint_parser.h"
#include "tile_editor/parser/map_parser.h"

#include <lm/sentry.h>
#include <tools/string_utils.h>

#include <algorithm>

using namespace dfwimpl;

state_driver::state_driver(dfw::kernel& kernel, dfwimpl::config& c)
	:state_driver_interface(controller::t_states::state_editor),
	config(c),
	log(kernel.get_log()),
	message_manager(30)
{

	lm::log(log, lm::lvl::info)<<"setting state check function..."<<std::endl;

	states.set_function([](int v){
		return v > controller::state_min && v < controller::state_max;
	});

	lm::log(log, lm::lvl::info)<<"init state driver building: preparing video..."<<std::endl;
	prepare_video(kernel);

	lm::log(log, lm::lvl::info)<<"preparing audio..."<<std::endl;
	prepare_audio(kernel);

	lm::log(log, lm::lvl::info)<<"preparing input..."<<std::endl;
	prepare_input(kernel);

	lm::log(log, lm::lvl::info)<<"preparing resources..."<<std::endl;
	prepare_resources(kernel);

	lm::log(log, lm::lvl::info)<<"registering controllers..."<<std::endl;
	register_controllers(kernel);

	lm::log(log, lm::lvl::info)<<"virtualizing input..."<<std::endl;
	virtualize_input(kernel.get_input());

	lm::log(log, lm::lvl::info)<<"reading application data..."<<std::endl;
	read_app_data(kernel.get_arg_manager());

	lm::log(log, lm::lvl::info)<<"state driver fully constructed"<<std::endl;
}

void state_driver::prepare_video(dfw::kernel& kernel) {

	kernel.init_video_system({
		config.int_from_path("video:window_w_px"),
		config.int_from_path("video:window_h_px"),
		config.int_from_path("video:window_w_logical"),
		config.int_from_path("video:window_h_logical"),
		config.string_from_path("video:window_title"),
		config.bool_from_path("video:window_show_cursor"),
		config.get_screen_vsync()
	});

	auto& screen=kernel.get_screen();
	screen.set_fullscreen(config.bool_from_path("video:fullscreen"));

	ttf_manager.insert("main", 14, "assets/ttf/BebasNeue-Regular.ttf");
}

void state_driver::prepare_audio(dfw::kernel& kernel) {

	kernel.init_audio_system({
		config.get_audio_ratio(),
		config.get_audio_out(),
		config.get_audio_buffers(),
		config.get_audio_channels(),
		config.get_audio_volume(),
		config.get_music_volume()
	});
}

void state_driver::prepare_input(dfw::kernel& kernel) {

	using namespace dfw;

	std::vector<input_pair> pairs{
		{{input_description::types::keyboard, SDL_SCANCODE_ESCAPE, 0}, input::escape},
		{input_description_from_config_token(config.token_from_path("input:left")), input::left},
		{input_description_from_config_token(config.token_from_path("input:right")), input::right},
		{input_description_from_config_token(config.token_from_path("input:up")), input::up},
		{input_description_from_config_token(config.token_from_path("input:down")), input::down},
		{input_description_from_config_token(config.token_from_path("input:enter")), input::enter},
		{input_description_from_config_token(config.token_from_path("input:space")), input::space},
		{input_description_from_config_token(config.token_from_path("input:save")), input::save},
		{input_description_from_config_token(config.token_from_path("input:load")), input::load},
		{input_description_from_config_token(config.token_from_path("input:help")), input::help},
		{input_description_from_config_token(config.token_from_path("input:pageup")), input::pageup},
		{input_description_from_config_token(config.token_from_path("input:pagedown")), input::pagedown},
		{input_description_from_config_token(config.token_from_path("input:zoom_in")), input::zoom_in},
		{input_description_from_config_token(config.token_from_path("input:zoom_out")), input::zoom_out},
		{input_description_from_config_token(config.token_from_path("input:left_click")), input::left_click}
	};

	kernel.init_input_system(pairs);
}

void state_driver::prepare_resources(dfw::kernel& /*kernel*/) {

/*
	dfw::resource_loader r_loader(kernel.get_video_resource_manager(), kernel.get_audio_resource_manager());

	r_loader.generate_textures(tools::explode_lines_from_file(std::string("data/resources/textures.txt")));
	r_loader.generate_sounds(tools::explode_lines_from_file(std::string("data/resources/audio.txt")));
	r_loader.generate_music(tools::explode_lines_from_file(std::string("data/resources/music.txt")));
*/
}

void state_driver::register_controllers(dfw::kernel& /*kernel*/) {

	auto reg=[this](ptr_controller& _ptr, int _i, dfw::controller_interface * _ci) {
		_ptr.reset(_ci);
		register_controller(_i, *_ptr);
	};


	unsigned int    screen_w=config.int_from_path("video:window_w_logical"),
					screen_h=config.int_from_path("video:window_h_logical");

	reg(
		c_editor,
		controller::t_states::state_editor,
		new controller::editor(
			log,
			ttf_manager,
			message_manager,
			screen_w,
			screen_h
		)
	);

	reg(
		c_file_browser,
		controller::t_states::state_file_browser,
		new controller::file_browser(
			log,
			ttf_manager,
			screen_h
		)
	);
	//[new-controller-mark]
}

void state_driver::prepare_state(int _next, int /*_current*/) {

	controller::file_browser& fb{* c_file_browser};

	switch(next) {
		case t_states::state_file_browser:

			//TODO: set the ability to new or not new.

		break;
		default:
		break;
	}

	switch(_current) {

		case t_states::state_file_browser:

			//TODO: Perform whatever is needed...

		break;
	}
}

void state_driver::common_pre_loop_input(dfw::input& input, float _delta) {

	if(input().is_event_joystick_connected()) {
		lm::log(log, lm::lvl::info)<<"New joystick detected..."<<std::endl;
		virtualize_input(input);
	}

	message_manager.tick(_delta);
}

void state_driver::common_loop_input(dfw::input& /*input*/, float /*delta*/) {

}

void state_driver::common_pre_loop_step(float /*delta*/) {

}

void state_driver::common_loop_step(float /*delta*/) {

}

void state_driver::virtualize_input(dfw::input& input) {

	lm::log(log, lm::lvl::info)<<"trying to virtualize "<<input().get_joysticks_size()<<" controllers..."<<std::endl;

	for(size_t i=0; i < input().get_joysticks_size(); ++i) {
		input().virtualize_joystick_hats(i);
		input().virtualize_joystick_axis(i, 15000);
		lm::log(log, lm::lvl::info)<<"Joystick virtualized "<<i<<std::endl;
	}
}

void state_driver::read_app_data(tools::arg_manager& _arg_manager) {

	//TODO: Should this shit be read outside and fed to the state driver
	//from main????
	tile_editor::blueprint_parser cfp;
	session=cfp.parse_file(_arg_manager.get_following("-c"));

	if(_arg_manager.exists("-f") && _arg_manager.arg_follows("-f")) {

		//TODO: This could be a map loader class so we can invoke it here
		//and from the outside too.
		tile_editor::map_parser mp;
		map=mp.parse_file(_arg_manager.get_following("-f"));

		for(const auto& msg : mp.get_errors()) {

			lm::log(log, lm::lvl::notice)<<msg<<std::endl;
			message_manager.add(msg);
		}

		if(mp.get_errors().size()) {

			message_manager.add("there were errors loading the map, please check the log file");
		}
		else {

			std::string msg=std::string{"loaded map with "}
				+std::to_string(map.tile_layers.size())+" tile layers,"
				+std::to_string(map.thing_layers.size())+" thing layers,"
				+std::to_string(map.poly_layers.size())+" polygon layers and "
				+std::to_string(map.properties.size())+" properties";

			message_manager.add(msg);
		}
	}
}

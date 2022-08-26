#include "dfwimpl/state_driver.h"
#include "input/input.h"
#include "app/definitions.h"
#include "controller/states.h"
#include <lm/log.h>
#include <tools/string_utils.h>

#include <algorithm>
#include <string>
#include <sstream>

using namespace dfwimpl;

state_driver::state_driver(
	dfw::kernel& kernel,
	dfwimpl::config& c,
	const tile_editor::env& _env
)
	:state_driver_interface(controller::t_states::state_editor),
	config(c),
	log(kernel.get_log()),
	env{_env},
	message_manager(30)
{

	lm::log(log).info()<<"setting state check function..."<<std::endl;

	states.set_function([](int v){
		return v > controller::state_min && v < controller::state_max;
	});

	lm::log(log).info()<<"init state driver building: preparing video..."<<std::endl;
	prepare_video(kernel);

	lm::log(log).info()<<"preparing audio..."<<std::endl;
	prepare_audio(kernel);

	lm::log(log).info()<<"preparing input..."<<std::endl;
	prepare_input(kernel);

	lm::log(log).info()<<"preparing resources..."<<std::endl;
	prepare_resources(kernel);

	lm::log(log).info()<<"registering controllers..."<<std::endl;
	register_controllers(kernel);

	lm::log(log).info()<<"virtualizing input..."<<std::endl;
	virtualize_input(kernel.get_input());

	lm::log(log).info()<<"reading application data..."<<std::endl;
	read_app_data(kernel.get_arg_manager());

	lm::log(log).info()<<"state driver fully constructed"<<std::endl;
}

void state_driver::prepare_video(dfw::kernel& kernel) {

	const auto& argman=kernel.get_arg_manager();

	int w=config.int_from_path("video:window_w_px");
	int h=config.int_from_path("video:window_h_px");

	if(argman.exists("-w") && argman.arg_follows("-w")) {

		const std::string& window_size_str=argman.get_following("-w");

		auto xpos=window_size_str.find("x");
		if(std::string::npos==xpos) {
			throw std::runtime_error("-w parameter must be specified in wxh");
		}

		w=std::stoi(window_size_str.substr(0, xpos));
		h=std::stoi(window_size_str.substr(xpos+1));

		lm::log(log).info()<<"window size specified by command line as "<<window_size_str<<" ["<<w<<"x"<<h<<"]"<<std::endl;
	}

	std::stringstream ss;
	ss<<config.string_from_path("video:window_title")<<" v"<<MAJOR_VERSION<<"."<<MINOR_VERSION<<"."<<PATCH_VERSION<<"-"<<BUILD_VERSION;

	kernel.init_video_system({
		w, h,
		w, h,
		ss.str(),	
		config.bool_from_path("video:window_show_cursor"),
		config.get_screen_vsync()
	});

	auto& screen=kernel.get_screen();
	screen.set_fullscreen(config.bool_from_path("video:fullscreen"));

	ttf_manager.insert(
		tile_editor::definitions::main_font_name,
		tile_editor::definitions::main_font_size,
		env.build_assets_path("ttf/unispace.ttf")
	);
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
		{input_description_from_config_token(config.token_from_path("input:backspace")), input::backspace},
		{input_description_from_config_token(config.token_from_path("input:tab")), input::tab},
		{input_description_from_config_token(config.token_from_path("input:save")), input::save},
		{input_description_from_config_token(config.token_from_path("input:load")), input::load},
		{input_description_from_config_token(config.token_from_path("input:del")), input::del},
		{input_description_from_config_token(config.token_from_path("input:insert")), input::insert},
		{input_description_from_config_token(config.token_from_path("input:lctrl")), input::lctrl},
		{input_description_from_config_token(config.token_from_path("input:lshift")), input::lshift},
		{input_description_from_config_token(config.token_from_path("input:lalt")), input::lalt},
		{input_description_from_config_token(config.token_from_path("input:help")), input::help},
		{input_description_from_config_token(config.token_from_path("input:layer_settings")), input::layer_settings},
		{input_description_from_config_token(config.token_from_path("input:map_properties")), input::map_properties},
		{input_description_from_config_token(config.token_from_path("input:pageup")), input::pageup},
		{input_description_from_config_token(config.token_from_path("input:pagedown")), input::pagedown},
		{input_description_from_config_token(config.token_from_path("input:zoom_in")), input::zoom_in},
		{input_description_from_config_token(config.token_from_path("input:zoom_out")), input::zoom_out},
		{input_description_from_config_token(config.token_from_path("input:smaller_subgrid")), input::smaller_subgrid},
		{input_description_from_config_token(config.token_from_path("input:larger_subgrid")), input::larger_subgrid},
		{input_description_from_config_token(config.token_from_path("input:left_click")), input::left_click},
		{input_description_from_config_token(config.token_from_path("input:right_click")), input::right_click}
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

void state_driver::register_controllers(dfw::kernel& _kernel) {

	auto reg=[this](ptr_controller& _ptr, int _i, dfw::controller_interface * _ci) {
		_ptr.reset(_ci);
		register_controller(_i, *_ptr);
	};

	auto& screen=_kernel.get_screen();
	unsigned int    screen_w=screen.get_w(),
					screen_h=screen.get_h();

	reg(
		c_editor,
		controller::t_states::state_editor,
		new controller::editor(
			log,
			ttf_manager,
			message_manager,
			exchange_data,
			env,
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
			exchange_data,
			env,
			screen_h
		)
	);

	reg(
		c_help,
		controller::t_states::state_help,
		new controller::help{
			log,
			ttf_manager,
			env,
			screen_w,
			screen_h
		}
	);
	reg(
		c_tile_editor_properties,
		controller::t_states::state_tile_editor_properties,
		new controller::tile_editor_properties(
			log,
			ttf_manager,
			exchange_data
		)
	);
	reg(
		c_thing_editor_properties,
		controller::t_states::state_thing_editor_properties,
		new controller::thing_editor_properties(
			log,
			ttf_manager,
			exchange_data
		)
	);
	reg(
		c_poly_editor_properties,
		controller::t_states::state_poly_editor_properties,
		new controller::poly_editor_properties(
			log,
			ttf_manager,
			exchange_data
		)
	);
	reg(
		c_properties,
		controller::t_states::state_properties,
		new controller::properties(
			log,
			ttf_manager,
			exchange_data
		)
	);
	reg(
		c_layer_selector,
		controller::t_states::state_layer_selector,
		new controller::layer_selector(
			log,
			ttf_manager,
			message_manager,
			exchange_data
		)
	);
	//[new-controller-mark]
}

void state_driver::prepare_state(int /*_next*/, int /*_current*/) {

}

void state_driver::common_pre_loop_input(dfw::input& input, float _delta) {

	if(input().is_event_joystick_connected()) {
		lm::log(log).info()<<"New joystick detected..."<<std::endl;
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

	lm::log(log).info()<<"trying to virtualize "<<input().get_joysticks_size()<<" controllers..."<<std::endl;

	for(size_t i=0; i < input().get_joysticks_size(); ++i) {
		input().virtualize_joystick_hats(i);
		input().virtualize_joystick_axis(i, 15000);
		lm::log(log).info()<<"Joystick virtualized "<<i<<std::endl;
	}
}

void state_driver::read_app_data(tools::arg_manager& _arg_manager) {

	auto& editor=static_cast<controller::editor&>(*c_editor);

	editor.load_session(_arg_manager.get_following("-c"));
	if(_arg_manager.exists("-f") && _arg_manager.arg_follows("-f")) {

		editor.load_map(_arg_manager.get_following("-f"));
	}
	else {
		message_manager.add("application started, f1 for help");
	}
}

#pragma once

#include "config.h"

#include <dfw/state_driver_interface.h>
#include <ldtools/ttf_manager.h>

#include <memory>

//Controllers.
#include "controller/editor.h"
#include "controller/file_browser.h"
#include "../../include/controller/help.h"
#include "../../include/controller/tile_editor_properties.h"
#include "../../include/controller/thing_editor_properties.h"
#include "../../include/controller/poly_editor_properties.h"
#include "../../include/controller/properties.h"
#include "../../include/controller/layer_selector.h"
//[new-controller-header-mark]

//Specific app_config
#include "tools/message_manager.h"
#include "app/exchange_data.h"
#include "app/screen_titler.h"
#include "app/env.h"

namespace dfwimpl {

class state_driver:
	public dfw::state_driver_interface {

	public:
									state_driver(dfw::kernel& kernel, dfwimpl::config& config, const tile_editor::env&);

	virtual void                    init(dfw::kernel&);
	virtual void					common_pre_loop_input(dfw::input& input, ldtools::tdelta delta);
	virtual void					common_pre_loop_step(ldtools::tdelta delta);
	virtual void					common_loop_input(dfw::input& input, ldtools::tdelta delta);
	virtual void					common_loop_step(ldtools::tdelta delta);
	virtual void					prepare_state(int, int);
	virtual ldtools::tdelta			get_max_timestep() const {return 0.03;}

	private:

	void						prepare_video(dfw::kernel&);
	void						prepare_audio(dfw::kernel&);
	void						prepare_input(dfw::kernel&);
	void						prepare_resources(dfw::kernel&);
	void						register_controllers(dfw::kernel&);
	void 						virtualize_input(dfw::input& input);
	void                        read_app_data(tools::arg_manager&);

	//references
	dfwimpl::config&                config;
	lm::logger&                     log;
	const tile_editor::env&         env;

	typedef std::unique_ptr<dfw::controller_interface>	ptr_controller;
	ptr_controller                  c_editor;
	ptr_controller                  c_file_browser;
	ptr_controller					c_help;
	ptr_controller					c_tile_editor_properties;
	ptr_controller                  c_thing_editor_properties;
	ptr_controller                  c_poly_editor_properties;
	ptr_controller					c_properties;

	ptr_controller					c_layer_selector;
	//[new-controller-property-mark]

	ldtools::ttf_manager            ttf_manager;
	tools::message_manager          message_manager;
	tile_editor::exchange_data      exchange_data;
	tile_editor::screen_titler      screen_titler;


};

}

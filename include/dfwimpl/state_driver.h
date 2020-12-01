#pragma once

#include "config.h"

#include <dfw/state_driver_interface.h>
#include <ldtools/ttf_manager.h>

#include <memory>

//Controllers.
#include "../../include/controller/editor.h"
//[new-controller-header-mark]

//Specific app_config
#include "../tile_editor/blueprint_types/map_blueprint.h"
#include "../tile_editor/editor_types/map.h"

namespace dfwimpl {

class state_driver:
	public dfw::state_driver_interface {

	public:
									state_driver(dfw::kernel& kernel, dfwimpl::config& config);

	virtual void					common_pre_loop_input(dfw::input& input, float delta);
	virtual void					common_pre_loop_step(float delta);
	virtual void					common_loop_input(dfw::input& input, float delta);
	virtual void					common_loop_step(float delta);
	virtual void					prepare_state(int, int);
	virtual float					get_max_timestep() const {return 0.03f;}

	private:

	void						prepare_video(dfw::kernel&);
	void						prepare_audio(dfw::kernel&);
	void						prepare_input(dfw::kernel&);
	void						prepare_resources(dfw::kernel&);
	void						register_controllers(dfw::kernel&);
	void 						virtualize_input(dfw::input& input);
	void                        read_app_data(tools::arg_manager&);

	//references
	dfwimpl::config&				config;
	lm::logger&					log;

	typedef std::unique_ptr<dfw::controller_interface>	ptr_controller;
	ptr_controller					c_editor;
	//[new-controller-property-mark]

	ldtools::ttf_manager            ttf_manager;
	tile_editor::map_blueprint      session;
	tile_editor::map                map;
};

}

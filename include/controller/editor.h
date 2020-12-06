#pragma once

//local
#include "states.h"
#include "tools/message_manager.h"
#include "app/exchange_data.h"
#include "blueprint_types/map_blueprint.h"
#include "editor_types/map.h"

#include <ldv/ttf_representation.h>
#include <dfw/controller_interface.h>
#include <lm/logger.h>
#include <ldtools/ttf_manager.h>

#include <cmath>

namespace controller {

class editor:
	public dfw::controller_interface {

	public:

								editor(lm::logger&, ldtools::ttf_manager&, tools::message_manager&, tile_editor::exchange_data&, unsigned int, unsigned int);
	virtual void 				loop(dfw::input&, const dfw::loop_iteration_data&);
	virtual void 				draw(ldv::screen&, int);
	virtual void 				awake(dfw::input& /*input*/);
	virtual void 				slumber(dfw::input& /*input*/) {}
	virtual bool				can_leave_state() const {return true;}
	void                        load_session(const std::string&);
	void                        load_map(const std::string&);

	private:

	ldt::point_2d<int>          get_mouse_position(dfw::input&) const;
	void                        draw_messages(ldv::screen&);
	void                        draw_hud(ldv::screen&);
	void                        draw_grid(ldv::screen&);
	void                        draw_layers(ldv::screen&);
	void                        draw_layer(ldv::screen&, const tile_editor::tile_layer&);
	void                        draw_layer(ldv::screen&, const tile_editor::thing_layer&);
	void                        draw_layer(ldv::screen&, const tile_editor::poly_layer&);
	void                        zoom_in();
	void                        zoom_out();
	void                        save_current();
	void                        receive_message(tools::message_manager::notify_event_type);

	//references...
	lm::logger&                 log;
	ldtools::ttf_manager&       ttf_manager;
	tools::message_manager&     message_manager;
	tile_editor::exchange_data& exchange_data;

	//The editor controller owns both the map and the session. Data input for
	//specific components can be referenced somewhere else.
	tile_editor::map_blueprint  session;
	tile_editor::map            map;
	ldv::rect                   screen_rect;
	ldv::camera                 camera;
	ldv::ttf_representation     last_message_rep;
	ldt::point_2d<int>			mouse_pos;
	std::string                 current_filename;
	std::size_t	                current_layer{0};

};
}


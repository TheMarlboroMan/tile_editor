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
#include <tools/grid_list.h>
#include <tools/vertical_list.h>

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
	void                        draw_set(ldv::screen&);
	//!draws the background box for the toolset. also returns its x.
	int                         draw_set_background(ldv::screen&);
	//!draws a text item for the toolset.
	void                        draw_set_text(ldv::screen& _screen, int _x, int _y, int _h, tile_editor::color _color, const std::string& _name, bool _is_current);
	void                        draw_set(ldv::screen&, const tile_editor::tile_layer&);
	void                        draw_set(ldv::screen&, const tile_editor::thing_layer&);
	void                        draw_set(ldv::screen&, const tile_editor::poly_layer&);
	void                        zoom_in();
	void                        zoom_out();
	void                        next_layer();
	void                        previous_layer();
	void                        toggle_layer_draw_mode();
	void                        load_layer_toolset();
	void                        save_current();
	void                        receive_message(tools::message_manager::notify_event_type);

	//references...
	lm::logger&                 log;
	ldtools::ttf_manager&       ttf_manager;
	tools::message_manager&     message_manager;
	tile_editor::exchange_data& exchange_data;

	//TODO: These should likely go somewhere else...
	tile_editor::map_blueprint  session;
	tile_editor::map            map;
	std::map<std::string, std::unique_ptr<ldv::texture>> tileset_textures;

	enum class layer_draw_modes {all, current, stack} layer_draw_mode{layer_draw_modes::stack};
	ldv::rect                   screen_rect;
	ldv::camera                 camera;
	ldv::ttf_representation     last_message_rep;
	ldt::point_2d<int>          mouse_pos;
	tools::grid_list<ldtools::sprite_frame> tile_list;
	tools::vertical_list<tile_editor::thing_definition> thing_list;
	tools::vertical_list<tile_editor::poly_definition> poly_list;
	std::string                 current_filename;
	std::size_t	                current_layer{0},
	                            component_index{0}; //!< Currently chosen tile/thing/poly.
	bool                        show_set{true};

	static const int            grid_list_w{32},
	                            grid_list_h{32},
	                            grid_list_margin{8},
	                            vertical_list_h{32},
	                            vertical_list_margin{8},
	                            list_screen_portion{3}; //1 / x of the full w.
};
}


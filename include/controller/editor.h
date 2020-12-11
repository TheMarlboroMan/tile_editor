#pragma once

//local
#include "states.h"
#include "tools/message_manager.h"
#include "app/exchange_data.h"
#include "blueprint_types/map_blueprint.h"
#include "editor_types/map.h"
#include "editor_types/thing.h"
#include "editor_types/poly.h"

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

	enum key_modifiers {
		click_modifier_none=0,
		click_modifier_lshift=1,
		click_modifier_lctrl=2
	};

	using editor_point=ldt::point_2d<int>;

	struct {
		editor_point        point{0,0};
		bool                engaged{false};
		struct {
			int min{0}, max{0};
		}                  rangex, rangey;
	} multiclick;

	//!returns the world position from the mouse position.
	editor_point                get_world_position(ldt::point_2d<int>) const;
	//!returns grid-based position from world position.
	editor_point                get_grid_position(ldt::point_2d<int>) const;
	void                        arrow_input_set(int, int);
	void                        arrow_input_map(int, int);
	void                        click_input(int, int);
	void                        click_input(int, int, tile_editor::tile_layer&);
	void                        left_click_input(int, tile_editor::tile_layer&);
	void                        right_click_input(int, tile_editor::tile_layer&);
	void                        del_input();
	void                        subgrid_input(bool);
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
	void                        draw_cursor(ldv::screen&);
	void                        zoom_in();
	void                        zoom_out();
	void                        make_subgrid_smaller();
	void                        make_subgrid_larger();
	void                        next_layer();
	void                        previous_layer();
	//!resets all layer sensitive data (selections, polygons in the making...).
	void                        layer_change_cleanup();
	void                        toggle_layer_draw_mode();
	void                        load_layer_toolset();
	void                        save_current();
	void                        receive_message(tools::message_manager::notify_event_type);
	void                        open_layer_settings();
	//!Helpers for layer dispatchers, will do nothing if there are no layers, saving us 100 checks.
	bool                        dispatch_layer(tile_editor::const_layer_visitor&);
	bool                        dispatch_layer(tile_editor::layer_visitor&);

	//references...
	lm::logger&                 log;
	ldtools::ttf_manager&       ttf_manager;
	tools::message_manager&     message_manager;
	tile_editor::exchange_data& exchange_data;

	//TODO: These should likely go somewhere else...
	ldv::texture                cursor_tex;
	ldtools::sprite_table       cursor_table;
	tile_editor::map_blueprint  session;
	tile_editor::map            map;
	std::map<std::string, std::unique_ptr<ldv::texture>> tileset_textures;

	enum class layer_draw_modes {all, current, stack} layer_draw_mode{layer_draw_modes::stack};
	ldv::rect                   screen_rect;
	ldv::camera                 camera;
	ldv::ttf_representation     last_message_rep;
	ldt::point_2d<int>          mouse_pos;

	struct frame {
		std::size_t             id;
		ldtools::sprite_frame   frame;
	};

	//The sprite_frame does not store its id, so the grid will store the
	//natural pair id-sprite.
	tools::grid_list<ldtools::sprite_table::container::value_type> tile_list;
	tools::vertical_list<tile_editor::thing_definition> thing_list;
	tools::vertical_list<tile_editor::poly_definition> poly_list;
	std::string                 current_filename;
	std::size_t	                current_layer{0},
	                            subgrid_factor{0};
	tile_editor::thing *        selected_thing{nullptr};
	tile_editor::poly *         selected_poly{nullptr};
	bool                        show_set{true},
	                            tile_delete_mode{false};


	static const int            grid_list_w{32},
	                            grid_list_h{32},
	                            grid_list_margin{8},
	                            vertical_list_h{32},
	                            vertical_list_margin{8};
};
}


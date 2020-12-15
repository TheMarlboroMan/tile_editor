#include "controller/editor.h"

//local
#include "input/input.h"
#include "app/definitions.h"
#include "app/map_loader.h"
#include "app/map_saver.h"
#include "app/map_loader.h"
#include "app/entity_inflator.h"
#include "tile_editor/parser/blueprint_parser.h"
#include "tile_editor/editor_types/tile_layer.h"
#include "tile_editor/editor_types/thing_layer.h"
#include "tile_editor/editor_types/poly_layer.h"
#include "tile_editor/app/set_layer_loader.h"
#include <lm/sentry.h>
#include <ldv/line_representation.h>
#include <ldv/bitmap_representation.h>
#include <ldv/box_representation.h>
#include <ldv/line_representation.h>
#include <ldt/box.h>

#include <algorithm>

using namespace controller;

editor::editor(
	lm::logger& _log,
	ldtools::ttf_manager& _ttf_manager,
	tools::message_manager& _message_manager,
	tile_editor::exchange_data& _exchange_data,
	unsigned int _screen_w,
	unsigned int _screen_h
)
	:log(_log),
	ttf_manager(_ttf_manager),
	message_manager{_message_manager},
	exchange_data{_exchange_data},
	cursor_tex{ldv::texture(ldv::image("assets/bitmap/cursor.png"))},
	cursor_table{"data/cursor.dat"},
	screen_rect{0, 0, _screen_w, _screen_h},
	camera{
		screen_rect, //pointing at world 0,0.
		{0,0}           //at 0.0 in the screen...
	},
	last_message_rep{
		ttf_manager.get(tile_editor::definitions::main_font_name, tile_editor::definitions::main_font_size),
		ldv::rgba8(255, 255, 255, 255),
	},
	mouse_pos{0,0},
	tile_list{_screen_w, _screen_h, grid_list_w, grid_list_h},
	thing_list{_screen_h, vertical_list_h},
	poly_list{_screen_h, vertical_list_h} {

	tile_list.set_margin_w(grid_list_margin);
	tile_list.set_margin_h(grid_list_margin);
	thing_list.set_margin_h(vertical_list_margin);
	poly_list.set_margin_h(8);

	message_manager.subscribe("editor", [this](tools::message_manager::notify_event_type _type) {

		receive_message(_type);
	});
}

void editor::awake(dfw::input& /*_input*/) {

	lm::log(log, lm::lvl::info)<<"map editor controller awakens"<<std::endl;

	//Only the load-save controller leaves the signal.
	if(exchange_data.has(state_editor)) {

		exchange_data.recover(state_editor);
		if(exchange_data.file_browser_success) {

			//Save as requested...
			if(exchange_data.file_browser_allow_create) {

				current_filename=exchange_data.file_browser_choice;
				lm::log(log, lm::lvl::info)<<"map editor changes current filename to "<<current_filename<<std::endl;
				save_current();
			}
			//Entry point for when loading of a map was requested...
			else {

				load_map(exchange_data.file_browser_choice);
			}
		}
	}

	//Each time we reload let us just try and adjust selected entities, just
	//in case we come from the properties controller. This is pure shotgun
	//surgery, but this tool does not need much finesse.
	if(map.layers.size()) {

		if(nullptr!=selected_thing) {

			tile_editor::entity_inflator inflator;
			auto layer=map.layers.at(current_layer).get();
			inflator.inflate(*selected_thing, session.thingsets.at(layer->set).table.at(selected_thing->type));
		}

		if(nullptr!=selected_poly) {

			tile_editor::entity_inflator inflator;
			auto layer=map.layers.at(current_layer).get();
			inflator.inflate(*selected_poly, session.polysets.at(layer->set).table.at(selected_poly->type));
		}
	}
}

void editor::loop(dfw::input& _input, const dfw::loop_iteration_data& /*_lid*/) {

	if(_input().is_exit_signal() || _input.is_input_down(input::escape)) {

		set_leave(true);
		return;
	}

	int modifiers=key_modifier_none;
	if(_input.is_input_pressed(input::lshift)) {

		modifiers|=key_modifier_lshift;
	}

	if(_input.is_input_pressed(input::lctrl)) {

		modifiers|=key_modifier_lctrl;
	}

	if(_input.is_input_pressed(input::lalt)) {

		modifiers|=key_modifier_lalt;
	}

	if(_input.is_input_down(input::help)) {

		push_state(state_help);
		return;
	}

	if(_input.is_input_down(input::map_properties)) {

		open_map_properties();
		return;
	}

	if(_input.is_input_down(input::layer_settings)) {

		open_layer_settings();
		return;
	}

	if(_input.is_input_down(input::load)) {

		exchange_data.file_browser_allow_create=false;
		exchange_data.file_browser_title="Load map file";
		exchange_data.file_browser_invoker_id=state_editor;
		exchange_data.put(state_file_browser);
		push_state(state_file_browser);
		return;
	}

	if(_input.is_input_down(input::save)) {

		if(_input.is_input_pressed(input::lctrl)) {

			exchange_data.file_browser_allow_create=true;
			exchange_data.file_browser_title="Save map file as";
			exchange_data.file_browser_invoker_id=state_editor;
			exchange_data.put(state_file_browser);
			push_state(state_file_browser);
			return;
		}
		else {

			if(!current_filename.size()) {

				message_manager.add("cannot save without current filename, use ctrl+s");
				return;
			}
			save_current();
		}
	}

	if(_input.is_input_down(input::zoom_in)) {

		zoom_in();
		return;
	}
	else if(_input.is_input_down(input::zoom_out)) {

		zoom_out();
		return;
	}

	if(_input.is_input_down(input::space)) {

		if(modifiers & key_modifier_lctrl) {

			toggle_layer_draw_mode();
			return;
		}

		selected_thing=nullptr;
		selected_poly=nullptr;
		return;
	}

	if(_input.is_input_down(input::tab)) {

		if(modifiers & key_modifier_lctrl) {

			open_layer_selector();
			return;
		}

		if(map.layers.size()) {
			toggle_set_gui();
			return;
		}
	}

	auto mpos=_input().get_mouse_position();
	mouse_pos={mpos.x, mpos.y};
	if(modifiers & key_modifier_lalt) {

		struct : tile_editor::const_layer_visitor {
			bool snappable=true;
			void visit(const tile_editor::tile_layer&) {snappable=false;}
			void visit(const tile_editor::thing_layer&) {}
			void visit(const tile_editor::poly_layer&) {}
		} dispatcher;

		if(dispatch_layer(dispatcher) && dispatcher.snappable) {
			mouse_pos=snap_to_grid(mouse_pos);
		}
	}

	if(_input.is_input_down(input::pageup)) {

		previous_layer();
		return;
	}
	else if(_input.is_input_down(input::pagedown)) {

		next_layer();
		return;
	}

	if(_input.is_input_down(input::del)) {

		del_input();
	}

	if(_input.is_input_down(input::smaller_subgrid) || _input.is_input_down(input::larger_subgrid)) {

		subgrid_input(_input.is_input_down(input::smaller_subgrid));
		return;
	}

	if(_input.is_input_down(input::left_click)) {
		click_input(input::left_click, modifiers);
		return;
	}

	if(_input.is_input_down(input::right_click)) {
		click_input(input::right_click, modifiers);
		return;
	}

	if(show_set) {

		int movement_x=0,
			movement_y=0;

		if(_input.is_input_down(input::up)) {
			movement_y=-1;
		}
		else if(_input.is_input_down(input::down)) {
			movement_y=1;
		}
		else if(_input.is_input_down(input::left)) {
			movement_x=-1;
		}
		else if(_input.is_input_down(input::right)) {
			movement_x=1;
		}

		if(movement_x || movement_y) {

			 arrow_input_set(movement_x, movement_y, modifiers);
		}

		return;
	}
	else {

		typedef  bool (dfw::input::*input_fn)(int) const;

		input_fn movement_fn=_input.is_input_pressed(input::lctrl)
			? &dfw::input::is_input_down
			: &dfw::input::is_input_pressed;

		int movement_x=0,
			movement_y=0;

		if(std::invoke(movement_fn, _input, input::up)) {

			movement_y=-1;
		}
		else if(std::invoke(movement_fn, _input, input::down)) {

			movement_y=1;
		}

		if(std::invoke(movement_fn, _input, input::left)) {

			movement_x=-1;
		} else if(std::invoke(movement_fn, _input, input::right)) {

			movement_x=1;
		}

		if(movement_x || movement_y) {

			 arrow_input_layer(movement_x, movement_y, modifiers);
		}

		return;
	}
}

void editor::del_input() {

	struct : public tile_editor::layer_visitor {
		editor * controller{nullptr};
		void visit(tile_editor::tile_layer&) {controller->tile_delete_mode=!controller->tile_delete_mode;}
		void visit(tile_editor::thing_layer& _layer) {controller->del_input(_layer);}
		void visit(tile_editor::poly_layer&) {}
	} dispatcher;
	dispatcher.controller=this;
	dispatch_layer(dispatcher);
}

void editor::del_input(tile_editor::thing_layer& _layer) {

	if(nullptr==selected_thing) {

		return;
	}

	auto it=std::remove_if(
		std::begin(_layer.data),
		std::end(_layer.data),
		[this](const tile_editor::thing& _thing) {
			return &_thing==selected_thing;
		}
	);

	_layer.data.erase(it);
	selected_thing=nullptr;
	message_manager.add("thing deleted");
}

void editor::subgrid_input(
	bool _make_smaller
) {

	struct : public tile_editor::const_layer_visitor {
		editor * controller{nullptr};
		bool smaller;
		void visit(const tile_editor::tile_layer&) {}
		void visit(const tile_editor::thing_layer&) {smaller ? controller->make_subgrid_smaller() : controller->make_subgrid_larger();}
		void visit(const tile_editor::poly_layer&) {smaller ? controller->make_subgrid_smaller() : controller->make_subgrid_larger();}
	} dispatcher;
	dispatcher.controller=this;
	dispatcher.smaller=_make_smaller;
	dispatch_layer(dispatcher);
}

void editor::click_input(
	int _input,
	int _modifiers
) {
	struct : tile_editor::layer_visitor {
		editor * controller{nullptr};
		int input{0};
		int modifiers{0};
		void visit(tile_editor::tile_layer& _layer) {controller->click_input(input, modifiers, _layer);}
		void visit(tile_editor::thing_layer& _layer) {controller->click_input(input, modifiers, _layer);}
		void visit(tile_editor::poly_layer&) {}
	} dispatcher;
	dispatcher.controller=this;
	dispatcher.input=_input;
	dispatcher.modifiers=_modifiers;
	dispatch_layer(dispatcher);
}

void editor::click_input(
	int _input,
	int _modifiers,
	tile_editor::thing_layer& _layer
) {
	switch(_input) {
		case input::left_click: left_click_input(_modifiers, _layer); break;
		case input::right_click: right_click_input(_modifiers, _layer); break;
	}
}

void editor::click_input(
	int _input,
	int _modifiers,
	tile_editor::tile_layer& _layer
) {
	switch(_input) {
		case input::left_click: left_click_input(_modifiers, _layer); break;
		case input::right_click: right_click_input(_modifiers, _layer); break;
	}
}

void editor::left_click_input(
	int _modifiers,
	tile_editor::tile_layer& _layer
) {

	auto world_pos=get_world_position(mouse_pos);
	auto grid=get_grid_position(world_pos);

	//Process multiclick with lshift. Multiclick acts by delimiting a box.
	if(! (_modifiers & key_modifier_lshift)) {

		multiclick.engaged=false;
	}
	else {

		if(multiclick.point!=grid) {

			multiclick.engaged=true;
			multiclick.rangex={std::min(grid.x, multiclick.point.x), std::max(grid.x, multiclick.point.x)};
			multiclick.rangey={std::min(grid.y, multiclick.point.y), std::max(grid.y, multiclick.point.y)};
		}
	}

	multiclick.point=grid;

	//TODO: duplicate.
	auto find_tile_at=[&_layer](editor_point _pt) {

		return std::find_if(
			std::begin(_layer.data),
			std::end(_layer.data),
			[_pt](const tile_editor::tile& _tile) {
				return _tile.x==_pt.x && _tile.y==_pt.y;
			}
		);
	};

	if(tile_delete_mode) {

		auto delete_tile_if_exists=[&_layer, find_tile_at](editor_point _pt) {

			auto it=find_tile_at(_pt);
			if(it!=std::end(_layer.data)) {

				_layer.data.erase(it);
			}
		};

		if(!multiclick.engaged) {
			delete_tile_if_exists(grid);
			return;
		}

		for(int x=multiclick.rangex.min; x<=multiclick.rangex.max; x++) {
			for(int y=multiclick.rangey.min; y<=multiclick.rangey.max; y++) {
				delete_tile_if_exists({x, y});
			}
		}
		return;
	}

	//Insert or update...
	auto set_tile=[&_layer, find_tile_at](editor_point _pt, std::size_t _type) {

		auto it=find_tile_at(_pt);
		if(it!=std::end(_layer.data)) {
			it->type=_type;
		}
		//did not find anything...
		else {
			_layer.data.push_back({_pt.x, _pt.y, _type});
		}
	};

	if(!multiclick.engaged) {
		set_tile(grid, tile_list.get().first);
		return;
	}

	for(int x=multiclick.rangex.min; x<=multiclick.rangex.max; x++) {
		for(int y=multiclick.rangey.min; y<=multiclick.rangey.max; y++) {
			set_tile({x, y}, tile_list.get().first);
		}
	}

	return;
}

void editor::right_click_input(
	int /*_modifiers*/,
	tile_editor::tile_layer& _layer
) {

	auto world_pos=get_world_position(mouse_pos);
	auto grid=get_grid_position(world_pos);

	//TODO: duplicate.
	auto find_tile_at=[&_layer](editor_point _pt) {

		return std::find_if(
			std::begin(_layer.data),
			std::end(_layer.data),
			[_pt](const tile_editor::tile& _tile) {
				return _tile.x==_pt.x && _tile.y==_pt.y;
			}
		);
	};

	auto it=find_tile_at(grid);
	if(it!=std::end(_layer.data)) {

		//Is there anything in the tile list that matches this id?
		const auto id=it->type;
		auto index=tile_list.find([id](const ldtools::sprite_table::container::value_type& _item) -> bool {
			return id==_item.first;
		});

		if(tile_list.none!=index) {
			tile_list.set_index(index);
			message_manager.add("tile type selected");
		}
	}
}

void editor::left_click_input(
	int /*_modifiers*/,
	tile_editor::thing_layer& _layer
) {

	//Try and select...
	auto world_pos=get_world_position(mouse_pos);
	for(auto& thing : _layer.data) {

		//The origin is in "screen cordinates" so we must "cartesianify" it
		//when creating the box...
		auto origin=thing_origin_fn(thing.x, thing.y, thing.w, thing.h);
		auto box=ldt::box<int, unsigned>{{origin.x, origin.y}, (unsigned)thing.w, (unsigned)thing.h};

		if(box.point_inside(world_pos)) {

			//If already selected, open its properties!
			if(nullptr!=selected_thing && &thing==selected_thing) {

				open_thing_properties(
					thing.properties,
					session.thingsets.at(_layer.set).table.at(thing.type)
				);
				return;
			}

			selected_thing=&thing;
			return;
		}
	}

	//Add the current thing...
	const auto prototype=thing_list.get();

	tile_editor::thing thing{
		mouse_pos.x,
		mouse_pos.y,
		prototype.w,
		prototype.h,
		prototype.type_id,
		prototype.color,
		tile_editor::property_manager{}
	};

	auto dump_properties=[](auto list, auto& destination){

		for(const auto& prop : list) {
			destination[prop.first]=prop.second.default_value;
		}
	};

	dump_properties(prototype.properties.int_properties, thing.properties.int_properties);
	dump_properties(prototype.properties.double_properties, thing.properties.double_properties);
	dump_properties(prototype.properties.string_properties, thing.properties.string_properties);

	_layer.data.push_back(thing);
}

void editor::right_click_input(
	int /*_modifiers*/,
	tile_editor::thing_layer& /*_layer*/
) {

	//TODO:
	//Open the thing editor!!!
}

void editor::arrow_input_set(
	int _movement_x,
	int _movement_y,
	int /*_modifiers*/
) {

	struct : tile_editor::const_layer_visitor {
		editor * controller{nullptr};
		int      movement_x{0},
		         movement_y{0};

		void visit(const tile_editor::tile_layer&) {

			if(movement_y < 0) {
				controller->tile_list.previous_row();
				return;
			}
			else if(movement_y > 0) {
				controller->tile_list.next_row();
				return;
			}

			if(movement_x < 0) {
				controller->tile_list.previous();
			}
			else if(movement_x > 0) {
				controller->tile_list.next();
			}
		}
		void visit(const tile_editor::thing_layer&) {

			if(movement_y < 0) {
				controller->thing_list.previous();
			}
			else if(movement_y > 0) {
				controller->thing_list.next();
			}
		}
		void visit(const tile_editor::poly_layer&) {

			if(movement_y < 0) {
				controller->poly_list.previous();
			}
			else if(movement_y > 0) {
				controller->poly_list.next();
			}
		}
	} dispatcher;
	dispatcher.controller=this;
	dispatcher.movement_x=_movement_x;
	dispatcher.movement_y=_movement_y;
	dispatch_layer(dispatcher);
}

void editor::arrow_input_layer(
	int _movement_x,
	int _movement_y,
	int _modifiers
) {

	if(!map.layers.size()) {

		move_camera(_movement_x, _movement_y);
		return;
	}

	struct : tile_editor::layer_visitor {
		editor * controller{nullptr};
		int movement_x, movement_y, modifiers;
		void visit(tile_editor::tile_layer& _layer) {
			controller->arrow_input_layer(_layer, movement_x, movement_y, modifiers);
		};
		void visit(tile_editor::thing_layer& _layer) {
			controller->arrow_input_layer(_layer, movement_x, movement_y, modifiers);

		};
		void visit(tile_editor::poly_layer& _layer) {
			controller->arrow_input_layer(_layer, movement_x, movement_y, modifiers);

		};
	} dispatcher;
	dispatcher.controller=this;
	dispatcher.movement_x=_movement_x;
	dispatcher.movement_y=_movement_y;
	dispatcher.modifiers=_modifiers;
	dispatch_layer(dispatcher);
}

void editor::move_camera(
	int _movement_x,
	int _movement_y
) {
	camera.move_by(_movement_x * session.grid_data.size, _movement_y * session.grid_data.size);
}

void editor::arrow_input_layer(
	tile_editor::tile_layer&,
	int _movement_x,
	int _movement_y,
	int /*_modifiers*/
) {
	move_camera(_movement_x, _movement_y);
}

void editor::arrow_input_layer(
	tile_editor::thing_layer& /*_layer*/,
	int _movement_x,
	int _movement_y,
	int /*_modifiers*/
) {

	if(nullptr==selected_thing) {

		move_camera(_movement_x, _movement_y);
		return;
	}

	if(_movement_x) {

		selected_thing->x+=_movement_x*subgrid_factor;
		return;
	}

	if(_movement_y) {

		selected_thing->y+=_movement_y*subgrid_factor;
		return;
	}
}

void editor::arrow_input_layer(
	tile_editor::poly_layer& /*_layer*/,
	int _movement_x,
	int _movement_y,
	int /*_modifiers*/
) {

	if(nullptr==selected_poly) {

		move_camera(_movement_x, _movement_y);
		return;
	}

	//TODO: if there's something selected, move it.
}


void editor::draw(ldv::screen& _screen, int /*fps*/) {

	_screen.clear(ldv::rgba8(session.bg_color.r, session.bg_color.g, session.bg_color.b, session.bg_color.a));

	draw_grid(_screen);
	draw_layers(_screen);
	draw_hud(_screen);
	if(show_set) {
		draw_set(_screen);
	}
	draw_messages(_screen);
	draw_cursor(_screen);
}

void editor::draw_cursor(ldv::screen& _screen) {

	ldv::bitmap_representation cursor(cursor_tex);
	cursor.set_blend(ldv::representation::blends::alpha);
	const auto rect=cursor_table.get(tile_delete_mode ? 2 : 1).get_rect();
	cursor.set_clip(rect);
	int x=mouse_pos.x-(rect.w/2),
		y=mouse_pos.y-(rect.h/2);

	//TODO: What about snap to grid???

	cursor.set_location({x, y, rect.w, rect.h});
	cursor.draw(_screen);
}

void editor::draw_messages(ldv::screen& _screen) {

	last_message_rep.draw(_screen);
}

void editor::draw_set(
	ldv::screen& _screen
) {
	struct : tile_editor::const_layer_visitor {

		editor *         controller{nullptr};
		ldv::screen *    screen{nullptr};
		void visit(const tile_editor::tile_layer& _layer) {controller->draw_set(*screen, _layer);}
		void visit(const tile_editor::thing_layer& _layer) {controller->draw_set(*screen, _layer);}
		void visit(const tile_editor::poly_layer& _layer) {controller->draw_set(*screen, _layer);}

	} dispatcher;

	dispatcher.controller=this;
	dispatcher.screen=&_screen;
	dispatch_layer(dispatcher);
}

int editor::draw_set_background(
	ldv::screen& _screen
) {
	unsigned int w=_screen.get_w() * (session.toolbox_width_percent / 100.);

	ldv::box_representation box(
		{0,0, w, _screen.get_h()},
		ldv::rgba8(0,0,0,128)
	);

	box.set_blend(ldv::representation::blends::alpha);
	box.align(
		screen_rect,
		{
			ldv::representation_alignment::h::inner_right,
			ldv::representation_alignment::v::inner_top
		}
	);

	box.draw(_screen);

	return box.get_position().x;
}

void editor::draw_set(
	ldv::screen& _screen,
	const tile_editor::tile_layer& _layer
) {

	//draw the tiles...
	ldv::bitmap_representation bmp(
		*tileset_textures.at(session.tilesets.at(_layer.set).image_path)
	);

	const int background_start{draw_set_background(_screen)};
	const auto item_w{tile_list.get_item_w()},
	                   item_h{tile_list.get_item_h()};

	for(const auto& item : tile_list.get_page()) {

		const int x=background_start+item.x,
				y=item.y;

		const unsigned int w=item_w,
							h=item_h;

		if(item.index==tile_list.get_current_index()) {

			ldv::box_representation current_box(
				{x-1, y-1, w+2, h+2},
				ldv::rgba8(0,0,255,128)
			);

			current_box.draw(_screen);
		}

		bmp.set_clip(item.item.second.get_rect());
		bmp.set_location({x, y, w, h});
		bmp.draw(_screen);
	}
}

void editor::draw_set(
	ldv::screen& _screen,
	const tile_editor::thing_layer& _layer
) {
	const auto background_start{draw_set_background(_screen)};
	const auto item_h{thing_list.get_item_h()};

	for(const auto& item : thing_list.get_page()) {

		const auto prototype=session.thingsets.at(_layer.set).table.at(item.item.type_id);

		draw_set_text(
			_screen,
			background_start,
			item.y,
			item_h,
			prototype.color,
			prototype.name,
			item.index==thing_list.get_current_index()
		);
	}
}

void editor::draw_set(
	ldv::screen& _screen,
	const tile_editor::poly_layer& _layer
) {

	const auto background_start{draw_set_background(_screen)};
	const auto item_h{poly_list.get_item_h()};

	for(const auto& item : poly_list.get_page()) {

		const auto prototype=session.polysets.at(_layer.set).table.at(item.item.poly_id);

		draw_set_text(
			_screen,
			background_start,
			item.y,
			item_h,
			prototype.color,
			prototype.name,
			item.index==poly_list.get_current_index()
		);
	}
}

void editor::draw_set_text(
	ldv::screen& _screen,
	int _x,
	int _y,
	int _h,
	tile_editor::color _color,
	const std::string& _name,
	bool _is_current
) {

	//Color...
	ldv::box_representation color_box(
		{_x, _y, (unsigned)_h , (unsigned)_h},
		ldv::rgba8(_color.r, _color.g, _color.b, _color.a)
	);

	color_box.draw(_screen);

	//Name...
	ldv::ttf_representation txt_name{
		ttf_manager.get(tile_editor::definitions::main_font_name, tile_editor::definitions::main_font_size),
		_is_current
			? ldv::rgba8(0, 0, 255, 255)
			: ldv::rgba8(255, 255, 255, 255),
		_name
	};

	txt_name.go_to({_x+_h+2, _y});
	txt_name.draw(_screen);
}

//TODO: Take me to some toolset I can reuse.
void editor::draw_grid(
	ldv::screen& _screen
) {
	struct : public tile_editor::const_layer_visitor {
		bool show_subgrid{true};
		void visit(const tile_editor::tile_layer&) {show_subgrid=false;}
		void visit(const tile_editor::thing_layer&) {}
		void visit(const tile_editor::poly_layer&) {}
	} dispatcher;
	dispatch_layer(dispatcher);

	auto euclidean_module=[](int a, int b) -> int {

		int m=a%b;
		if(m < 0) {
			return b < 0 ? m-b : m+b;
		}
		return m;
	};

	auto to_color=[](const tile_editor::color _c) -> ldv::rgba_color {

		return ldv::rgba8(_c.r, _c.g, _c.b, _c.a);
	};

	const auto& focus=camera.get_focus_box();

	int x_max=focus.origin.x + focus.w;
	int y_max=focus.origin.y + focus.h;

	//Horizontal lines...
	int module=euclidean_module(focus.origin.x, session.grid_data.size);
	int x=focus.origin.x-module;
	int ruler_units=session.grid_data.horizontal_ruler * session.grid_data.size;

	auto choose_color=[to_color, this, ruler_units](int _value) {

		if(0==_value) {
			return to_color(session.grid_data.origin_color);
		}

		if(0== (_value % ruler_units)) {
			return to_color(session.grid_data.ruler_color);
		}

		if(0 == (_value % session.grid_data.size)) {
			return to_color(session.grid_data.color);
		}

		return to_color(session.grid_data.subcolor);
	};

	while(x < x_max) {

		ldv::line_representation line(
			{x, focus.origin.y},
			{x, y_max},
			choose_color(x)
		);

		line.draw(_screen, camera);

		x+=dispatcher.show_subgrid ? subgrid_factor : session.grid_data.size;
	}

	//Horizontal lines...
	module=euclidean_module(focus.origin.y, session.grid_data.size);
	int y=focus.origin.y-module;
	ruler_units=session.grid_data.vertical_ruler * session.grid_data.size;
	while(y < y_max) {

		ldv::line_representation line(
			{focus.origin.x, y},
			{x_max, y},
			choose_color(y)
		);

		line.draw(_screen, camera);

		y+=dispatcher.show_subgrid ? subgrid_factor : session.grid_data.size;
	}
}

void editor::draw_layers(ldv::screen& _screen) {

	if(!map.layers.size()) {

		return;
	}

	struct :tile_editor::const_layer_visitor {

		ldv::screen * screen{nullptr};
		editor * controller{nullptr};
		void visit(const tile_editor::tile_layer& _layer) {controller->draw_layer(*screen, _layer);}
		void visit(const tile_editor::thing_layer& _layer) {controller->draw_layer(*screen, _layer);}
		void visit(const tile_editor::poly_layer& _layer) {controller->draw_layer(*screen, _layer);}
	} visitor;

	visitor.screen=&_screen;
	visitor.controller=this;

	switch(layer_draw_mode) {

		case layer_draw_modes::all: {
			for(std::size_t index=0; index < map.layers.size(); index++) {
				map.layers[index]->accept(visitor);
			}
		}
		break;

		case layer_draw_modes::stack: {

			for(std::size_t index=current_layer; index < map.layers.size(); index++) {
				map.layers[index]->accept(visitor);
			}
		}
		break;

		case layer_draw_modes::current:
			map.layers[current_layer]->accept(visitor);
		break;
	}
}

void editor::draw_layer(
	ldv::screen& _screen,
	const tile_editor::tile_layer& _layer
) {

	const auto& tileset=session.tilesets.at(_layer.set);
	const auto& table=tileset.table;

	ldv::bitmap_representation bmp(
		*tileset_textures.at(tileset.image_path)
	);

	bmp.set_alpha(_layer.alpha);
	bmp.set_blend(ldv::representation::blends::alpha);

	for(const auto& tile : _layer.data) {

		//Calculate the world position...
		int x=tile.x * session.grid_data.size,
		    y=tile.y * session.grid_data.size;

		unsigned int size=session.grid_data.size;
		bmp.set_location({x, y, size, size});

		//TODO: Check errors with "exists"?
		//TODO: If it fails -> DRAW DEFAULT.
		const auto& rect=table.get(tile.type).get_rect();
		bmp.set_clip(rect);

		bmp.draw(_screen, camera);
	}
}

void editor::draw_layer(
	ldv::screen& _screen,
	const tile_editor::thing_layer& _layer
) {

	ldv::box_representation box(
		{0,0,0,0},
		ldv::rgba8(0,0,0,0)
	);

	box.set_alpha(_layer.alpha);
	box.set_blend(ldv::representation::blends::alpha);


	//Crosshair representations...
	ldv::line_representation vline({0,0},{0,0}, ldv::rgba8(0,0,0, 128)),
		hline({0,0},{0,0}, ldv::rgba8(0,0,0, 128));

	auto crosshair=[&vline, &hline, &_layer, this](int _x, int _y, int _w, int _h, tile_editor::color _color) {

		auto color=ldv::rgba8(255-_color.r, 255-_color.g, 255-_color.b, blend_alpha(_layer.alpha, 128));

		vline.set_points({_x-(_w/2), _y}, {_x+(_w/2), _y});
		vline.set_color(color);
		hline.set_points({_x, _y-(_h/2)}, {_x, _y+(_h/2)});
		hline.set_color(color);
	};

	for(const auto& thing : _layer.data) {

		auto origin=thing_origin_fn(thing.x, thing.y, thing.w, thing.h);
		box.set_filltype(ldv::polygon_representation::type::fill);
		box.set_location({origin.x, origin.y, (unsigned)thing.w, (unsigned)thing.h});
		box.set_color(ldv::rgba8(thing.color.r, thing.color.g, thing.color.b, blend_alpha(_layer.alpha, thing.color.a)));
		box.draw(_screen, camera);

		//The crosshair indicates where the x and y in the map file reside.
		crosshair(thing.x, thing.y, thing.w /4 , thing.h /4, thing.color);
		vline.draw(_screen, camera);
		hline.draw(_screen, camera);

		//Additionally, the current box has a specific hue to it.
		if(selected_thing!=nullptr && selected_thing==&thing) {

			box.set_filltype(ldv::polygon_representation::type::line);
			//TODO: it would be great if there was a glow to this.
			box.set_color(ldv::rgba8(255,255,255, blend_alpha(_layer.alpha, 255)));
			box.draw(_screen, camera);
		}
	}
}

void editor::draw_layer(
	ldv::screen& _screen,
	const tile_editor::poly_layer& _layer
) {

	ldv::polygon_representation shape(
		{{0,0}},
		ldv::rgba8(0,0,0,0)
	);

	shape.set_alpha(_layer.alpha);
	shape.set_blend(ldv::representation::blends::alpha);

	for(const auto& poly : _layer.data) {

		std::vector<ldv::point> points(poly.points.size());
		std::transform(
			std::begin(poly.points),
			std::end(poly.points),
			std::begin(points),
			[](const ldv::point& _point) -> tile_editor::poly_point {

				return {_point.x, -_point.y};
			}
		);

		shape.set_points(points);
		shape.set_color(ldv::rgba8(poly.color.r, poly.color.g, poly.color.b, blend_alpha(_layer.alpha, poly.color.a)));
		shape.draw(_screen, camera);
	}
}

void editor::draw_hud(ldv::screen& _screen) {

	std::stringstream ss;

	ss<<" "<<mouse_pos.x<<","<<mouse_pos.y<<" zoom:"<<camera.get_zoom()<<std::endl;

	if(!map.layers.size()) {

		ss<<"no layers, use lctrl+tab for layer controls";
		return;
	}
	else {

		struct :tile_editor::const_layer_visitor {

			tile_editor::map_blueprint * session{nullptr};
			std::stringstream * ss{nullptr};

			void visit(const tile_editor::tile_layer& _layer) {
				(*ss)<<" tile, set: "<<session->tilesets[_layer.set].name<<" size: "<<_layer.data.size();
			}

			void visit(const tile_editor::thing_layer& _layer) {
				(*ss)<<" thing, set: "<<session->thingsets[_layer.set].name<<" size: "<<_layer.data.size();
			}

			void visit(const tile_editor::poly_layer& _layer) {
				(*ss)<<" poly, set: "<<session->polysets[_layer.set].name<<", size: "<<_layer.data.size();
			}
		} visitor;
		visitor.session=&session;
		visitor.ss=&ss;

		ss<<"'"<<map.layers[current_layer]->id<<"', type ";
		map.layers[current_layer]->accept(visitor);
		ss<<" alpha: "<<map.layers[current_layer]->alpha<<", layer "<<(current_layer+1)<<" / "<<map.layers.size()<<std::endl;

		//Show currently selected thing...
		struct :tile_editor::const_layer_visitor {

			editor * controller{nullptr};
			std::stringstream * ss{nullptr};

			void visit(const tile_editor::tile_layer&) {}

			void visit(const tile_editor::thing_layer& _layer) {
				if(nullptr!=controller->selected_thing) {
					controller->draw_hud_thing_info(*ss, *controller->selected_thing, controller->session.thingsets.at(_layer.set).table.at(controller->selected_thing->type));
				}
			}

			void visit(const tile_editor::poly_layer& _layer) {
				if(nullptr!=controller->selected_poly) {
					controller->draw_hud_poly_info(*ss, *controller->selected_poly, controller->session.polysets.at(_layer.set).table.at(controller->selected_poly->type));
				}
			}
		} dispatcher;

		dispatcher.controller=this;
		dispatcher.ss=&ss;
		map.layers[current_layer]->accept(dispatcher);
	}

	ldv::ttf_representation txt_hud{
		ttf_manager.get(tile_editor::definitions::main_font_name, tile_editor::definitions::main_font_size),
		ldv::rgba8(255, 255, 255, 192),
		""
	};
	txt_hud.set_line_height_ratio(tile_editor::definitions::line_height_ratio);
	txt_hud.go_to({0,0});
	txt_hud.set_text(ss.str());
	txt_hud.draw(_screen);
}

void editor::draw_hud_thing_info(
	std::stringstream& _ss,
	const tile_editor::thing& _thing,
	const tile_editor::thing_definition& _blueprint
) {

	_ss<<_blueprint.name<<" ["<<_blueprint.type_id<<"]"<<std::endl
		<<"x: "<<_thing.x<<std::endl
		<<"y: "<<_thing.y<<std::endl
		<<"w: "<<_thing.w<<std::endl
		<<"h: "<<_thing.h<<std::endl
		<<"color: "<<_thing.color.r<<" "<<_thing.color.g<<" "<<_thing.color.b<<" "<<_thing.color.a<<std::endl;

	auto print_properties=[&_ss](const auto& _props, const auto& _protos) {

		for(const auto& pair : _props) {

			_ss<<"["<<_protos.at(pair.first).name<<"]:"<<pair.second<<std::endl;
		}
	};

	print_properties(_thing.properties.string_properties, _blueprint.properties.string_properties);
	print_properties(_thing.properties.int_properties, _blueprint.properties.int_properties);
	print_properties(_thing.properties.double_properties, _blueprint.properties.double_properties);
}


void editor::draw_hud_poly_info(
	std::stringstream& _ss,
	const tile_editor::poly& _poly,
	const tile_editor::poly_definition& _blueprint
) {

	_ss<<_blueprint.name<<" ["<<_blueprint.poly_id<<"]"<<std::endl
		<<"color: "<<_poly.color.r<<" "<<_poly.color.g<<" "<<_poly.color.b<<" "<<_poly.color.a<<std::endl;

	auto print_properties=[&_ss](const auto& _props, const auto& _protos) {

		for(const auto& pair : _props) {

			_ss<<"["<<_protos.at(pair.first).name<<"]:"<<pair.second<<std::endl;
		}
	};

	print_properties(_poly.properties.string_properties, _blueprint.properties.string_properties);
	print_properties(_poly.properties.int_properties, _blueprint.properties.int_properties);
	print_properties(_poly.properties.double_properties, _blueprint.properties.double_properties);
}

void editor::zoom_in() {

	auto zoom=camera.get_zoom();

	if(zoom > 0.1) {

		camera.set_zoom(zoom/2.);
	}
}

void editor::zoom_out() {

	auto zoom=camera.get_zoom();

	if(zoom < 4) {

		camera.set_zoom(zoom*2.);
	}
}

void editor::receive_message(tools::message_manager::notify_event_type /*_type*/) {

	if(message_manager.size()) {

		last_message_rep.set_text(message_manager.last());

		last_message_rep.align(
			screen_rect,
			ldv::representation_alignment{
				ldv::representation_alignment::h::center,
				ldv::representation_alignment::v::inner_bottom
			}
		);
	}
	else {
		last_message_rep.set_text("");
	}
}

void editor::save_current() {

	tile_editor::map_saver ms{log};

	if(!ms.save(map, current_filename)) {

		message_manager.add("could not save map!");
	}
	else {

		message_manager.add("map saved");
	}
}

void editor::load_map(const std::string& _path) {

	tile_editor::map_loader ml{
		log,
		message_manager,
		session.thingsets,
		session.polysets,
		session.properties
	};
	map=ml.load_from_file(_path);
	current_filename=_path;
	current_layer=0;
	load_layer_toolset();
}

void editor::load_session(const std::string& _path) {

	lm::log(log, lm::lvl::info)<<"map editor will load session data from "<<_path<<std::endl;

	tile_editor::blueprint_parser cfp;
	session=cfp.parse_file(_path);

	subgrid_factor=session.grid_data.size;

	//set the toolbox width...
	int w=screen_rect.w * (session.toolbox_width_percent / 100.);
	tile_list.set_available_w(w);

	//set the thing origin function...
	switch(session.thing_center) {
		case tile_editor::map_blueprint::thing_centers::center:
			thing_origin_fn=[](int _x, int _y, int _w, int _h) -> editor_point {
				return {_x-_w/2, _y-_h/2};
			};
		break;
		case tile_editor::map_blueprint::thing_centers::top_left:
			thing_origin_fn=[](int _x, int _y, int, int) -> editor_point {
				return {_x, _y};
			};
		break;
		case tile_editor::map_blueprint::thing_centers::top_right:
			thing_origin_fn=[](int _x, int _y, int _w, int) -> editor_point {
				return {_x+_w, _y};
			};
		break;
		case tile_editor::map_blueprint::thing_centers::bottom_right:
			thing_origin_fn=[](int _x, int _y, int _w, int _h) -> editor_point {
				return {_x+_w, _y+_h};
			};
		break;
		case tile_editor::map_blueprint::thing_centers::bottom_left:
			thing_origin_fn=[](int _x, int _y, int _w, int) -> editor_point {
				return {_x, _y+_w};
			};
		break;
	}

	//TODO: I would enjoy if this was another component.
	lm::log(log, lm::lvl::info)<<"map editor will load textures..."<<std::endl;
	tileset_textures.clear();
	for(const auto& set : session.tilesets) {

		if(!tileset_textures.count(set.second.image_path)) {

			//TODO: Control failures and shit.
			ldv::image img(set.second.image_path);
			tileset_textures.emplace(
				set.second.image_path,
				std::unique_ptr<ldv::texture>(new ldv::texture(img))
			);

			lm::log(log, lm::lvl::info)<<"loaded "<<set.second.image_path<<std::endl;
		}
	}
}

void editor::previous_layer() {

	if(!map.layers.size() || 0==current_layer) {

		return;
	}

	--current_layer;
	load_layer_toolset();
	layer_change_cleanup();
}

void editor::next_layer() {

	const auto size=map.layers.size();
	if(!size || current_layer+1==size) {

		return;
	}

	++current_layer;
	load_layer_toolset();
	layer_change_cleanup();
}

void editor::toggle_layer_draw_mode() {

	switch(layer_draw_mode) {

		case layer_draw_modes::all:
			layer_draw_mode=layer_draw_modes::stack;
			message_manager.add("layer draw mode: stack");
			return;

		case layer_draw_modes::stack:
			layer_draw_mode=layer_draw_modes::current;
			message_manager.add("layer draw mode: current");
			return;

		case layer_draw_modes::current:
			layer_draw_mode=layer_draw_modes::all;
			message_manager.add("layer draw mode: all");
			return;
	}
}

void editor::load_layer_toolset() {

	tile_editor::set_layer_loader ll{
		tile_list,
		thing_list,
		poly_list,
		session.tilesets,
		session.thingsets,
		session.polysets
	};

	dispatch_layer(ll);
}

ldt::point_2d<int> editor::get_world_position(ldt::point_2d<int> _pos) const {

	_pos.x/=camera.get_zoom();
	_pos.y/=camera.get_zoom();

	_pos.x+=camera.get_x();
	_pos.y+=camera.get_y();

	return {_pos.x, _pos.y};
}

ldt::point_2d<int> editor::get_grid_position(ldt::point_2d<int> _point) const {

	double size=session.grid_data.size;

	return {
		(int)(floor(_point.x / size)) ,
		(int)(floor(_point.y / size))
	};
}

void editor::layer_change_cleanup() {

	selected_poly=nullptr;
	selected_thing=nullptr;
	multiclick.engaged=false;
	tile_delete_mode=false;
	subgrid_factor=session.grid_data.size;
}

void editor::open_thing_properties(
	tile_editor::property_manager& _properties,
	tile_editor::thing_definition& _blueprint
) {
	exchange_data.edited_thing=selected_thing;
	exchange_data.edited_thing_blueprint=&_blueprint;
	exchange_data.properties=&_properties;
	exchange_data.properties_blueprint=&_blueprint.properties;
	exchange_data.put(state_properties);
	push_state(state_properties);
}

void editor::open_map_properties() {

	exchange_data.properties=&map.properties;
	exchange_data.properties_blueprint=&session.properties;
	exchange_data.put(state_properties);
	push_state(state_properties);
}

void editor::open_layer_selector() {

	exchange_data.current_layer=&current_layer;
	exchange_data.layers=&map.layers;
	exchange_data.put(state_layer_selector);
	push_state(state_layer_selector);
}

void editor::open_layer_settings() {

	struct :public tile_editor::layer_visitor {
		editor * controller{nullptr};
		void visit(tile_editor::tile_layer& _layer) {

			controller->exchange_data.layer=&_layer;
			controller->exchange_data.blueprint=&controller->session;
			start(state_tile_editor_properties);
		}
		void visit(tile_editor::thing_layer& _layer) {

			controller->exchange_data.layer=&_layer;
			controller->exchange_data.blueprint=&controller->session;
			start(state_thing_editor_properties);
		}
		void visit(tile_editor::poly_layer&) {}

		private:
		void start(int _state) {
			controller->exchange_data.put(_state);
			controller->push_state(_state);
		}
	} dispatcher;
	dispatcher.controller=this;
	dispatch_layer(dispatcher);
}

void editor::make_subgrid_smaller() {

	if((int)subgrid_factor <= session.grid_data.size / 8) {

		message_manager.add("fine grid is already at its smallest");
		return;
	}

	subgrid_factor/=2;
	message_manager.add(std::string{"fine grid set at "}+std::to_string(subgrid_factor));
}

void editor::make_subgrid_larger() {

	if((int)subgrid_factor==session.grid_data.size) {

		message_manager.add("fine grid is already at its largest");
		return;
	}

	subgrid_factor*=2;
	message_manager.add(std::string{"fine grid set at "}+std::to_string(subgrid_factor));
}

bool editor::dispatch_layer(tile_editor::const_layer_visitor& _dispatcher) {

	if(!map.layers.size()) {

		return false;
	}

	map.layers.at(current_layer)->accept(_dispatcher);
	return true;
}

bool editor::dispatch_layer(tile_editor::layer_visitor& _dispatcher) {

	if(!map.layers.size()) {

		return false;
	}

	map.layers.at(current_layer)->accept(_dispatcher);
	return true;
}

editor::editor_point editor::snap_to_grid(editor_point _point) const {

	double  x=_point.x,
	        y=_point.y,
	        factor=subgrid_factor;

	int px=round(x / factor) * factor;
	int py=round(y / factor) * factor;

	return {px, py};
}

int editor::blend_alpha(
	int _layer_alpha,
	int _entity_alpha
) const {

	return floor( ((double)_layer_alpha / 255.) * _entity_alpha);
}

void editor::toggle_set_gui() {

	show_set=!show_set;
	message_manager.add(
		show_set
			? "set selector enabled, camera movement disabled"
			: "set selector disabled, camera movement enabled"
	);
}

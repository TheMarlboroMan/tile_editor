#include "controller/editor.h"

//local
#include "input/input.h"
#include "app/map_loader.h"
#include "app/map_saver.h"
#include "app/map_loader.h"
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
		ttf_manager.get("main", 14),
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
}

void editor::loop(dfw::input& _input, const dfw::loop_iteration_data& /*_lid*/) {

	if(_input().is_exit_signal() || _input.is_input_down(input::escape)) {

		set_leave(true);
		return;
	}

	if(_input.is_input_down(input::help)) {

		push_state(state_help);
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

		toggle_layer_draw_mode();
		return;
	}

	if(_input.is_input_down(input::tab)) {

		show_set=!show_set;
	}

	auto mpos=_input().get_mouse_position();
	mouse_pos={mpos.x, mpos.y};

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

	if(_input.is_input_down(input::left_click)
		|| _input.is_input_down(input::right_click)) {

		int click_modifiers=click_modifier_none;

		if(_input.is_input_pressed(input::lshift)) {

			click_modifiers|=click_modifier_lshift;
		}

		if(_input.is_input_pressed(input::lctrl)) {

			click_modifiers|=click_modifier_lctrl;
		}

		if(_input.is_input_down(input::left_click)) {
			click_input(input::left_click, click_modifiers);
			return;
		}

		if(_input.is_input_down(input::right_click)) {
			click_input(input::right_click, click_modifiers);
			return;
		}

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

			 arrow_input_set(movement_x, movement_y);
		}

		return;
	}
	else {

		typedef  bool (dfw::input::*input_fn)(int) const;
		input_fn movement_fn=_input.is_input_pressed(input::lctrl)
			? &dfw::input::is_input_down
			: &dfw::input::is_input_pressed;

		const int factor=_input.is_input_pressed(input::lctrl)
			? session.grid_data.size / 4
			: session.grid_data.size / 2;

		int movement_x=0,
			movement_y=0;

		if(std::invoke(movement_fn, _input, input::up)) {

			movement_y=-1*factor;
		}
		if(std::invoke(movement_fn, _input, input::down)) {

			movement_y=1*factor;
		}

		if(std::invoke(movement_fn, _input, input::left)) {

			movement_x=-1*factor;
		}
		if(std::invoke(movement_fn, _input, input::right)) {

			movement_x=1*factor;
		}

		if(movement_x || movement_y) {

			 arrow_input_map(movement_x, movement_y);
		}

		return;
	}
}

void editor::del_input() {

	struct : public tile_editor::const_layer_visitor {
		editor * controller{nullptr};
		void visit(const tile_editor::tile_layer&) {controller->tile_delete_mode=!controller->tile_delete_mode;}
		void visit(const tile_editor::thing_layer&) {}
		void visit(const tile_editor::poly_layer&) {}
	} dispatcher;
	dispatcher.controller=this;
	dispatch_layer(dispatcher);
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
		void visit(tile_editor::thing_layer&) {}
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
	if(! (_modifiers & click_modifier_lshift)) {

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
		}
	}
}

void editor::arrow_input_set(
	int _movement_x,
	int _movement_y
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

void editor::arrow_input_map(
	int _movement_x,
	int _movement_y
) {

	camera.move_by(_movement_x, _movement_y);
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
		ttf_manager.get("main", 14),
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

	ldv::line_representation vline({0,0},{0,0}, ldv::rgba8(0,0,0, 128)),
		hline({0,0},{0,0}, ldv::rgba8(0,0,0, 128));

	//Sets the crosshair position
	auto crosshair=[&vline, &hline](int _x, int _y, int _w, int _h, tile_editor::color _color) {

		auto color=ldv::rgba8(255-_color.r, 255-_color.g, 255-_color.b, 128);

		vline.set_points({_x-(_w/2), _y}, {_x+(_w/2), _y});
		vline.set_color(color);
		hline.set_points({_x, _y-(_h/2)}, {_x, _y+(_h/2)});
		hline.set_color(color);
	};

	auto thing_center=[](int _x, int _y, int _w, int _h, tile_editor::map_blueprint::thing_centers _center) -> editor_point {

		using namespace tile_editor;

		switch(_center) {
			case map_blueprint::thing_centers::center: return {_x-_w/2, _y-_h/2};
			case map_blueprint::thing_centers::top_left: return {_x, _y};
			case map_blueprint::thing_centers::top_right: return {_x+_w, _y};
			case map_blueprint::thing_centers::bottom_right: return {_x+_w, _y+_h};
			case map_blueprint::thing_centers::bottom_left: return {_x, _y+_w};
		}

		return {_x, _y};
	};

	for(const auto& thing : _layer.data) {

		auto center=thing_center(thing.x, thing.y, thing.w, thing.h, session.thing_center);
		box.set_location({center.x, center.y, (unsigned)thing.w, (unsigned)thing.h});
		box.set_color(ldv::rgba8(thing.color.r, thing.color.g, thing.color.b, thing.color.a));
		box.draw(_screen, camera);

		//The crosshair indicates where the x and y in the map file reside.
		crosshair(thing.x, thing.y, thing.w /4 , thing.h /4, thing.color);
		vline.draw(_screen, camera);
		hline.draw(_screen, camera);
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
		shape.set_color(ldv::rgba8(poly.color.r, poly.color.g, poly.color.b, poly.color.a));
		shape.draw(_screen, camera);
	}
}

void editor::draw_hud(ldv::screen& _screen) {

	std::stringstream ss;

	if(!map.layers.size()) {

		ss<<"no layers";
	}
	else {

		struct :tile_editor::const_layer_visitor {

			tile_editor::map_blueprint * session{nullptr};
			std::string contents;

			void visit(const tile_editor::tile_layer& _layer) {
				contents=std::string{" type: tile, set: "}+session->tilesets[_layer.set].name+" size: "+std::to_string(_layer.data.size());
			}

			void visit(const tile_editor::thing_layer& _layer) {
				contents=std::string{" type: thing, set: "}+session->thingsets[_layer.set].name+" size: "+std::to_string(_layer.data.size());
			}

			void visit(const tile_editor::poly_layer& _layer) {
				contents=std::string{" type: poly, set: "}+session->polysets[_layer.set].name+" size: "+std::to_string(_layer.data.size());
			}
		} visitor;

		visitor.session=&session;
		map.layers[current_layer]->accept(visitor);

		ss<<"layer "
			<<" '"<<map.layers[current_layer]->id<<"'"
			<<visitor.contents
			<<" "<<(current_layer+1)<<" / "<<map.layers.size();
	}

	ss<<" "<<mouse_pos.x<<","<<mouse_pos.y<<" zoom:"<<camera.get_zoom();

	ldv::ttf_representation txt_hud{
		ttf_manager.get("main", 14),
		ldv::rgba8(255, 255, 255, 192),
		""
	};

	txt_hud.go_to({0,0});
	txt_hud.set_text(ss.str());
	txt_hud.draw(_screen);
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
		session.polysets
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
}

void editor::open_layer_settings() {

	struct :public tile_editor::layer_visitor {
		editor * controller{nullptr};
		void visit(tile_editor::tile_layer& _layer) {

			controller->exchange_data.layer=&_layer;
			controller->exchange_data.blueprint=&controller->session;
			start(state_tile_editor_properties);
		}
		void visit(tile_editor::thing_layer&) {}
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

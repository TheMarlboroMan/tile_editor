#include "controlador_rejilla.h"
#include "../app/recursos.h"
#include <sstream>

using namespace DLibV;
using namespace DLibH;

extern Log_base LOG;

Controlador_rejilla::Controlador_rejilla(Director_estados &DI, Pantalla& pantalla, const std::string& nombre_fichero, const Contenedor_tilesets& t, const Contenedor_logica_sets& s)
	:Controlador_base(DI), 
	
	info_zoom(1, pantalla.acc_w(), pantalla.acc_h()),
	nombre_fichero(nombre_fichero),
	renderer(pantalla.acc_renderer()),
	tilesets(t),
	sets_tipo_logica(s),
	camara(0, 0, pantalla.acc_w(), pantalla.acc_h()),
	rep_tiles(),
	rep_info_capa(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), ""),
	rep_info_pos(pantalla.acc_renderer(), DLibV::Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), ""),
	rejilla_actual(0),
	capa_logica_actual(0),
	modo_actual(modo_operacion::REJILLA),
	objeto_logica_actual(nullptr),
	listado_tiles(W_LISTADOS, 64/*pantalla.acc_h()*/, DIM_LISTADO_REJILLA, DIM_LISTADO_REJILLA),
	listado_logica(W_LISTADOS, ALTURA_LISTADO_VERTICAL),
	rep_listado_tiles(true),
	rep_listado_logica(true),
	rep_seleccion_actual(
			DLibH::Herramientas_SDL::nuevo_sdl_rect(0, 0, DIM_LISTADO_REJILLA, DIM_LISTADO_REJILLA),
			64, 64, 192)
{
	rep_seleccion_actual.establecer_alpha(128);
	rep_info_pos.establecer_posicion(16, 16);
	rep_info_capa.establecer_posicion(16, 32);

	rep_tiles.establecer_modo_blend(Representacion::BLEND_ALPHA);

	rep_listado_tiles.establecer_posicion(pantalla.acc_w()-W_LISTADOS, 0),
	rep_listado_tiles.establecer_modo_blend(DLibV::Representacion::BLEND_ALPHA);

	rep_listado_logica.establecer_posicion(pantalla.acc_w()-W_LISTADOS, 0),
	rep_listado_logica.establecer_modo_blend(DLibV::Representacion::BLEND_ALPHA);
}

void Controlador_rejilla::inicializar_sin_fichero()
{
	try
	{
		rejillas.push_back(Rejilla(64, 64, 32, 32, 8, 8, tilesets[0]));
		capas_logica.push_back(Capa_logica(sets_tipo_logica[0]));
		inicializar();
	}
	catch(Contenedor_tilesets_exception& e)
	{
		LOG<<e.what()<<"\n";
	}
}

void Controlador_rejilla::inicializar()
{
	auto r=*rejillas.begin();
	preparar_listado_tiles(r.acc_gestor());
	preparar_listado_logica(capas_logica.begin()->acc_gestor());
	camara.establecer_limites(0, 0, r.acc_w() * r.acc_w_celda(), r.acc_h()*r.acc_h_celda());
	reconstruir_rep_info_posicion(Info_input(), r);
	reconstruir_rep_info_con_rejilla(r);
}

/**
* Rellenar el objeto del listado con las tiles que correspondan...
*/

void Controlador_rejilla::preparar_listado_tiles(const Tile_set& s)
{
	listado_tiles.clear();
	const auto& v=s.acc_items();
	for(const auto& tile : v) listado_tiles.insertar(Item_tile{tile});

	rep_listado_tiles.vaciar_grupo();
	const auto pagina=listado_tiles.obtener_pagina();
	auto * rec=DLibV::Gestor_texturas::obtener(s.acc_indice_recurso());
	using BMP=DLibV::Representacion_bitmap_estatica;

	for(const auto& itemp : pagina)
	{
		BMP * bmp=new BMP(rec);
		const auto& tile=itemp.item.tile;
		bmp->establecer_recorte(tile.acc_x(), tile.acc_y(), tile.acc_w(), tile.acc_h());
		bmp->establecer_posicion(itemp.x, itemp.y, 32, 32);
		rep_listado_tiles.insertar_representacion(bmp);
	}
}

void Controlador_rejilla::preparar_listado_logica(const Logica_set& s)
{
	listado_logica.clear();
	const auto& v=s.acc_items();
	for(const auto& logica : v) listado_logica.insertar(Item_logica{logica});

	rep_listado_logica.vaciar_grupo();
	const auto pagina=listado_logica.obtener_pagina();

	using CAJA=DLibV::Representacion_primitiva_caja_estatica;
	using TXT=DLibV::Representacion_texto_auto_estatica;

	for(const auto& itemp : pagina)
	{
		const auto& l=itemp.item.logica;
		CAJA * rep_caja=new CAJA(Herramientas_SDL::nuevo_sdl_rect(2, itemp.y, 6, 6), l.acc_r_editor(), l.acc_g_editor(), l.acc_b_editor());
		TXT * rep_txt=new TXT(renderer, Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), l.acc_nombre());
		rep_txt->establecer_posicion(10, itemp.y);

		rep_listado_logica.insertar_representacion(rep_caja);
		rep_listado_logica.insertar_representacion(rep_txt);
	}
}

void Controlador_rejilla::loop(Input_base& input, float delta)
{
	if(input.es_senal_salida() || input.es_input_down(Input::I_ESCAPE))
	{
		abandonar_aplicacion();
	}
	else
	{
		auto ii=recoger_input(input);

		switch(modo_actual)
		{	
			case modo_operacion::REJILLA: procesar_input_rejilla(ii, rejillas[rejilla_actual]); break;
			case modo_operacion::CAPA_LOGICA: procesar_input_capa_logica(ii, capas_logica[capa_logica_actual]); break;
		}
	}
}

void Controlador_rejilla::dibujar(Pantalla& pantalla)
{
	pantalla.limpiar(128, 128, 128, 255);

	auto& r=rejillas[rejilla_actual];
	auto& c=capas_logica[capa_logica_actual];
	const int wc=r.acc_w_celda(), hc=r.acc_h_celda();

	dibujar_rejilla(pantalla, wc, hc, wc * r.acc_w(), hc * r.acc_h(), r.acc_w_unidades_separador(), r.acc_h_unidades_separador());

	for(auto& rej : rejillas)
	{
		if(rej.es_visible_siempre() || (modo_actual==modo_operacion::REJILLA && &rej==&r) )
		{
			dibujar_celdas(pantalla, rej);
		}
	}

	for(auto& capa : capas_logica)
	{
		if(capa.es_visible_siempre() || (modo_actual==modo_operacion::CAPA_LOGICA && &capa==&c) )
		{
			dibujar_capa_logica(pantalla, capa);
		}
	}

	dibujar_hud_rejillas(pantalla);

	switch(modo_actual)
	{
		case modo_operacion::REJILLA: rep_listado_tiles.volcar(pantalla); break;
		case modo_operacion::CAPA_LOGICA: rep_listado_logica.volcar(pantalla); break;
	}

	rep_seleccion_actual.volcar(pantalla);
	rep_info_pos.volcar(pantalla);
	rep_info_capa.volcar(pantalla);
}

void Controlador_rejilla::dibujar_rejilla(Pantalla& pantalla, const int w_rejilla, const int h_rejilla, const int w_nivel, const int h_nivel, const int w_unidades_separador, const int h_unidades_separador)
{
	Representacion_primitiva_linea_estatica lin(0, 0, 0, 0, 0, 0, 0);
	auto const foco=camara.acc_caja_foco();
	//TODO: Esto sigue sin estar bien... Estamos desperdiciando ciclos intentando dibujar fuera de la pantalla.
	const int max_x=w_nivel, max_y=h_nivel;
	int x=(w_rejilla - foco.x) % w_rejilla, y=(h_rejilla - foco.y) % h_rejilla, px=0, py=0;

	//En vertical... Solución de mierda para un problema de mierda.
	int linea_x_actual=foco.x && foco.x < w_rejilla ? 1 : floor( (float)foco.x / (float)w_rejilla); 
	
	py=ceil((float)max_y / (float)info_zoom.zoom);
	while(x < max_x)
	{
		(linea_x_actual % w_unidades_separador==0) ? lin.mut_rgb(0, 0, 255) : lin.mut_rgb(0, 0, 0);
		px=ceil((float)x / (float)info_zoom.zoom);
		lin.establecer_puntos(px, 0, px, py);
		lin.volcar(pantalla);
		x+=w_rejilla;
		++linea_x_actual;
	}

	//En horizontal...
	int linea_y_actual=foco.y && foco.y < h_rejilla ? 1 : floor( (float)foco.y / (float)h_rejilla);
	px=ceil((float)max_x / (float)info_zoom.zoom);
	while(y < max_y)
	{
		(linea_y_actual % h_unidades_separador == 0) ? lin.mut_rgb(0, 0, 255) : lin.mut_rgb(0, 0, 0);
		py=ceil((float)y / (float)info_zoom.zoom);
		lin.establecer_puntos(0, py, px, py);
		lin.volcar(pantalla);
		y+=h_rejilla;
		++linea_y_actual;
	}
}

void Controlador_rejilla::dibujar_capa_logica(Pantalla& pantalla, Capa_logica& capa)
{
	const auto& v=capa.acc_objetos();

	Representacion_primitiva_caja_dinamica caja(Herramientas_SDL::nuevo_sdl_rect(0, 0, 0, 0), 0, 0, 0);

	for(const auto& obj : v)
	{
		int t=obj.acc_tipo();
		auto cb=[t](const Logica& l) {return l.acc_tipo()==t;};
		auto tipo=capa.acc_gestor().buscar_unico_callback(cb);
		if(!tipo)
		{
			dibujar_elemento_desconocido(pantalla, obj.acc_x(), obj.acc_y());
		}
		else
		{
			caja.establecer_posicion(obj.acc_x(), obj.acc_y(), tipo->acc_w_editor(), tipo->acc_h_editor());
			caja.mut_rgb(tipo->acc_r_editor(), tipo->acc_g_editor(), tipo->acc_b_editor());
			caja.establecer_alpha(&obj==objeto_logica_actual ? 255 : 192);
		}

		caja.volcar(pantalla, camara);
	}
}

void Controlador_rejilla::dibujar_elemento_desconocido(Pantalla& pantalla, int x, int y)
{
	rep_tiles.establecer_textura(Gestor_texturas::obtener(Recursos_graficos::RT_DEFECTO));
	rep_tiles.establecer_posicion(x, y, 32, 32);
	rep_tiles.establecer_recorte(0, 0, 32, 32);
	rep_tiles.volcar(pantalla, camara);
}

void Controlador_rejilla::dibujar_celdas(Pantalla& pantalla, Rejilla& rejilla)
{
	//TODO... Sacar sólo las que queden dentro de la cámara...
	auto& celdas=rejilla.r.acc_matriz();
	rep_tiles.establecer_textura(Gestor_texturas::obtener(rejilla.acc_indice_recurso()));

	for(const Matriz_2d<Celda>::par& p : celdas)
	{
		auto coord=coordenadas_mundo_desde_rejilla(p.second.acc_x(), p.second.acc_y(), rejilla.acc_w_celda(), rejilla.acc_h_celda());

		try
		{
			auto tile=rejilla.acc_gestor().at(p.second.acc_indice_tile()-1);
			rep_tiles.establecer_posicion(coord.x, coord.y, tile.acc_w(), tile.acc_h());
			rep_tiles.establecer_recorte(tile.acc_x(), tile.acc_y(), tile.acc_w(), tile.acc_h());
			rep_tiles.volcar(pantalla, camara);
		}
		//Si el tile no existe dibujaremos un "?".
		catch(std::exception& e )
		{
			dibujar_elemento_desconocido(pantalla, coord.x, coord.y);
			rep_tiles.establecer_textura(Gestor_texturas::obtener(rejilla.acc_indice_recurso()));
		}
	}
}

void Controlador_rejilla::dibujar_hud_rejillas(Pantalla& pantalla)
{
	const int ancho=20, alto=10, margen=4;
	int pos_y=pantalla.acc_h()-margen-alto;

	rep_tiles.establecer_textura(Gestor_texturas::obtener(Recursos_graficos::RT_DEFECTO));
	Representacion_primitiva_caja_estatica caja(Herramientas_SDL::nuevo_sdl_rect(margen, pos_y, ancho, alto), 216, 216, 216);	

	for(size_t i=0; i < rejillas.size(); ++i)
	{
		if(i==rejilla_actual) caja.mut_rgb(227, 132, 74);
		else caja.mut_rgb(216, 216, 216);

		int x=margen+(i* (ancho+margen));
		caja.establecer_posicion(x, pos_y);
		caja.volcar(pantalla);

		if(rejillas[i].es_visible_siempre())
		{
			rep_tiles.establecer_posicion(x, pos_y, ancho, alto);
			rep_tiles.establecer_recorte(32, 0, ancho, alto);
			rep_tiles.volcar(pantalla);
		}
	}

	pos_y-=(alto+margen);

	for(size_t i=0; i < capas_logica.size(); ++i)
	{
		if(i==capa_logica_actual) caja.mut_rgb(132, 227, 74);
		else caja.mut_rgb(216, 216, 216);

		int x=margen+(i* (ancho+margen));

		caja.establecer_posicion(x, pos_y);
		caja.volcar(pantalla);

		if(capas_logica[i].es_visible_siempre())
		{
			rep_tiles.establecer_posicion(x, pos_y, ancho, alto);
			rep_tiles.establecer_recorte(32, 0, ancho, alto);
			rep_tiles.volcar(pantalla);
		}
	}
}

Controlador_rejilla::Info_input Controlador_rejilla::recoger_input(const Input_base& input)
{
	int x=0, y=0, downx=0, downy=0;

	if(input.es_input_pulsado(Input::I_ABAJO)) y=1;
	else if(input.es_input_pulsado(Input::I_ARRIBA)) y=-1; 

	if(input.es_input_pulsado(Input::I_DERECHA)) x=1;
	else if(input.es_input_pulsado(Input::I_IZQUIERDA)) x=-1;

	if(input.es_input_down(Input::I_ABAJO)) downy=1;
	else if(input.es_input_down(Input::I_ARRIBA)) downy=-1; 

	if(input.es_input_down(Input::I_DERECHA)) downx=1;
	else if(input.es_input_down(Input::I_IZQUIERDA)) downx=-1;

	if(input.es_input_down(Input::I_ESPACIO)) 
	{	
		if(modo_actual==modo_operacion::CAPA_LOGICA) deseleccionar_objeto_logica_actual();
	}
	else if(input.es_input_pulsado(Input::I_RSHIFT) || input.es_input_pulsado(Input::I_RCONTROL) || input.es_input_pulsado(Input::I_ESPACIO))
	{
		x=0; y=0;

		if(input.es_input_down(Input::I_ABAJO)) y=1;
		else if(input.es_input_down(Input::I_ARRIBA)) y=-1; 
		if(input.es_input_down(Input::I_DERECHA)) x=1;
		else if(input.es_input_down(Input::I_IZQUIERDA)) x=-1;

		if(x || y)
		{
			if(input.es_input_pulsado(Input::I_RSHIFT))
			{
				auto& r=rejillas[rejilla_actual];
				int w=r.acc_w()+x, h=r.acc_h()+y;
				if(w && h) 
				{
					redimensionar_rejilla(w, h, r);
					camara.establecer_limites(0, 0, r.acc_w() * r.acc_w_celda(), r.acc_h()*r.acc_h_celda());
				}
			}
			else if(input.es_input_pulsado(Input::I_RCONTROL))
			{
				auto& r=rejillas[rejilla_actual];
				r=r.copiar_mover(x, y);
				reconstruir_rep_info_con_rejilla(r);
			}
			else if(input.es_input_pulsado(Input::I_ESPACIO))
			{
				auto& r=rejillas[rejilla_actual];
				r.reajustar_espaciado(x, y);
				reconstruir_rep_info_con_rejilla(r);
			}
		}
	
		x=0; y=0;
	}
	else if(input.es_input_down(Input::I_MODO_AYUDA)) 
	{
		solicitar_cambio_estado(Director_estados::t_estados::AYUDA);
	}
	else if(input.es_input_down(Input::I_MODO_REJILLA)) 
	{
		if(modo_actual==modo_operacion::CAPA_LOGICA) 
		{
			modo_actual=modo_operacion::REJILLA;
			const auto& pos=rep_seleccion_actual.acc_posicion();
			rep_seleccion_actual.establecer_posicion(pos.x, pos.y, DIM_LISTADO_REJILLA, DIM_LISTADO_REJILLA);
		}
	}
	else if(input.es_input_down(Input::I_MODO_OBJETOS)) 
	{
		if(modo_actual==modo_operacion::REJILLA) 
		{
			modo_actual=modo_operacion::CAPA_LOGICA;
			const auto& pos=rep_seleccion_actual.acc_posicion();
			rep_seleccion_actual.establecer_posicion(pos.x, pos.y, W_LISTADOS, ALTURA_LISTADO_VERTICAL);
		}
	}
	else if(input.es_input_down(Input::I_MODO_META)) 
	{
		solicitar_cambio_estado(Director_estados::t_estados::PROPIEDADES_META);
	}
	else if(input.es_input_down(Input::I_CICLAR_REJILLAS_1) || 
		input.es_input_down(Input::I_CICLAR_REJILLAS_2) || 
		input.es_input_down(Input::I_CICLAR_REJILLAS_3) || 
		input.es_input_down(Input::I_CICLAR_REJILLAS_4) || 
		input.es_input_down(Input::I_CICLAR_REJILLAS_5) || 
		input.es_input_down(Input::I_CICLAR_REJILLAS_6))
	{
		size_t indice=input.es_input_down(Input::I_CICLAR_REJILLAS_1) ? 0 :
			input.es_input_down(Input::I_CICLAR_REJILLAS_2) ? 1 :
			input.es_input_down(Input::I_CICLAR_REJILLAS_3) ? 2 : 
			input.es_input_down(Input::I_CICLAR_REJILLAS_4) ? 3 : 
			input.es_input_down(Input::I_CICLAR_REJILLAS_5) ? 4 : 5;

		switch(modo_actual)
		{
			case modo_operacion::REJILLA: 
				if(input.es_input_pulsado(Input::I_CONTROL)) intercambiar_visibilidad_parte(rejillas, indice);
				else seleccionar_rejilla_actual(indice);
			break;
			case modo_operacion::CAPA_LOGICA: 
				if(input.es_input_pulsado(Input::I_CONTROL)) intercambiar_visibilidad_parte(capas_logica, indice);
				else seleccionar_capa_logica_actual(indice);
			break;
		}
	}
	else if(input.es_input_down(Input::I_TAB)) 
	{		
		switch(modo_actual)
		{
			case modo_operacion::REJILLA: 
				rep_listado_tiles.intercambiar_visibilidad(); 
				rep_seleccion_actual.cambiar_visibilidad(rep_listado_tiles.es_visible());
			break;
			case modo_operacion::CAPA_LOGICA:
				rep_listado_logica.intercambiar_visibilidad(); 
				rep_seleccion_actual.cambiar_visibilidad(rep_listado_logica.es_visible());
			break;
		}		
	}
	else if(input.es_input_down(Input::I_SWAP_REJILLAS)) 
	{
		switch(modo_actual)
		{
			case modo_operacion::REJILLA:
				swap_parte(rejillas, rejilla_actual);
				reconstruir_rep_info_con_rejilla(rejillas[rejilla_actual]);
			break;
			case modo_operacion::CAPA_LOGICA: swap_parte(capas_logica, capa_logica_actual); break;
		}
	}
	else if(input.es_input_down(Input::I_CICLAR_TILESETS)) 
	{
		switch(modo_actual)
		{
			case modo_operacion::REJILLA: 
				ciclar_set_listado(rejillas[rejilla_actual], tilesets); 
				preparar_listado_tiles(rejillas[rejilla_actual].acc_gestor());
			break;
			case modo_operacion::CAPA_LOGICA:
				ciclar_set_listado(capas_logica[capa_logica_actual], sets_tipo_logica); 
				preparar_listado_logica(capas_logica[capa_logica_actual].acc_gestor()); 
			break;
		}
	}
	else if(input.es_input_down(Input::I_NUEVO)) 
	{
		switch(modo_actual)
		{
			case modo_operacion::REJILLA: insertar_rejilla(); break;
			case modo_operacion::CAPA_LOGICA: insertar_capa_logica(); break;
		}
	}
	else if(input.es_input_down(Input::I_DELETE)) 
	{
		switch(modo_actual)
		{
			case modo_operacion::REJILLA: eliminar_parte(rejillas, rejilla_actual, &Controlador_rejilla::seleccionar_rejilla_actual); break;
			case modo_operacion::CAPA_LOGICA: eliminar_parte(capas_logica, capa_logica_actual, &Controlador_rejilla::seleccionar_capa_logica_actual); break;
		}
	}
	else if(input.es_input_down(Input::I_CICLAR_ZOOM)) ciclar_zoom();
	else if(input.es_input_down(Input::I_GUARDAR)) guardar();
	else if(input.es_input_down(Input::I_CARGAR)) cargar();
	else if(input.es_input_down(Input::I_AV_PAG)) pasar_pagina_selector(1);
	else if(input.es_input_down(Input::I_RE_PAG)) pasar_pagina_selector(-1);
	else if(input.es_input_down(Input::I_SIGUIENTE_TILE)) cambiar_tile_selector(1);
	else if(input.es_input_down(Input::I_ANTERIOR_TILE)) cambiar_tile_selector(-1);

	//TODO: Al mover sólo la cámara no se refresca esto.
	auto pos_raton=input.acc_posicion_raton();

	//TODO: Mover a otra función, usar cuando sea conveniente: esto es, cuando se cambie 
	//realmente el item actual (pasar página, recibir click...).
	switch(modo_actual)
	{
		case modo_operacion::REJILLA: 
		{
			const auto l=listado_tiles.linea_actual();
			rep_seleccion_actual.establecer_posicion(rep_listado_tiles.acc_posicion().x + l.x, l.y);
		}
		break;
		case modo_operacion::CAPA_LOGICA:
		{
			const auto l=listado_logica.linea_actual();
			rep_seleccion_actual.establecer_posicion(rep_listado_logica.acc_posicion().x, l.y);
		}
		break;
	}

	return Info_input(x, y, pos_raton.x, pos_raton.y, 
		input.es_input_down(Input::I_CLICK_I), input.es_input_down(Input::I_CLICK_D), 
		input.es_movimiento_raton(), 
		input.es_input_pulsado(Input::I_SHIFT),
		input.es_input_pulsado(Input::I_CONTROL),
		input.es_input_pulsado(Input::I_ESPACIO),
		downx, downy
		);
}

void Controlador_rejilla::procesar_input_capa_logica(Info_input&ii, Capa_logica& capa)
{
	//Se usará la rejilla para snap to grid y ver el multiplicador de movimiento.
	auto &r=rejillas[rejilla_actual];

	//Comprobar si movemos la cámara o el objeto actual.

	if(ii.x || ii.y || ii.down_x || ii.down_y)
	{
		if(objeto_logica_actual)
		{
			if(ii.shift) objeto_logica_actual->movimiento_relativo(ii.down_x, ii.down_y);
			else if(ii.control) objeto_logica_actual->movimiento_relativo(ii.down_x * r.acc_w_celda(), ii.down_y * r.acc_h_celda());
			else objeto_logica_actual->movimiento_relativo(ii.x , ii.y);
		}
		else
		{
			mover_camara(ii);
		}
	}

	//Comprobar si seleccionamos, creamos o borramos...
	if(ii.click_i || ii.click_d)
	{
		//TODO TODO
//		if(selector_tipo_logica.recibe_click(ii.raton_x, ii.raton_y))
//		{
//			int indice=selector_tipo_logica.procesar_click(ii.raton_x, ii.raton_y);
//			if(indice) capa.mut_indice_actual(indice);
//		}
		if(false) {}
		else
		{
			camara.transformar_posicion_raton(ii.raton_x, ii.raton_y);
			auto coords=coordenadas_mundo(ii.raton_x, ii.raton_y);

			if(ii.click_i)
			{
				auto obj=capa.obtener_objeto(coords.x, coords.y);

				if(obj==nullptr)
				{
					int x=(coords.x / r.acc_w_celda()) * r.acc_w_celda();
					int y=(coords.y / r.acc_h_celda()) * r.acc_h_celda();
					capa.insertar_objeto(x, y);
				}
				else
				{
					if(objeto_logica_actual && objeto_logica_actual==obj)
					{
						solicitar_cambio_estado(Director_estados::t_estados::PROPIEDADES_OBJETO_LOGICA);
					}
					else
					{
						objeto_logica_actual=obj;
						reconstruir_rep_info_con_objeto_logica(*objeto_logica_actual);
					}
				}
			}
			else if(ii.click_d)
			{
				auto obj=capa.obtener_objeto(coords.x, coords.y);
				if(obj)
				{
					if(objeto_logica_actual && *obj==*objeto_logica_actual) objeto_logica_actual=nullptr;
					capa.eliminar_objeto(coords.x, coords.y);
				}
			}
		}
	}
}

void Controlador_rejilla::mover_camara(const Info_input& ii)
{
	const auto& r=rejillas[rejilla_actual];

	int mult_x=ii.shift ? r.acc_w_celda() : 1, mult_y=ii.shift ? r.acc_h_celda() : 1;
	camara.movimiento_relativo(ii.x * mult_x , ii.y * mult_y);
	reconstruir_rep_info_posicion(ii, r);
}

void Controlador_rejilla::procesar_input_rejilla(Info_input& ii, Rejilla& rejilla)
{
	if(ii.x || ii.y)
	{
		mover_camara(ii);
	}

	if(ii.click_i || ii.click_d)
	{
		if(rep_listado_tiles.es_visible() && es_click_en_selector(ii.raton_x, ii.raton_y))
		{
			//Convertir a "coordenadas" de listado...
			const size_t rx=ii.raton_x - rep_listado_tiles.acc_posicion().x,
				ry=ii.raton_y,
				w=listado_tiles.acc_w_item(),
				h=listado_tiles.acc_h_item();

			//El listado no tiene conciencia de si mismo en el espacio, luego...
			const auto& pag=listado_tiles.obtener_pagina();
			for(const auto& itemp : pag)
			{

				if(	rx >= itemp.x
					&& ry >= itemp.y
					&& rx <= itemp.x + w
					&& ry <= itemp.y + h)
				{
					listado_tiles.mut_indice(itemp.indice);
					rejilla.mut_indice_actual(itemp.item.tile.acc_tipo());
					break;
				}
			}
		}
		else
		{		
			camara.transformar_posicion_raton(ii.raton_x, ii.raton_y);
			auto coords=coordenadas_rejilla(ii.raton_x, ii.raton_y, rejilla.acc_w_celda(), rejilla.acc_h_celda());

			if(ii.click_i || ii.click_d)
			{
				int btn=ii.click_i ? 0 : 1;

				//Seleccionar el tipo de tile.
				if(ii.espacio)
				{
					if(rejilla.r.comprobar(coords.x, coords.y))
					{
						rejilla.mut_indice_actual(rejilla.r(coords.x, coords.y).acc_indice_tile());
					}
				}
				//Trazar líneas o rectángulos.
				else if(ii.shift || ii.control)
				{
					auto c_ini=Coordenadas(ultimo_click.x, ultimo_click.y);
					auto trazar_horizontal=[this, &rejilla](int a, int b, int y, int btn)
					{
						int ini=a > b ? b : a;
						int fin=a > b ? a : b;
						while(ini<=fin) click_celda(ini++, y, btn, rejilla);
					};

					auto trazar_vertical=[this, &rejilla](int a, int b, int x, int btn)
					{
						int ini=a > b ? b : a;
						int fin=a > b ? a : b;
						while(ini<=fin) click_celda(x, ini++, btn, rejilla);
					};

					if(ii.shift)
					{					
						if(c_ini.x == coords.x) trazar_vertical(c_ini.y, coords.y, c_ini.x, btn);
						else if(c_ini.y == coords.y) trazar_horizontal(c_ini.x, coords.x, c_ini.y, btn);
					}
					else if(ii.control)
					{
						trazar_horizontal(c_ini.x, coords.x, c_ini.y, btn);
						if(c_ini.y != coords.y) 
						{
							int dir=c_ini.y > coords.y ? -1 : 1;
							while(true)
							{
								c_ini.y+=dir;
								trazar_horizontal(c_ini.x, coords.x, c_ini.y, btn);
								if(c_ini.y == coords.y) break;
							}
						}
					}
				}
				//Simplemente dibujar una celda.
				else
				{
					click_celda(coords.x, coords.y, btn, rejilla);
				}

				ultimo_click.asignar(coords.x, coords.y);
			}
		}	
	}	

	if(ii.x || ii.y || ii.movimiento_raton || ii.click_i)
	{
		reconstruir_rep_info_posicion(ii, rejilla);
	}
}

bool Controlador_rejilla::es_click_en_selector(int x, int y)
{
	return x >= rep_listado_tiles.acc_posicion().x;
}

void Controlador_rejilla::redimensionar_rejilla(int w, int h, Rejilla& r)
{
	auto nr=r.copiar_redimensionada(w, h);
	rejillas[rejilla_actual]=nr;
	reconstruir_rep_info_con_rejilla(r);
}

void Controlador_rejilla::click_celda(int x, int y, int btn, Rejilla& rejilla)
{
	switch(btn)
	{
		case 0: insertar_celda(x, y, rejilla); break;
		case 1: eliminar_celda(x, y, rejilla); break;
	}	
}

void Controlador_rejilla::insertar_celda(int x, int y, Rejilla& rejilla)
{
	try
	{
		if(!rejilla.r.comprobar(x, y))
		{	
			rejilla.r(x, y, Celda(x, y, rejilla.acc_indice_actual()));
		}
		else
		{
			auto& c=rejilla.r(x, y);
			c.mut_indice_tile(rejilla.acc_indice_actual());
		}
	}
	//No hay nada que hacer.
	catch(Matriz_2d_excepcion_fuera_limites &e) 
	{
		LOG<<"WARNING: Detectado acceso insertar_celda fuera de límites "<<e.x<<","<<e.y<<std::endl;
	}
	catch(Matriz_2d_excepcion_item_existe &e)
	{
		LOG<<"WARNING: Detectado acceso insertar_celda cuando existe item "<<e.x<<","<<e.y<<std::endl;
	}
	catch(Matriz_2d_excepcion_item_invalido &e) 
	{
		LOG<<"WARNING: Detectado acceso insertar_celda con item inválido "<<e.x<<","<<e.y<<std::endl;
	}
}

void Controlador_rejilla::eliminar_celda(int x, int y, Rejilla& rejilla)
{
	try
	{
		if(rejilla.r.comprobar(x, y)) rejilla.r.erase(x, y);
	}
	catch(Matriz_2d_excepcion_fuera_limites &e) {}
}

Controlador_rejilla::Coordenadas Controlador_rejilla::coordenadas_mundo(int x, int y) const
{
	auto pos=camara.acc_caja_pos();
	return Coordenadas(x+pos.x, y+pos.y);
}

Controlador_rejilla::Coordenadas Controlador_rejilla::coordenadas_rejilla(int x, int y, int w, int h) const
{
	auto c=coordenadas_mundo(x, y);
	return Coordenadas(floor(c.x / w), floor(c.y / h));
}

Controlador_rejilla::Coordenadas Controlador_rejilla::coordenadas_mundo_desde_rejilla(int x, int y, int w, int h) const
{
	return Coordenadas(x * w, y * h);
}

void Controlador_rejilla::reconstruir_rep_info_posicion(const Info_input& ii, const Rejilla& r)
{
	auto rm=coordenadas_mundo(ii.raton_x, ii.raton_y);
	auto rr=coordenadas_rejilla(ii.raton_x, ii.raton_y, r.acc_w_celda(), r.acc_h_celda());

	std::stringstream ss;
	auto foco=camara.acc_caja_foco();
	ss<<"CAM["<<foco.x<<","<<foco.y<<"] POS["<<rm.x<<","<<rm.y<<"] REJ["<<rr.x<<","<<rr.y<<"]";
	rep_info_pos.asignar(ss.str());
}

void Controlador_rejilla::reconstruir_rep_info_con_rejilla(const Rejilla& r)
{
	std::stringstream ss;
	ss<<"DIM:["<<r.acc_w()<<", "<<r.acc_h()<<"] SEP: ["<<r.acc_w_unidades_separador()<<", "<<r.acc_h_unidades_separador()<<"]\n";
	rep_info_capa.asignar(ss.str());
}

void Controlador_rejilla::reconstruir_rep_info_con_objeto_logica(const Objeto_logica& obj)
{
	const auto& capa=capas_logica[capa_logica_actual];
	int t=obj.acc_tipo();
	auto cb=[t](const Logica& l) {return l.acc_tipo()==t;};
	auto tipo=capa.acc_gestor().buscar_unico_callback(cb);

	std::string info_obj=obj.como_cadena();
	if(tipo!=nullptr)
	{
		info_obj=tipo->acc_nombre()+"\n"+info_obj;
		const auto& propiedades=tipo->acc_propiedades();
		for(const auto&p : propiedades) info_obj+="\n - "+p.nombre+"="+obj.valor_propiedad(p.id);
	}

	rep_info_capa.asignar(info_obj);
}

void Controlador_rejilla::seleccionar_rejilla_actual(size_t indice)
{
	if(seleccionar_parte(rejillas, indice, rejilla_actual))
	{
		const auto& g=rejillas[rejilla_actual].acc_gestor();
		preparar_listado_tiles(g);

		auto& r=rejillas[rejilla_actual];
		//Ajusta los nuevos límites de la cámara.
		camara.establecer_limites(0, 0, r.acc_w() * r.acc_w_celda(), r.acc_h()*r.acc_h_celda());
		reconstruir_rep_info_con_rejilla(r);
	}
}

void Controlador_rejilla::seleccionar_capa_logica_actual(size_t indice)
{
	if(seleccionar_parte(capas_logica, indice, capa_logica_actual))
	{
		preparar_listado_logica(capas_logica[capa_logica_actual].acc_gestor());
	}
}

void Controlador_rejilla::ciclar_zoom()
{
	++info_zoom.zoom;
	if(info_zoom.zoom==4) info_zoom.zoom=1;

	camara.mut_enfoque(info_zoom.w * info_zoom.zoom, info_zoom.h * info_zoom.zoom);

//	auto& foco=camara.acc_caja_foco();
//	camara.enfocar_a(0,0);

	//TODO: Cambiar el zoom en un momento distinto de enfocar a 0,0 tiene consecuencias chungas...
	//camara.enfocar_a(0,0);
}

void Controlador_rejilla::insertar_rejilla()
{
	try
	{
		insertar_parte(rejillas, rejilla_actual, &Controlador_rejilla::seleccionar_rejilla_actual);
	}
	catch(Contenedor_tilesets_exception& e)
	{
		LOG<<e.what()<<"\n";
	}
}

void Controlador_rejilla::insertar_capa_logica()
{
	insertar_parte(capas_logica, capa_logica_actual, &Controlador_rejilla::seleccionar_capa_logica_actual);
}

void Controlador_rejilla::guardar()
{
	try
	{
		LOG<<"Iniciando exportación de "<<nombre_fichero<<"\n";
		Exportador E;
		E.exportar(rejillas, capas_logica, propiedades_meta, tilesets, sets_tipo_logica, nombre_fichero);
	}
	catch(Exportador_exception& e)
	{
		LOG<<"Ha ocurrido un error en el proceso de exportación : "<<e.what()<<"\n";
	}
}

void Controlador_rejilla::cargar()
{
	try
	{
		LOG<<"Iniciando importación de "<<nombre_fichero<<"\n";
		Importador I;
		I.importar(rejillas, capas_logica, propiedades_meta, tilesets, sets_tipo_logica, nombre_fichero);
		inicializar();
	}
	catch(Exportador_exception& e)
	{
		LOG<<"Ha ocurrido un error en el proceso de importación : "<<e.what()<<"\n";
	}
	
}

void Controlador_rejilla::deseleccionar_objeto_logica_actual()
{
	objeto_logica_actual=nullptr;
	rep_info_capa.asignar("");
}

void Controlador_rejilla::pasar_pagina_selector(int v)
{
	//TODO: Actualizar el item actual!!!

	switch(modo_actual)
	{
		case modo_operacion::REJILLA:
			if(listado_tiles.cambiar_pagina(v))
			{
				preparar_listado_tiles(rejillas[rejilla_actual].acc_gestor());
			}
		break;
		case modo_operacion::CAPA_LOGICA:
			if(listado_logica.cambiar_pagina(v))
			{
				preparar_listado_logica(capas_logica[capa_logica_actual].acc_gestor());
			}
		break;
	}
}

void Controlador_rejilla::cambiar_tile_selector(int v)
{
	//TODO: Actualizar el item actual!!!

	switch(modo_actual)
	{
		case modo_operacion::REJILLA: 
			if(listado_tiles.cambiar_item(v))
				preparar_listado_tiles(rejillas[rejilla_actual].acc_gestor());
		break;
		case modo_operacion::CAPA_LOGICA: 
			if(listado_logica.cambiar_item(v))
				preparar_listado_logica(capas_logica[capa_logica_actual].acc_gestor());
		break;
	}
}

const Logica& Controlador_rejilla::obtener_tipo_objeto_logica_actual()
{
	int t=objeto_logica_actual->acc_tipo();
	auto cb=[t](const Logica& l) {return l.acc_tipo()==t;};
	return *capas_logica[capa_logica_actual].acc_gestor().buscar_unico_callback(cb);
}

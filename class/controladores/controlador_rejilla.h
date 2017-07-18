#ifndef CONTROLADOR_REJILLA
#define CONTROLADOR_REJILLA

#include <templates/listado_rejilla.h>
#include <templates/listado_vertical.h>
#include "controlador_base.h"
#include "../app/propiedad_meta.h"
#include "../app/tile.h"
#include "../app/rejilla.h"
#include "../app/objeto_logica.h"
#include "../app/capa_logica.h"
#include "../app/contenedor_tilesets.h"
#include "../app/contenedor_logica_sets.h"
#include "../app/exportador.h"
#include "../app/importador.h"

class Controlador_rejilla:public Controlador_base
{
	//////////////
	//Propiedades
	private:

	static const size_t MAX_REJILLAS=6; //Completamente arbitrario.
	static const size_t ALTURA_LISTADO_VERTICAL=10;
	static const size_t DIM_LISTADO_REJILLA=32;
	static const size_t W_LISTADOS=200;
	
	enum class modo_operacion {REJILLA, CAPA_LOGICA};

	struct Info_input
	{
		int x, y, raton_x, raton_y;		
		bool click_i, click_d, movimiento_raton, shift, control, espacio;
		int down_x, down_y;
	
		Info_input(): x(0), y(0), raton_x(0), raton_y(0), click_i(false), 
			click_d(false), movimiento_raton(false), shift(false), 
			control(false), espacio(false), down_x(0), down_y(0) {}
		Info_input(int px, int py, int rx, int ry, bool ci, bool cd, bool mr, bool s, bool c, bool e, int dx, int dy)
			: x(px), y(py), raton_x(rx), raton_y(ry), click_i(ci), 
			click_d(cd), movimiento_raton(mr), shift(s), control(c), 
			espacio(e), down_x(dx), down_y(dy) {}
	};

	struct Coordenadas
	{
		int x, y;
		Coordenadas(): x(0), y(0) {}
		Coordenadas(int px, int py): x(px), y(py) {}
	};

	struct Ultimo_click	//X e Y en "coordenadas rejilla".
	{
		int x, y;
		Ultimo_click(): x(0), y(0) {}
		void asignar(int px, int py) {x=px; y=py;}

	}ultimo_click;

	struct Info_zoom
	{
		unsigned int zoom;
		unsigned int w;
		unsigned int h;

		Info_zoom(unsigned int z, unsigned int pw, unsigned int ph):
			zoom(z), w(pw), h(ph)
		{}
	}info_zoom;

	struct Item_tile
	{
		const Tile& tile;
	};

	struct Item_logica
	{
		const Logica& logica;
	};

	const std::string& nombre_fichero;
	const Contenedor_tilesets& tilesets;
	const Contenedor_logica_sets& sets_tipo_logica;
	std::vector<Rejilla> rejillas;
	std::vector<Capa_logica> capas_logica;
	std::vector<Propiedad_meta> propiedades_meta;
	DLibV::Camara camara;
	DLibV::Representacion_bitmap_dinamica rep_tiles;
	DLibV::Representacion_texto_auto_estatica rep_info_capa;
	DLibV::Representacion_texto_auto_estatica rep_info_pos;

	size_t rejilla_actual;
	size_t capa_logica_actual;
	modo_operacion modo_actual;
	Objeto_logica * objeto_logica_actual;

	Herramientas_proyecto::Listado_rejilla<Item_tile>	listado_tiles;
	Herramientas_proyecto::Listado_vertical<Item_logica>	listado_logica;

	DLibV::Representacion_agrupada_estatica 		rep_listado_tiles;
	DLibV::Representacion_agrupada_estatica 		rep_listado_logica;

	DLibV::Representacion_primitiva_caja_estatica 		rep_seleccion_actual;

	///////////////
	//Métodos internos.

	void ciclar_zoom();
	void seleccionar_rejilla_actual(size_t indice);
	void seleccionar_capa_logica_actual(size_t indice);
	void intercambiar_visibilidad_rejilla(size_t indice);
	void redimensionar_rejilla(int w, int h, Rejilla& rejilla);
	void mover_rejilla(int w, int h, Rejilla& rejilla);
	void click_celda(int x, int y, int btn, Rejilla& rejilla);
	void insertar_celda(int x, int y, Rejilla& rejilla);
	void eliminar_celda(int x, int y, Rejilla& rejilla);
	void insertar_rejilla();
	void insertar_capa_logica();

	void mover_camara(const Info_input& ii);
	void deseleccionar_objeto_logica_actual();

	void guardar();
	void inicializar();

	void dibujar_rejilla(DLibV::Pantalla& p, const int w, const int h, const int w_nivel, const int h_nivel, const int w_unidades_separador, const int h_unidades_separador);
	void dibujar_celdas(DLibV::Pantalla& p, Rejilla& rejilla);
	void dibujar_capa_logica(DLibV::Pantalla& p, Capa_logica& capa);
	void dibujar_hud_rejillas(DLibV::Pantalla& p);
	void dibujar_elemento_desconocido(DLibV::Pantalla& p, int x, int y);

	void preparar_listado_tiles(const Tile_set& s);
	void preparar_listado_logica(const Logica_set& s);
	void actualizar_seleccion_actual_listado();

	void pasar_pagina_selector(int v);
	void cambiar_tile_selector(int v);
	bool es_click_en_selector(int x, int y);

	template<typename T> 
	void swap_parte(std::vector<T>& v, size_t &indice)
	{
		if(v.size() > 1 && indice > 0)
		{
			std::swap(v[indice], v[indice-1]);
			--indice;	
		}
	}

	template <typename T>
	bool seleccionar_parte(std::vector<T>& v, size_t indice, size_t& actual)
	{
		if(indice < v.size())
		{	
			actual=indice;
			return true;
		}
		return false;
	}

	template <typename T> 
	void insertar_parte(std::vector<T>& v, size_t indice_actual, void(Controlador_rejilla::*metodo)(size_t))
	{
		if(v.size() < MAX_REJILLAS)
		{
			T& actual=v[indice_actual];
			v.insert(std::begin(v)+indice_actual+1,	T::copiar_sin_contenido(actual));
			(this->*metodo)(indice_actual+1);
		}
	}

	template <typename T>
	void eliminar_parte(std::vector<T>& v, size_t indice_actual, void(Controlador_rejilla::*metodo)(size_t))
	{
		if(v.size() > 1) 
		{
			v.erase(std::begin(v)+indice_actual);
			if(indice_actual==0) (this->*metodo)(0);
			else (this->*metodo)(indice_actual-1);
		}
	}
	
	template <typename T, typename C>
	void ciclar_set_listado(T& parte, const C& set)
	{
		auto& gestor=set.obtener_siguiente(parte.acc_gestor());
		parte.cambiar_gestor(gestor);
	}

	template <typename T>
	void intercambiar_visibilidad_parte(std::vector<T>& v, size_t indice) 
	{
		if(indice < v.size()) v[indice].intercambiar_visible_siempre();
	}

	void procesar_input_rejilla(Info_input& ii, Rejilla& rejilla);
	void procesar_input_capa_logica(Info_input& ii, Capa_logica& capa);
	Info_input recoger_input(const Input_base& input);

	void reconstruir_rep_info_posicion(const Info_input& ii, const Rejilla& r);
	void reconstruir_rep_info_con_rejilla(const Rejilla& rejilla);
	Coordenadas coordenadas_rejilla(int x, int y, int w, int h) const;
	Coordenadas coordenadas_mundo(int x, int y) const;
	Coordenadas coordenadas_mundo_desde_rejilla(int x, int y, int w, int h) const;

	///////////////
	//Interface pública.
	public:
	
	Controlador_rejilla(Director_estados &DI, DLibV::Pantalla&, const std::string& nombre_fichero, const Contenedor_tilesets& t, const Contenedor_logica_sets& S);

	virtual void preloop(Input_base& input, float delta) {/*input.iniciar_input_texto()*/;}
	virtual void postloop(Input_base& input, float delta) {/*input.finalizar_input_texto()*/;}
	virtual void loop(Input_base& input, float delta);
	virtual void dibujar(DLibV::Pantalla& pantalla);
	void cargar();
	void inicializar_sin_fichero();

	std::vector<Propiedad_meta>& acc_propiedades_meta() {return propiedades_meta;}
	Objeto_logica& obtener_objeto_logica_actual() {return *objeto_logica_actual;}
	const Logica& obtener_tipo_objeto_logica_actual();
	void reconstruir_rep_info_con_objeto_logica(const Objeto_logica& obj); //Es public... Se llama desde el bootstrap en algún momento.
};

#endif

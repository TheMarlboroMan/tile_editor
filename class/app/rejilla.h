#ifndef REJILLA_H
#define REJILLA_H

#include "tile.h"
#include <templates/matriz2d.h>

class Celda
{
	private:

	int x, y;
	size_t indice_tile;

	public:

	Celda():x(0), y(0), indice_tile(0) {}
	Celda(int px, int py, size_t it): x(px), y(py), indice_tile(it) {}

	int acc_x() const {return x;}
	int acc_y() const {return y;}
	size_t acc_indice_tile() const {return indice_tile;}
	
	void mut_indice_tile(size_t v) {indice_tile=v;}
};

class Rejilla
{
	private:

	//Ancho y alto de la rejilla, en celdas.
	int w, h;
	//Dimensiones de la celda.
	int w_celda, h_celda;
	//Cada x celdas hay una línea de un color diferente en la rejilla.
	int w_unidades_separador, h_unidades_separador;

	//Indice de tile seleccionado actualmente.
	int indice_actual;

	//Lo ponemos como puntero porque es posible cambiarlo en marcha.
	const Tile_set * gestor_tiles;

	bool visible_siempre;

	public:

	Herramientas_proyecto::Matriz_2d<Celda> r;	//La rejilla es PUBLICA. Si, pública... No tiene sentido hacer de este un objeto proxy para eso.

	int acc_w() const {return w;}
	int acc_h() const {return h;}
	int acc_w_celda() const {return w_celda;}
	int acc_h_celda() const {return h_celda;}
	int acc_w_unidades_separador() const {return w_unidades_separador;}
	int acc_h_unidades_separador() const {return h_unidades_separador;}

	Rejilla(int pw, int ph, int pwc, int phc, int pws, int phs, const Tile_set& gt): 
		w(pw), h(ph), w_celda(pwc), h_celda(phc), w_unidades_separador(pws), h_unidades_separador(phs), indice_actual(1), gestor_tiles(&gt), visible_siempre(false), r(w, h)
	{}

	void cambiar_gestor(const Tile_set& gt) {gestor_tiles=&gt; indice_actual=1;}
	const Tile_set& acc_gestor() const {return *gestor_tiles;}
	size_t acc_indice_recurso() const {return gestor_tiles->acc_indice_recurso();}
	int acc_indice_actual() const {return indice_actual;}
	void mut_indice_actual(int v) {indice_actual=v;}
	void intercambiar_visible_siempre() {visible_siempre=!visible_siempre;}
	bool es_visible_siempre() const {return visible_siempre;}
	
	void reajustar_espaciado(int px, int py)
	{
		w_unidades_separador+=px;
		h_unidades_separador+=py;

		if(w_unidades_separador <= 0) w_unidades_separador=1;
		if(h_unidades_separador <= 0) h_unidades_separador=1;
	}

	static Rejilla copiar_sin_contenido(const Rejilla& r)
	{
		return Rejilla(
			r.w, r.h, 
			r.w_celda, r.h_celda, 
			r.w_unidades_separador, r.h_unidades_separador, 
			*r.gestor_tiles );
	}

	Rejilla copiar_redimensionada(int pw, int ph)
	{
		Rejilla resultado(pw, ph, w_celda, h_celda, w_unidades_separador, h_unidades_separador, *gestor_tiles);
		resultado.r=r.copiar_con_dimensiones(pw, ph);
		return resultado;
	}

	Rejilla copiar_mover(int px, int py)
	{
		Rejilla resultado(w, h, w_celda, h_celda, w_unidades_separador, h_unidades_separador, *gestor_tiles);
		auto& m=r.acc_matriz();
		for(auto& par : m)
		{
			auto& c=par.second;
			int x=c.acc_x()+px, y=c.acc_y()+py;

			if(x >= 0 && y >= 0 && x < w && y < h)
				resultado.r(x, y, Celda(x, y, c.acc_indice_tile()));
		}
		return resultado;
	}
};

#endif

#ifndef SELECTOR_TILES_H
#define SELECTOR_TILES_H

#include "tile.h"
#include "selector_base.h"

/*El selector guarda el índice de la tile actual y se encarga de ordenar las
tiles existentes para mostrarlas en pantalla ordenadas por página. */

class Selector_tiles:public Selector_base
{
	private:

	struct Info_presentacion
	{
		int x, y, w, h;
		const Tile& tile;

		Info_presentacion(int px, int py, int pw, int ph, const Tile& pt):
			x(px), y(py), w(pw), h(ph), tile(pt)
		{}
	};

	std::vector<std::vector<Info_presentacion>> paginas;

	size_t indice_recurso;

	DLibV::Representacion_primitiva_caja_estatica rep_selector;
	DLibV::Representacion_bitmap_estatica rep_bmp;

	protected:

	virtual void dibujar(DLibV::Pantalla& p, int indice_actual);
	virtual size_t total_paginas() const {return paginas.size();}

	public:

	Selector_tiles(DLibV::Pantalla& p)
		:Selector_base(p),
		rep_selector(Herramientas_SDL::nuevo_sdl_rect(0, 0, 0, 0), 255, 255, 255),
		rep_bmp()
	{}

	void preparar_paginas(const Tile_set& gestor_tiles);
	void pasar_pagina(int v);
	virtual int procesar_click(int x, int y);
};

#endif

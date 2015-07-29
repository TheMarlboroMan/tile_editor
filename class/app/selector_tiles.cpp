#include "selector_tiles.h"

#include "recursos.h"
#include <iostream>
#include <algorithm>

using namespace DLibV;
using namespace DLibH;

void Selector_tiles::preparar_paginas(const Tile_set& gestor_tiles)
{
	paginas.clear();
	establecer_pagina_actual(0);

	indice_recurso=gestor_tiles.acc_indice_recurso();
	rep_bmp.establecer_textura(Gestor_texturas::obtener(indice_recurso));
	rep_bmp.establecer_modo_blend(Representacion::BLEND_ALPHA);

	const int ini_x=0, ini_y=0, margen_y=4, margen_x=4, max_x=acc_w(), max_y=acc_h();
	int x=ini_x, y=ini_y, w=0, h=0;
	int max_h=0;

	auto& tiles=gestor_tiles.acc_items();	

	paginas.push_back(std::vector<Info_presentacion>{});

	for(auto& t : tiles)
	{
		//Medir el frame. Buscar la altura mayor para poder hacer el salto Y cuando sea preciso.
		/*x=f.acc_x(), y=f.acc_y(), */
		w=t.acc_w();
		h=t.acc_h();
		if(h > max_h) max_h=h;

		//Con respecto al ancho, si no cabe en la línea, saltar a la siguiente línea de y
		//excepto que sea lo único que hay en la línea, en ese caso lo ponemos tal cual.

		if(x+w > max_x)	
		{
			if(x!=ini_x)
			{
				x=ini_x;
				y+=max_h+margen_y;

				max_h=0;

				//TODO: Esto no está bien del todo.
				if(y+h > max_y)
				{
					paginas.push_back(std::vector<Info_presentacion>{});
					y=ini_y;	
				}
			}
		}

		//Rellenar info presentación...
		paginas.back().push_back(Info_presentacion(x, y, w, h, t));
		x+=w+margen_x;
	}
}

void Selector_tiles::dibujar(DLibV::Pantalla& p, int indice_actual)
{
	const int pos_x=acc_x()+2, pos_y=2;

	for(auto& i:paginas[acc_pagina_actual()])
	{
		if((int)i.tile.acc_tipo()==indice_actual)
		{
			rep_selector.establecer_posicion(i.x+pos_x-3, pos_y+i.y-3, i.w+6, i.h+6);
			rep_selector.volcar(p);
		}
		//rep_bmp.establecer_alpha((int)i.tile.acc_tipo()==indice_actual ? 255 : 128);

		rep_bmp.establecer_posicion(i.x+pos_x, pos_y+i.y, i.w, i.h);
		rep_bmp.establecer_recorte(i.tile.acc_x(), i.tile.acc_y(), i.tile.acc_w(), i.tile.acc_h());
		rep_bmp.volcar(p);
	}
}

/*Devuelve 0 si clickamos en nada o el tipo del Tile, si picamos en un tile.*/

int Selector_tiles::procesar_click(int cx, int cy)
{	
	cx-=acc_x();

	const auto& pag=paginas[acc_pagina_actual()];

	auto it=std::find_if(std::begin(pag), std::end(pag),
			[cx, cy](const Info_presentacion& i)
			{
				return cx >= i.x && cy >= i.y && cx < i.x+(int)i.tile.acc_w() && cy < i.y+(int)i.tile.acc_h();	
			});

	if(it!=pag.end()) return (*it).tile.acc_tipo();
	else return 0;
}

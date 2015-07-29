#include "selector_tipo_logica.h"
#include "recursos.h"
#include <video/gestores/gestor_superficies.h>

using namespace DLibV;
using namespace DLibH;

void Selector_tipo_logica::preparar_paginas(const Logica_set& g)
{
	//Vamos a ir rellenando el vector de strings según veamos que caben
	//en la resolución de pantalla escogida.

	paginas.clear();
	establecer_pagina_actual(0);

	const auto& vt=g.acc_items();
	
	int y=0, max_y=acc_h()-ALTURA_LINEA;

	//Inserción de la primera página..
	paginas.push_back(std::vector<Logica>() );

	for(const auto& t: vt)
	{
		//Evaluar si debemos insertar una nueva página...
		if(y >= max_y) 
		{
			y=0;
			paginas.push_back(std::vector<Logica>() );
		}

		paginas.back().push_back(t);
		y+=ALTURA_LINEA;
	}
}

void Selector_tipo_logica::dibujar(Pantalla& p, int indice_actual)
{
	const int x=acc_x(), pos_x=x+5, w=acc_w();
	int pos_y=2;
	Representacion_texto_auto_dinamica rep_txt(p.acc_renderer(), Gestor_superficies::obtener(Recursos_graficos::RS_FUENTE_BASE), "...");
	DLibV::Representacion_primitiva_caja_dinamica rep_color(Herramientas_SDL::nuevo_sdl_rect(x+2, pos_y, 6, 6), 0, 0, 0);

	for(auto& t : paginas[acc_pagina_actual()])
	{
		if(t.acc_tipo()==indice_actual)
		{
			DLibV::Representacion_primitiva_caja_dinamica rep_seleccion(Herramientas_SDL::nuevo_sdl_rect(x, pos_y, w, ALTURA_LINEA), 128, 128, 128);
			rep_seleccion.volcar(p);
		}

		rep_txt.asignar(t.acc_nombre());
		rep_txt.establecer_posicion(pos_x+9, pos_y);

		rep_color.establecer_posicion(pos_x, pos_y);
		rep_color.mut_rgb(t.acc_r_editor(), t.acc_g_editor(), t.acc_b_editor());

		rep_color	.volcar(p);
		rep_txt.volcar(p);
		pos_y+=ALTURA_LINEA;
	}
}

int Selector_tipo_logica::procesar_click(int x, int y)
{
	unsigned int indice=floor(y / ALTURA_LINEA);
	const auto& pag=paginas[acc_pagina_actual()];

	if(indice >= pag.size()) return 0;
	else return pag[indice].acc_tipo();
}

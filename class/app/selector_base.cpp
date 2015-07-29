#include "selector_base.h"

void Selector_base::volcar(DLibV::Pantalla& p, int indice_actual)
{
	if(!visible) return;
	rep_caja.volcar(p);
	dibujar(p, indice_actual);
}

void Selector_base::pasar_pagina(int v)
{
	if(pagina_actual==0 && v < 0) 
	{
		return;
	}
	else 
	{
		size_t total=total_paginas();
		pagina_actual+=v;
		if(pagina_actual >= total) pagina_actual=total-1;
	}
}


#ifndef SELECTOR_BASE_H
#define SELECTOR_BASE_H

#include <libDan2.h>

class Selector_base
{
	private:

	bool visible;
	int w, h, x;
	size_t pagina_actual;
	DLibV::Representacion_primitiva_caja_estatica rep_caja; //La caja que se dibujará...

	protected:

	virtual void dibujar(DLibV::Pantalla& p, int indice_actual)=0;
	virtual size_t total_paginas() const=0;

	public:

	Selector_base(DLibV::Pantalla& p)
		:
		visible(true), w(p.acc_w() / 4), h(p.acc_h()), x(p.acc_w()-w), 
		pagina_actual(0),
		rep_caja(Herramientas_SDL::nuevo_sdl_rect(x+2, 0, w, h), 0, 0, 0)
	{
		rep_caja.establecer_alpha(192);
	}

	void volcar(DLibV::Pantalla& p, int indice_actual);
	bool recibe_click(int cx, int cy) {return visible && cx >= x;}
	void intercambiar_visible() {visible=!visible;}
	int acc_w() const {return w;}
	int acc_h() const {return h;}
	int acc_x() const {return x;}
	size_t acc_pagina_actual() const {return pagina_actual;}
	void pasar_pagina(int v);
	void establecer_pagina_actual(size_t v) {pagina_actual=v;}

	virtual int procesar_click(int x, int y)=0;

};

#endif

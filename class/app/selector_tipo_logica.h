#ifndef SELECTOR_TIPO_LOGICA_H
#define SELECTOR_TIPO_LOGICA_H

#include "selector_base.h"
#include "logica.h"

class Selector_tipo_logica:public Selector_base
{
	private:

	static const int ALTURA_LINEA=16;
	std::vector<std::vector<Logica>> paginas;

	protected:

	virtual void dibujar(DLibV::Pantalla&p, int indice_actual);
	virtual size_t total_paginas() const {return paginas.size();}

	public:

	void preparar_paginas(const Logica_set& g);
	virtual int procesar_click(int x, int y);

	Selector_tipo_logica(DLibV::Pantalla& p)
		:Selector_base(p)
	{}
	
};

#endif

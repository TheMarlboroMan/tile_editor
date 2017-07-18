#ifndef PRIMITIVA_GRAFICA_CAJA_H
#define PRIMITIVA_GRAFICA_CAJA_H

/*
Simplemente hace un fillrect. Simple, rápido y efectivo.
*/

#include "../representacion_primitiva.h"

namespace DLibV
{
class Representacion_primitiva_caja_base
	:public Representacion_primitiva
{
	public:
	
				Representacion_primitiva_caja_base(const SDL_Rect&, Uint8, Uint8, Uint8);
				Representacion_primitiva_caja_base(const Representacion_primitiva_caja_base&);
				Representacion_primitiva_caja_base& operator=(const Representacion_primitiva_caja_base&);
	virtual 		~Representacion_primitiva_caja_base();

	virtual bool 		es_rellena() const=0;

	bool 			volcado(SDL_Renderer *);
	bool 			volcado(SDL_Renderer *, const SDL_Rect&, const SDL_Rect&, double); 

	protected:

	void 			preparar_posicion();
};

class Representacion_primitiva_caja
	:public Representacion_primitiva_caja_base
{
	public:
	
				Representacion_primitiva_caja(const SDL_Rect& p_pos, Uint8 pr, Uint8 pg, Uint8 pb);
				Representacion_primitiva_caja(const Representacion_primitiva_caja& p_otra);
				Representacion_primitiva_caja& operator=(const Representacion_primitiva_caja& p_otro);
	virtual 		~Representacion_primitiva_caja() {}

	bool 			es_rellena() const {return true;}
};

class Representacion_primitiva_caja_lineas
	:public Representacion_primitiva_caja_base
{
	public:

				Representacion_primitiva_caja_lineas(const SDL_Rect& p_pos, Uint8 pr, Uint8 pg, Uint8 pb);
				Representacion_primitiva_caja_lineas(const Representacion_primitiva_caja_lineas& p_otra);
				Representacion_primitiva_caja_lineas& operator=(const Representacion_primitiva_caja_lineas& p_otro);
	virtual 		~Representacion_primitiva_caja_lineas() {}

	bool 			es_rellena() const {return false;}
};


}

#endif

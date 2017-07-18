#ifndef PRIMITIVA_GRAFICA_LINEA_H
#define PRIMITIVA_GRAFICA_LINEA_H

#include "../representacion_primitiva.h"

namespace DLibV
{
class Representacion_primitiva_linea:public Representacion_primitiva
{
	public:
	
					Representacion_primitiva_linea(int x1, int y1, int x2, int y2, Uint8, Uint8, Uint8);
					Representacion_primitiva_linea(const Representacion_primitiva_linea&);
	virtual 			~Representacion_primitiva_linea();
					Representacion_primitiva_linea& operator=(const Representacion_primitiva_linea&);

	bool 				volcado(SDL_Renderer *);
	bool 				volcado(SDL_Renderer *, const SDL_Rect&, const SDL_Rect&, double); 


	void 				establecer_puntos(int x1, int y1, int x2, int y2);

	protected:

	void				generar_posicion();
	void 				preparar_posicion();

	private:

	int 				x1, y1, x2, y2;
};

}

#endif

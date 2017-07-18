#ifndef PRIMITIVA_GRAFICA_PUNTOS_H
#define PRIMITIVA_GRAFICA_PUNTOS_H

#include "../representacion_primitiva.h"
#include <vector>

namespace DLibV
{
class Representacion_primitiva_puntos:
	public Representacion_primitiva
{
	public:
	
						Representacion_primitiva_puntos(Uint8, Uint8, Uint8);
						Representacion_primitiva_puntos(int, int, Uint8, Uint8, Uint8);
						Representacion_primitiva_puntos(const Representacion_primitiva_puntos&);
	virtual 				~Representacion_primitiva_puntos();
						Representacion_primitiva_puntos& operator=(const Representacion_primitiva_puntos&);

	bool 					volcado(SDL_Renderer *);
	bool 					volcado(SDL_Renderer *, const SDL_Rect&, const SDL_Rect&, double); 

	void 					insertar(int x, int y);
	void					limpiar_puntos();

	protected:

	void 					generar_posicion();
	void 					preparar_posicion();


	private:

	std::vector<SDL_Point>			puntos;
};

}

#endif

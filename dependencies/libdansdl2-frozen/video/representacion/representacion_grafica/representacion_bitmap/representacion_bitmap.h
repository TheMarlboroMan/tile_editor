#ifndef REPRESENTACION_BITMAP
#define REPRESENTACION_BITMAP

#include "../representacion_grafica.h"

namespace DLibV
{


class Representacion_bitmap:public Representacion_grafica
{
	public:

	Representacion_bitmap();
	Representacion_bitmap(const Representacion_bitmap&);
	Representacion_bitmap(const Textura *);
	virtual ~Representacion_bitmap();
	Representacion_bitmap& operator=(const Representacion_bitmap &);

	void establecer_recurso_sin_recortar(Textura const * r){Representacion_grafica::establecer_textura(r);}
	virtual void establecer_textura(Textura const *);
	void preparar(const SDL_Renderer * r);
};

} //Fin namespace DLibV

#endif

#ifndef REPRESENTACION_TTF_H
#define REPRESENTACION_TTF_H

#include <memory>
#include "../representacion_grafica.h"
#include "../../../fuente_ttf/fuente_ttf.h"
#include "../../../textura/textura.h"
#include "../../../superficie/superficie.h"
#include "../../../superficie/lienzo.h"
#include "../../../../herramientas/log_base/log_base.h"
#include "../../../../herramientas/herramientas/herramientas.h"

namespace DLibV
{
class Representacion_TTF:
	public Representacion_grafica
{
	public:

					Representacion_TTF(const Fuente_TTF&, SDL_Color, const std::string&);
					Representacion_TTF(const Fuente_TTF&, Uint8 r, Uint8 g, Uint8 b, Uint8 a, const std::string&);
					Representacion_TTF(const Fuente_TTF&, SDL_Color);
					Representacion_TTF(const Fuente_TTF&, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
					Representacion_TTF(const Representacion_TTF&);
	virtual				~Representacion_TTF();
	Representacion_TTF&		operator=(const Representacion_TTF&);

	const std::string& 		acc_cadena() const {return this->cadena;}
	int				acc_tamano_fuente() const {return fuente->acc_tamano_fuente();}
	const std::string&		acc_nombre_fuente() const {return fuente->acc_nombre_fuente();}

	void 				preparar(const SDL_Renderer * renderer);
	void 				modificar_fuente(const Fuente_TTF&);
	virtual void 			asignar(const char);
	virtual void 			asignar(const char *);
	virtual void 			asignar(const std::string& p_param);

	private:
	
	void				interno_asignar(const std::string&);
	void				reemplazar(std::string&, const std::string&, const std::string&);

	//Esto es un puntero a conciencia, para poderlo cambiar.
	Fuente_TTF const *		fuente;
	std::string			cadena;
	SDL_Color			color;
};

}

#endif

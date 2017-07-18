#ifndef FUENTE_TTF_H
#define FUENTE_TTF_H

#include <SDL2/SDL_ttf.h>
#include <memory>
#include <stdexcept>
#include <string>

namespace DLibV
{

class Fuente_TTF
{
	public:

					Fuente_TTF(const std::string&, int);
					Fuente_TTF(const Fuente_TTF&);
					Fuente_TTF& operator=(const Fuente_TTF&);
					~Fuente_TTF();

	TTF_Font const *		acc_fuente() const {return fuente;}
	const std::string&		acc_nombre_fuente() const {return nombre_fuente;}
	int				acc_tamano_fuente() const {return tamano_fuente;}

	private:

	void				cargar_fuente();

	std::string			nombre_fuente;
	int				tamano_fuente;
	TTF_Font *			fuente;
};

}

#endif

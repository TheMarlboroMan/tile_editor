#include "fuente_ttf.h"

using namespace DLibV;

Fuente_TTF::Fuente_TTF(const std::string& f, int t)
	:nombre_fuente(f), tamano_fuente(t),
	fuente(nullptr)
{
	cargar_fuente();
}

Fuente_TTF::Fuente_TTF(const Fuente_TTF& f)
	:nombre_fuente(f.nombre_fuente),
	tamano_fuente(f.tamano_fuente),
	fuente(nullptr)
{
	cargar_fuente();
}

Fuente_TTF& Fuente_TTF::operator=(const Fuente_TTF& f)
{
	nombre_fuente=f.nombre_fuente;
	tamano_fuente=f.tamano_fuente;
	fuente=nullptr;
	cargar_fuente();
	return *this;
}

Fuente_TTF::~Fuente_TTF()
{
	if(fuente)
	{
		TTF_CloseFont(fuente);
	}
}

void Fuente_TTF::cargar_fuente()
{
	fuente=TTF_OpenFont(nombre_fuente.c_str(), tamano_fuente);
	if(fuente==nullptr)
	{
		throw std::runtime_error("ERROR: Imposible copiar fuente "+nombre_fuente);
	}
}

#include "representacion_bitmap.h"

using namespace DLibV;
Representacion_bitmap::Representacion_bitmap():Representacion_grafica()
{
	this->preparar(nullptr);
}

Representacion_bitmap::Representacion_bitmap(const Textura * p_rec):Representacion_grafica()
{
	this->establecer_textura(p_rec);
	this->establecer_dimensiones_posicion_por_recorte();
	this->preparar(nullptr);
}

Representacion_bitmap::Representacion_bitmap(const Representacion_bitmap& o)
	:Representacion_grafica(o)
{
	this->preparar(nullptr);
}

Representacion_bitmap::~Representacion_bitmap()
{

}

Representacion_bitmap& Representacion_bitmap::operator=(const Representacion_bitmap & p_otra)
{
	Representacion_grafica::operator=(p_otra);
	this->preparar(nullptr);
	return *this;
}

void Representacion_bitmap::establecer_textura(Textura const * p_recurso)
{
	Representacion_grafica::establecer_textura(p_recurso);
	this->recorte_a_medidas_textura();
}

void Representacion_bitmap::preparar(const SDL_Renderer * r)
{
	Representacion_grafica::preparar(r);
}

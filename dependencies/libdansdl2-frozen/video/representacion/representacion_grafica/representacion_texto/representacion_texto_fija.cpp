#include "representacion_texto_fija.h"

using namespace DLibV;

Representacion_texto_fija::Representacion_texto_fija(const Representacion_texto_fija& p_otra)
	:Representacion_texto(p_otra)
{
	
}

Representacion_texto_fija& Representacion_texto_fija::operator=(const Representacion_texto_fija& p_otra)
{
	Representacion_texto::operator=(p_otra);
	return *this;
}

Representacion_texto_fija::Representacion_texto_fija(const DLibV::Superficie * p_rep, std::string const& p_cad,unsigned int p_ancho, unsigned int p_alto):
	Representacion_texto(p_rep)
{
	this->asignar(p_cad);

	this->establecer_dimensiones_area(p_ancho, p_alto);
	this->iniciar_recurso();
}

Representacion_texto_fija::Representacion_texto_fija(const DLibV::Superficie * p_rep, const char * p_char, unsigned int p_ancho, unsigned int p_alto)
	:Representacion_texto(p_rep)
{
	if(p_char) this->asignar(p_char);

	this->establecer_dimensiones_area(p_ancho, p_alto);
	this->iniciar_recurso();
}

Representacion_texto_fija::Representacion_texto_fija(const DLibV::Superficie * p_rep, const char p_char, unsigned int p_ancho, unsigned int p_alto)
	:Representacion_texto(p_rep)
{
	this->asignar(p_char);

	this->establecer_dimensiones_area(p_ancho, p_alto);
	this->iniciar_recurso();
}

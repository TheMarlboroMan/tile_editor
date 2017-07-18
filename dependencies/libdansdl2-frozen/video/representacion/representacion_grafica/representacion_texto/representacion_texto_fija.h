#ifndef REPRESENTACION_TEXTO_FIJA_H
#define REPRESENTACION_TEXTO_FIJA_H

#include "representacion_texto.h"

namespace DLibV
{

class Representacion_texto_fija:public Representacion_texto
{
	public:

						Representacion_texto_fija(const DLibV::Superficie * p_rep, const char p_char, unsigned int p_ancho, unsigned int p_alto);
						Representacion_texto_fija(const DLibV::Superficie * p_rep, const char * p_char, unsigned int p_ancho, unsigned int p_alto);
						Representacion_texto_fija(const DLibV::Superficie * p_rep, std::string const& p_cad,unsigned int p_ancho, unsigned int p_alto);
						Representacion_texto_fija(const Representacion_texto_fija&);
	Representacion_texto_fija& 		operator=(const Representacion_texto_fija&);
	virtual 				~Representacion_texto_fija() {}
};

} //Fin namespace DLibV

#endif

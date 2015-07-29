#include "configuracion.h"

const std::string Configuracion::CLAVE_IDIOMA="idioma";

Configuracion::Configuracion(): Configuracion_base()
{

}

Configuracion::~Configuracion()
{

}

void Configuracion::grabar_valores_configuracion(std::ofstream& archivo, const std::string& SEPARADOR)
{
	archivo<<CLAVE_IDIOMA<<SEPARADOR<<idioma<<std::endl;	
}

void Configuracion::procesar_clave_y_valor(const std::string& clave, const std::string& valor)
{
	if(clave.compare(CLAVE_IDIOMA)==0)
	{
		this->idioma=std::atoi(valor.c_str());
		return;
	}
}

void Configuracion::asignar_valores_por_defecto()
{
	idioma=0;
}

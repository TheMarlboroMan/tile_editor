#include "objeto_logica.h"
#include <sstream>
#include <stdexcept>
unsigned int Objeto_logica::id_actual=0;

std::string Objeto_logica::como_cadena() const
{
	std::stringstream ss;
	ss<<"TIPO: "<<tipo<<" ID: "<<id<<" POS: "<<x<<","<<y;
	return ss.str();
}

void Objeto_logica::reservar_propiedades(const std::vector<std::string>& valores_defecto)
{
	size_t i=1;
	for(const auto& v : valores_defecto) propiedades[i++]=v;
}

void Objeto_logica::asignar_propiedad(int clave, const std::string valor)
{
	propiedades[clave]=valor;
}

std::string Objeto_logica::valor_propiedad(int clave) const
{	
	if(propiedades.count(clave)) return propiedades.at(clave);
	else return "";
}


void Objeto_logica::serializar(std::ostream &stream, const char separador) const
{	
	stream<<tipo<<separador<<x<<separador<<y;
	for(const auto p:propiedades) stream<<separador<<p.second;
	stream<<"\n";
}

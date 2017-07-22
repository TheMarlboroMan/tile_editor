#include "objeto_logica.h"
#include <sstream>
#include <stdexcept>
unsigned int Objeto_logica::id_actual=0;

std::string Objeto_logica::como_cadena() const
{
	std::stringstream ss;
	ss<</*"TIPO: "<<tipo<<"*/" ID: "<<id<<" POS: "<<x<<","<<y;
	return ss.str();
}

void Objeto_logica::reservar_propiedades(const std::map<std::string, std::string>& p)
{
	propiedades=p;
}

void Objeto_logica::asignar_propiedad(const std::string& clave, const std::string& valor)
{
	propiedades[clave]=valor;
}

void Objeto_logica::serializar(std::ostream &stream, const char separador) const
{	
	stream<<tipo<<separador<<x<<separador<<y;
	for(const auto p:propiedades) stream<<separador<<p.first<<":"<<p.second;
	stream<<"\n";
}

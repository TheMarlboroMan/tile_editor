#include "logica.h"

int Logica::Propiedad::gid=0;

std::map<std::string, std::string> Logica::obtener_propiedades_defecto() const
{
	std::map<std::string, std::string> resultado;
	for(const auto& p : propiedades) resultado[p.nombre]=p.valor_defecto;
	return resultado;
}

std::string Logica::nombre_propiedad_por_indice(int indice) const
{
	if(indice < 0 || indice > propiedades.size())
	{
		throw std::runtime_error("Indice inválido para nombre_propiedad_por_indice");
	}
	else return propiedades[indice].nombre;
}

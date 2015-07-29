#ifndef LOGICA_H
#define LOGICA_H

/* Esta clase es el prototipo de un objeto de lógica. Los objetos de lógica
tendrán un id de tipo y según el id de tipo una ristra de parámetros. La
ristra de parámetros que corresponde con cada id de tipo se establece en 
esta clase.

Los objetos de lógica se unificarán en grupos de contenedores que contienen
los unos a los otros.

Tipo_objeto_logica -> Set_tipo_objeto_logica -> Coleccion_objeto_logica
*/

#include <vector>
#include <string>
#include "base_set.h"

class Logica
{
	public:

	struct Propiedad
	{
		int id;
		std::string nombre;
		std::string valor_defecto;

		Propiedad(int pid, const std::string& pn, const std::string& vd):id(pid), nombre(pn), valor_defecto(vd)
		{}
	};	

	private:

	int tipo, w_editor, h_editor, r_editor, g_editor, b_editor;
	std::string nombre;
	std::vector<Propiedad> propiedades;

	public:

	int acc_tipo() const {return tipo;}
	int acc_w_editor() const {return w_editor;}
	int acc_h_editor() const {return h_editor;}
	int acc_r_editor() const {return r_editor;}
	int acc_g_editor() const {return g_editor;}
	int acc_b_editor() const {return b_editor;}
	const std::string& acc_nombre() const {return nombre;}
	const std::vector<Propiedad>& acc_propiedades() const {return propiedades;}
	size_t acc_total_propiedades() const {return propiedades.size();}
	void insertar_propiedad(const std::string& pp, const std::string& pv) {propiedades.push_back(Propiedad(propiedades.size()+1, pp, pv));}

	Logica(int pt, int pw, int ph, int pr, int pg, int pb, const std::string& pn)
		:tipo(pt), w_editor(pw), h_editor(ph), r_editor(pr), g_editor(pg), b_editor(pb), nombre(pn)
	{}
};


class Logica_set:public Base_set<Logica>
{

};

#endif

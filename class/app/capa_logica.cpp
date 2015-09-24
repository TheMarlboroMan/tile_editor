#include "capa_logica.h"
#include <algorithm>
#include <vector>
#include <herramientas/log_base/log_base.h>

using namespace DLibH;
extern Log_base LOG;

void Capa_logica::insertar_objeto(int pt, int px, int py)
{
	auto buscar=[pt](const Logica& l) -> bool {return l.acc_tipo()==pt;};

	const Logica * t=tipos->buscar_unico_callback(buscar);

	if(!t)
	{
		LOG<<"Se intenta localizar tipo de objeto lógica "<<pt<<" en set: no existe"<<std::endl;
	}
	else
	{
		Objeto_logica ob(pt, px, py);

		auto& propiedades=t->acc_propiedades();
		std::vector<std::string> valores_defecto;
		for(auto prop : propiedades) valores_defecto.push_back(prop.valor_defecto);

		ob.reservar_propiedades(valores_defecto);
		objetos.push_back(ob);	
	}
}

void Capa_logica::insertar_objeto(int px, int py)
{
	insertar_objeto(indice_actual, px, py);
}

void Capa_logica::eliminar_objeto(int x, int y)
{
	auto it=std::find_if(std::begin(objetos), std::end(objetos),
		[x, y, this](const Objeto_logica& o) 
		{
			int t=o.acc_tipo();
			auto cb=[t](const Logica& ol) {return t==ol.acc_tipo();};
			auto tipo=(*tipos).buscar_unico_callback(cb);
			if(!tipo) return false;
			else return x >= o.acc_x()
			&& y >= o.acc_y()
			&& x < o.acc_x()+tipo->acc_w_editor() 
			&& y < o.acc_y()+tipo->acc_h_editor();
		});

	if(it!=std::end(objetos))
	{
		objetos.erase(it);
	}
}

void Capa_logica::eliminar_objeto(Objeto_logica * obj)
{
	auto it=std::find_if(std::begin(objetos), std::end(objetos),
		[obj, this](const Objeto_logica& o) 
		{
			return obj==&o;
		});

	if(it!=std::end(objetos))
	{
		objetos.erase(it);
	}
}



Objeto_logica * Capa_logica::obtener_objeto(int x, int y)
{
	Objeto_logica * resultado=nullptr;

	auto it=std::find_if(std::begin(objetos), std::end(objetos),
		[x, y, this](Objeto_logica& o) 
		{
			int t=o.acc_tipo();
			auto cb=[t](const Logica& ol) {return t==ol.acc_tipo();};
			auto tipo=(*tipos).buscar_unico_callback(cb);
			if(!tipo) return false;
			else return x >= o.acc_x()
			&& y >= o.acc_y()
			&& x < o.acc_x()+tipo->acc_w_editor() 
			&& y < o.acc_y()+tipo->acc_h_editor();
		});

	if(it!=std::end(objetos))
	{
		resultado=&*it;
	}

	return resultado;
}

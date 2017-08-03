#include "exportador.h"
#include "definiciones_importacion_exportacion.h"
#include <istream>

typedef Definiciones_importacion_exportacion DEFS;

			 
void Exportador::exportar(const std::vector<Rejilla>& rejillas, const std::vector<Capa_logica>& capas_logica, const std::vector<Propiedad_meta>& propiedades_meta,
	const Contenedor_tilesets& contenedor_tilesets, const Contenedor_logica_sets& contenedor_logica_sets,  const std::string nombre_fichero)
{
	std::ofstream fichero(nombre_fichero.c_str());

	if(!fichero)
	{
		throw Exportador_exception("El fichero "+nombre_fichero+" no pudo ser abierto");
	}
	else
	{
		//Información general...
		fichero<<DEFS::ABRE_ESTRUCTURA<<"\n"<<DEFS::ABRE_INFO<<"\n"<<rejillas.size()<<DEFS::SEPARADOR<<capas_logica.size()<<"\n";

		//Información meta...
		fichero<<DEFS::ABRE_META<<"\n";
		for(const auto&p : propiedades_meta) fichero<<p.acc_nombre()<<DEFS::SEPARADOR_PROPIEDADES_META<<p.acc_valor()<<"\n";
		fichero<<DEFS::CIERRA_META<<"\n";

		//Información de rejillas...

		for(auto& r: rejillas)
		{
			auto pres=r.acc_presentacion();

			//There is no alpha here... This is old map format.
			fichero<<DEFS::ABRE_REJILLA<<"\n"<<r.acc_w()<<DEFS::SEPARADOR<<r.acc_h()
				<<DEFS::SEPARADOR<<r.acc_w_celda()<<DEFS::SEPARADOR<<r.acc_h_celda()
				<<DEFS::SEPARADOR<<pres.w_unidades_separador<<DEFS::SEPARADOR<<pres.h_unidades_separador
				<<DEFS::SEPARADOR<<contenedor_tilesets.obtener_indice_item(r.acc_gestor())<<"\n"<<DEFS::ABRE_CELDA<<"\n";

			auto& m=r.r.acc_matriz();
			//TODO: Valorar si merece la pena poner sólo el índice en lugar de x e y.
			for(auto& c: m) fichero<<c.second.acc_x()<<DEFS::SEPARADOR<<c.second.acc_y()<<DEFS::SEPARADOR<<c.second.acc_indice_tile()<<DEFS::SEPARADOR_ITEMS;

			//Quitarle el último espacio...
			size_t pos=fichero.tellp();
			fichero.seekp(pos-1);
			fichero<<"\n";
		};

		//Información de capas de lógica...

		for(const auto& l:capas_logica)
		{
			const auto& objetos=l.acc_objetos();
			fichero<<DEFS::ABRE_LOGICA<<"\n"<<contenedor_logica_sets.obtener_indice_item(l.acc_gestor())<<"\n"<<DEFS::ABRE_OBJETOS<<"\n";
			for(const auto& o : objetos) o.serializar(fichero, DEFS::SEPARADOR);
		}

		fichero<<DEFS::CIERRA_ESTRUCTURA<<std::endl;
		fichero.close();
	}
}

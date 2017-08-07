#include "importador_dnot.h"

#include <class/dnot_parser.h>

void Importador_dnot::importar(std::vector<Rejilla>& rejillas, std::vector<Capa_logica>& capas_logica, std::vector<Propiedad_meta>& propiedades_meta, 
	const Contenedor_tilesets& contenedor_tilesets, const Contenedor_logica_sets& contenedor_logica_sets, const std::string nombre_fichero)
{
//TODO: Cool... The dnot parse explodes on empty objects.
//TODO: Test this in more modern versions.

	try
	{
		auto root=Herramientas_proyecto::parsear_dnot(nombre_fichero);

		rejillas.clear();
		capas_logica.clear();
		propiedades_meta.clear();

		//Create tile layers...
		for(const auto& l : root["data"]["layers"].acc_lista())
		{
			const auto& info=l["info"];

			//This may not be present, thus needs default values.
			int alpha=255;
			if(info.existe_clave("al")) alpha=info["al"];

			Rejilla::Datos_presentacion pres(info["wu"], info["hu"], alpha);

			rejillas.push_back(Rejilla(
				info["w"], info["h"],
				info["wc"], info["hc"],
				pres,
				contenedor_tilesets[(int)info["i"]]));
			auto& rej=rejillas.back();

			for(const auto& t: l["data"].acc_lista())
			{	
				rej.r((int)t["x"], (int)t["y"], Celda(t["x"],t["y"],(int)t["t"]));
			}
		}

		for(const auto& l : root["data"]["logic"].acc_lista())
		{
			capas_logica.push_back(Capa_logica(contenedor_logica_sets[(int)l["info"]["i"]]));
			auto& cl=capas_logica.back();
			for(const auto& o : l["data"].acc_lista())
			{
				int tipo=o["t"];

				auto cb=[tipo](const Logica& l) {return l.acc_tipo()==tipo;};
				auto * proto=cl.acc_gestor().buscar_unico_callback(cb);
				if(!proto) throw Importador_dnot_exception("Tipo lógica desconocida '"+std::to_string(tipo));

				//Set width and height, from prototype or as set by the user.
				int w=proto->acc_w_editor(), 
					h=proto->acc_h_editor();

				if(proto->es_resizable())
				{
					if(o.existe_clave("w")) w=o["w"];
					if(o.existe_clave("h")) h=o["h"];
				}

				Objeto_logica obj(tipo, o["x"], o["y"], w, h);

				
				auto& base=*proto;
				obj.reservar_propiedades(base.obtener_propiedades_defecto());

				//Now the recorded properties...
				for(const auto& p : o["p"].acc_tokens())
				{
					//Check properties.
					if(!base.existe_propiedad(p.first)) throw Importador_dnot_exception("Propiedad lógica inexistente '"+p.first+"'.");
					obj.asignar_propiedad(p.first, p.second.acc_string());
				}

				cl.insertar_objeto(obj);
			}
		}

		//Meta...
		for(const auto& m : root["data"]["meta"].acc_tokens())
		{
			//Emplace_back is always fun... Pass arguments to directly construct somewhere without copy.
			propiedades_meta.emplace_back(m.first, m.second.acc_string());
		}

	}
	catch(std::exception& e)
	{
		throw Importador_dnot_exception(std::string("Imposible cargar fichero: ")+e.what());
	}
}

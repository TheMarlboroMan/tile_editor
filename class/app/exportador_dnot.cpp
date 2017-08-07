#include "exportador_dnot.h"

#include <istream>
#include <class/dnot_token.h>

void Exportador_dnot::exportar(const std::vector<Rejilla>& rejillas, const std::vector<Capa_logica>& capas_logica, const std::vector<Propiedad_meta>& propiedades_meta,
	const Contenedor_tilesets& contenedor_tilesets, const Contenedor_logica_sets& contenedor_logica_sets,  const std::string nombre_fichero)
{
	using namespace Herramientas_proyecto;

	//Building the first level map
	Dnot_token::t_mapa mroot;

	//Building the second level maps.
	Dnot_token::t_mapa second_level;
//	second_level["info"]=Dnot_token(Dnot_token::t_mapa());
	second_level["meta"]=Dnot_token();
	second_level["layers"]=Dnot_token(Dnot_token::t_vector());
	second_level["logic"]=Dnot_token(Dnot_token::t_vector());

	//Building map info data... This is a way to build a dnot, very direct.
	//There's actually no need for this.
//	second_level["info"].acc_tokens().insert(Dnot_token::par_mapa("layers", Dnot_token((int)rejillas.size())));
//	second_level["info"].acc_tokens().insert(Dnot_token::par_mapa("logic", Dnot_token((int)capas_logica.size())));

	//Building each layer...
	//This is a less direct way... We are building a two-depth structure.
	for(auto& r: rejillas)
	{
		//This is the outmost level. It will be a map with another map and a vector inside.
		//We are taking another approach: not building anything into tok_layer yet..
		//We build it with an empty map.
		Dnot_token tok_layer(Dnot_token::t_mapa{});

		auto pres=r.acc_presentacion();

		//This is one of the innermost structures. We are building the map separatedly...
		Dnot_token::t_mapa mlayer_info;

		//Layer data.
		mlayer_info["w"]=Dnot_token(r.acc_w());
		mlayer_info["h"]=Dnot_token(r.acc_h());
		mlayer_info["wc"]=Dnot_token(r.acc_w_celda());
		mlayer_info["hc"]=Dnot_token(r.acc_h_celda());

		//Presentation data...
		mlayer_info["wu"]=Dnot_token(pres.w_unidades_separador);
		mlayer_info["hu"]=Dnot_token(pres.h_unidades_separador);
		mlayer_info["al"]=Dnot_token(pres.alpha);

		mlayer_info["i"]=Dnot_token((int)contenedor_tilesets.obtener_indice_item(r.acc_gestor()));

		//Now we build this shit into tok_layer, as we did earlier. Sounds misterious, but all we
		//are doing is get the tokens from tok_layer and add "info" with a new token assigned to
		//mlayer. tok_layer["info"] will not work, since it does not exist and this is not 
		//javascript. Also, if we did not build tok_layer with an empty map we'll get a runtime error.
		tok_layer.acc_tokens().insert(Dnot_token::par_mapa("info", Dnot_token(mlayer_info)));
		//Remember, any reference to mlayer_info does nothing now.

		//This is the other innermost structure. 
		//In this case it is a vector and will contain... more vectors. Let's build the vector first...
		tok_layer.acc_tokens().insert(Dnot_token::par_mapa("data", Dnot_token(Dnot_token::t_vector{})));

		auto& m=r.r.acc_matriz();
		for(auto& c: m) 
		{
			//This was a vector of numbers... Now it is a map :D.
//			Dnot_token::t_vector vtile;
//			vtile.push_back(Dnot_token((int)c.second.acc_x()));
//			vtile.push_back(Dnot_token((int)c.second.acc_y()));
//			vtile.push_back(Dnot_token((int)c.second.acc_indice_tile()));

			Dnot_token::t_mapa mtile;
			mtile["x"]=Dnot_token((int)c.second.acc_x());
			mtile["y"]=Dnot_token((int)c.second.acc_y());
			mtile["t"]=Dnot_token((int)c.second.acc_indice_tile());

			//To add anything we need to always add a token.
			tok_layer["data"].acc_lista().push_back(Dnot_token(mtile));
		}

		//Now we can push tok_layer...
		second_level["layers"].acc_lista().push_back(tok_layer);
	}

	//Bulding logic data. Similar to above...
	for(const auto& l:capas_logica)
	{
		Dnot_token tok_logic(Dnot_token::t_mapa{});

		Dnot_token::t_mapa mlogic_info;
		mlogic_info["i"]=Dnot_token((int)contenedor_logica_sets.obtener_indice_item(l.acc_gestor()));

		tok_logic.acc_tokens().insert(Dnot_token::par_mapa("info", Dnot_token(mlogic_info)));
		tok_logic.acc_tokens().insert(Dnot_token::par_mapa("data", Dnot_token(Dnot_token::t_vector{})));

		const auto& objetos=l.acc_objetos();
		for(auto& o: objetos) 
		{
			Dnot_token::t_mapa mobj;

			mobj["x"]=Dnot_token((int)o.acc_x());
			mobj["y"]=Dnot_token((int)o.acc_y());

			auto cb=[o](const Logica& l) {return l.acc_tipo()==o.acc_tipo();};
			auto * proto=l.acc_gestor().buscar_unico_callback(cb);
			if(!proto) throw Exportador_dnot_exception("Tipo lógica desconocida '"+std::to_string(o.acc_tipo()));

			if(proto->es_resizable())
			{
				mobj["w"]=Dnot_token((int)o.acc_w());
				mobj["h"]=Dnot_token((int)o.acc_h());
			}

			mobj["t"]=Dnot_token((int)o.acc_tipo());
			mobj["p"]=Dnot_token(Dnot_token::t_mapa{});

			//The different part... this is a weird map of <int, string>...
			//Why I did that is beyond me. It also will explode if new properties are
			//added to any game logic object prototype (except if added at the
			//beginning) but well...

			const auto& prop=o.acc_propiedades();
			for(const auto& p: prop) 
			{
	//			for(const auto& p: prop) mobj["p"].acc_lista().push_back(Dnot_token(p.second));
				mobj["p"].acc_tokens().insert(Dnot_token::par_mapa(p.first, Dnot_token(p.second)));
			}

			//To add anything we need to always add a token.
			tok_logic["data"].acc_lista().push_back(Dnot_token(mobj));
		}

		second_level["logic"].acc_lista().push_back(tok_logic);
	}

	//Building meta data... This is tricky.... meta data is a combination of
	//keys and values of a single depth. We will codify them all as strings.
	//To build this, we'll just create the map, add the values and them assign
	//to the token. This does not allow for deeper shit or even for escaping
	//data.

	Dnot_token::t_mapa mmeta;
	for(const auto&p : propiedades_meta) 
		mmeta[p.acc_nombre()]=Dnot_token(std::string(p.acc_valor()));

	second_level["meta"]=Dnot_token(mmeta);
	
	//Adding it all up.
	mroot["data"]=Dnot_token(second_level);
	Dnot_token root;
	root.asignar(mroot);

	//Save to disk.
	std::ofstream fichero(nombre_fichero.c_str());
	if(!fichero)
	{
		throw Exportador_dnot_exception("El fichero "+nombre_fichero+" no pudo ser abierto");
	}

	fichero<<root.serializar();
	fichero.close();
}

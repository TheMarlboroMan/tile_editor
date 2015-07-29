#ifndef PROYECTO_SDL2_DERIVADA_CARGADOR_RECURSOS_H
#define PROYECTO_SDL2_DERIVADA_CARGADOR_RECURSOS_H

#include "../base/cargador_recursos_base.h"

class Cargador_recursos:public Cargador_recursos_base
{
	private:

//	std::string textura;

	std::vector<std::string> entradas;

	protected: 

	std::vector<std::string> obtener_entradas_audio() const 
	{
		std::vector<std::string> resultado;
		return resultado;
	}

	std::vector<std::string> obtener_entradas_musica() const
	{
		std::vector<std::string> resultado;
		return resultado;
	}

	std::vector<std::string> obtener_entradas_texturas() const
	{
		std::string linea0 {"1	data/graficos/defecto.png	1	0	255	255"};
		std::vector<std::string> resultado = {linea0};

		unsigned int indice=PRIMER_INDICE;

		std::stringstream ss;

		for(auto& e : entradas)
		{	
			ss.str(std::string());
			ss<<indice++<<'\t'<<e;
			resultado.push_back(ss.str());
		}

		return resultado;
	}

	std::vector<std::string> obtener_entradas_superficies() const {return obtener_entradas_desde_ruta(std::string("data/recursos/superficies.txt"));}

	public:

	static const unsigned int PRIMER_INDICE=10;

	Cargador_recursos():Cargador_recursos_base() {}

	void asignar_entrada(const std::string &e) {entradas.push_back(e);}
	

//	void establecer_textura(const std::string& v) {textura=v;}
};

#endif

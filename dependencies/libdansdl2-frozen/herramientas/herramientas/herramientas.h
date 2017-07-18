#ifndef HERRAMIENTAS_DLIBDAN_H
#define HERRAMIENTAS_DLIBDAN_H

#include <cstring> //Para memset
#include <fstream>
#include <iostream>
#include <string>
#include <locale>
#include <cmath>
#include <cctype>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159
#endif

namespace DLibH
{

class Herramientas
{
	private:

	Herramientas() 
	{
		//MUhaahahaaaaaahaahaa!
	}

	static std::stringstream sstream;

	public:

	static unsigned int digitos_en_entero(const int&);
	static unsigned short int tipo_inicio_utf8(const char c);
	static bool es_inicio_utf8_2b(const char c);
	static bool es_inicio_utf8_3b(const char c);
	static bool es_inicio_utf8_4b(const char c);
	static bool es_inicio_utf8_5b(const char c);
	static bool es_inicio_utf8_6b(const char c);
	static unsigned char utf8_2b_a_uchar(char a, char b);
	static int abs(int &p_valor);
	static char * cadena_w_a_8(const wchar_t * p_cadena);
//	static int cadena_a_entero(const wchar_t * p_cadena);
//	static int cadena_a_entero(const char * p_cadena);
//	static std::string entero_a_cadena(int numero);
//	static std::string float_a_cadena(float numero);
	static std::string cadena_a_alfanumerica_normalizada(const char *, const char);
	static std::string cadena_a_alfanumerica_normalizada(std::string const &, const char);
	static std::string cadena_a_maximo_ancho(std::string const &, unsigned int);
	static std::string cadena_desde_fichero(const char *);
	
	template<typename T>
	static float grados_a_radianes(T val)
	{
	    return (val * M_PI) / 180.0;
	}

	template<typename T>
	static float radianes_a_grados(T val)
	{
	    return ( val * 180.0) / M_PI;
	}

	//Sólo válido para líneas rectas, of course.
	template<typename T>
	static bool segmentos_superpuestos(T aini, T afin, T bini, T bfin, bool roce_es_colision=false)
	{
	 	//Comparamos si está fuera y lo negamos...
	        if(roce_es_colision)
	        {
	                return !( (bfin < aini) || (bini > afin) );
	        }
	        else
	        {
	                return !( (bfin <= aini) || (bini >= afin) );
	        }
	}

	static float round(float);
	static float angulo_360(float f);
	static float angulo_entre_puntos(float, float, float, float);
	static bool validar_email(std::string);
//	static int cadena_a_entero(const std::string &)
	static std::vector<std::string> explotar(const std::string &, const char);
	static std::vector<std::string> explotar(const std::string &, const char, size_t max);
	static float porcentaje(float, float);
	static int obtener_aleatorio_tramo(int, int);
	static std::vector<unsigned int> explotar_linea_a_enteros(const std::string& linea, const char separador);
	static std::string obtener_siguiente_linea_archivo(std::ifstream& archivo, char c='#');

	template <typename T, typename U> static bool rectangulos_superpuestos(T, T, U, U, T, T, U, U, bool=false);
	template <typename T, typename U> static bool rectangulos_superpuestos(T, T, U, U, T, T, U, U, T&, T&, U&, U&, bool=false);
	template <typename T, typename U> static void posicion_dimension_segmentos(T, T, T, T, T&, U&);
	template <typename T, typename U> static void posicion_dimension_segmentos_solucion_parcial(T, T, T, T, T&, U&);
	template <typename T, typename U> static void posicion_dimension_segmentos_solucion_completa(T, T, T, T, T&, U&);

/*
	template <typename T> static bool rectangulos_superpuestos(T, T, T, T, T, T, T, T, bool=false);
	template <typename T> static bool rectangulos_superpuestos(T, T, T, T, T, T, T, T, T&, T&, T&, T&, bool=false);
	template <typename T> static void posicion_dimension_segmentos(T, T, T, T, T&, T&);
	template <typename T> static void posicion_dimension_segmentos_solucion_parcial(T, T, T, T, T&, T&);
	template <typename T> static void posicion_dimension_segmentos_solucion_completa(T, T, T, T, T&, T&);
*/

	//TODO: Hacer esto con templates...
	static bool caja_en_caja(int, int, unsigned int, unsigned int, int, int, unsigned int, unsigned int);
	static bool punto_en_caja(int, int, unsigned int, unsigned int, int, int);

};

}

#include "herramientas.templates.cpp"

#endif

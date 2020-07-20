#ifndef CAPA_COMPATIBILIDAD_REPRESENTACIONES_LIBDAN_SDL2_H
#define CAPA_COMPATIBILIDAD_REPRESENTACIONES_LIBDAN_SDL2_H

#include <def_video.h>

namespace DLibV
{

/** Representacion_agrupada */

class Representacion_agrupada_estatica
	:public Representacion_agrupada
{
	public:
			Representacion_agrupada_estatica(bool v=true)
				:Representacion_agrupada(v){hacer_estatica();}

			Representacion_agrupada_estatica(const Representacion_agrupada_estatica& v)
				:Representacion_agrupada(v){hacer_estatica();}

	virtual 	~Representacion_agrupada_estatica(){}
};

class Representacion_agrupada_dinamica
	:public Representacion_agrupada
{
	public:
			Representacion_agrupada_dinamica(bool v=true)
				:Representacion_agrupada(v){hacer_dinamica();}

			Representacion_agrupada_dinamica(const Representacion_agrupada_estatica& v)
				:Representacion_agrupada(v){hacer_dinamica();}

	virtual 	~Representacion_agrupada_dinamica(){}
};

/** Representacion bitmap */

class Representacion_bitmap_estatica
	:public Representacion_bitmap
{
	public:
			Representacion_bitmap_estatica()
				:Representacion_bitmap() {hacer_estatica();}
			Representacion_bitmap_estatica(const Representacion_bitmap& v)
				:Representacion_bitmap(v) {hacer_estatica();}
			Representacion_bitmap_estatica(const Textura * v)
				:Representacion_bitmap(v) {hacer_estatica();}
	Representacion_bitmap_estatica& operator=(const Representacion_bitmap_estatica & v)
	{
		Representacion_bitmap::operator=(v);
		hacer_estatica();
		return *this;
	}
	virtual 	~Representacion_bitmap_estatica() {}
};

class Representacion_bitmap_dinamica
	:public Representacion_bitmap
{
	public:
			Representacion_bitmap_dinamica()
				:Representacion_bitmap() {hacer_dinamica();}
			Representacion_bitmap_dinamica(const Representacion_bitmap& v)
				:Representacion_bitmap(v) {hacer_dinamica();}
			Representacion_bitmap_dinamica(const Textura * v)
				:Representacion_bitmap(v) {hacer_dinamica();}
	Representacion_bitmap_dinamica& operator=(const Representacion_bitmap_dinamica & v)
	{
		Representacion_bitmap::operator=(v);
		hacer_dinamica();
		return *this;
	}
	virtual 	~Representacion_bitmap_dinamica() {}
};

/** Representacion bitmap patrón */

class Representacion_bitmap_patron_estatica
	:public Representacion_bitmap_patron
{
	public:

			Representacion_bitmap_patron_estatica()
				:Representacion_bitmap_patron() {hacer_estatica();}
			Representacion_bitmap_patron_estatica(const Representacion_bitmap_patron& v)
				:Representacion_bitmap_patron(v) {hacer_estatica();}
			Representacion_bitmap_patron_estatica(const Textura * v)
				:Representacion_bitmap_patron(v) {hacer_estatica();}
			Representacion_bitmap_patron_estatica& operator=(const Representacion_bitmap_patron & v)
			{
				Representacion_bitmap_patron::operator=(v);
				hacer_estatica();
				return *this;
			}
	virtual 	~Representacion_bitmap_patron_estatica() {}
};

class Representacion_bitmap_patron_dinamica
	:public Representacion_bitmap_patron
{
	public:

			Representacion_bitmap_patron_dinamica()
				:Representacion_bitmap_patron() {hacer_dinamica();}
			Representacion_bitmap_patron_dinamica(const Representacion_bitmap_patron& v)
				:Representacion_bitmap_patron(v) {hacer_dinamica();}
			Representacion_bitmap_patron_dinamica(const Textura * v)
				:Representacion_bitmap_patron(v) {hacer_dinamica();}
			Representacion_bitmap_patron_dinamica& operator=(const Representacion_bitmap_patron & v)
			{
				Representacion_bitmap_patron::operator=(v);
				hacer_dinamica();
				return *this;
			}
	virtual 	~Representacion_bitmap_patron_dinamica() {}
};

/** Representación texto auto. */

class Representacion_texto_auto_estatica
	:public Representacion_texto_auto
{
	public:

			Representacion_texto_auto_estatica(const DLibV::Superficie * p_rep, const char p_char)
				:Representacion_texto_auto(p_rep, p_char) {hacer_estatica();}
			Representacion_texto_auto_estatica(const DLibV::Superficie * p_rep, const char * p_char)
				:Representacion_texto_auto(p_rep, p_char) {hacer_estatica();}
			Representacion_texto_auto_estatica(const DLibV::Superficie * p_rep, std::string const& p_cad)
				:Representacion_texto_auto(p_rep, p_cad) {hacer_estatica();}
			Representacion_texto_auto_estatica(const Representacion_texto_auto_estatica& v)
				:Representacion_texto_auto(v) {hacer_estatica();}
			Representacion_texto_auto_estatica& operator=(const Representacion_texto_auto_estatica& v)
			{
				Representacion_texto_auto::operator=(v);
				hacer_estatica();
				return *this;
			}
	virtual 	~Representacion_texto_auto_estatica() {}
};

class Representacion_texto_auto_dinamica
	:public Representacion_texto_auto
{
	public:

			Representacion_texto_auto_dinamica(const DLibV::Superficie * p_rep, const char p_char)
				:Representacion_texto_auto(p_rep, p_char) {hacer_dinamica();}
			Representacion_texto_auto_dinamica(const DLibV::Superficie * p_rep, const char * p_char)
				:Representacion_texto_auto(p_rep, p_char) {hacer_dinamica();}
			Representacion_texto_auto_dinamica(const DLibV::Superficie * p_rep, std::string const& p_cad)
				:Representacion_texto_auto(p_rep, p_cad) {hacer_dinamica();}
			Representacion_texto_auto_dinamica(const Representacion_texto_auto_dinamica& v)
				:Representacion_texto_auto(v) {hacer_dinamica();}
			Representacion_texto_auto_dinamica& operator=(const Representacion_texto_auto_dinamica& v)
			{
				Representacion_texto_auto::operator=(v);
				hacer_dinamica();
				return *this;
			}
	virtual 	~Representacion_texto_auto_dinamica() {}
};

/** Representación texto fija. */

class Representacion_texto_fija_estatica
	:public Representacion_texto_fija
{
	public:
			Representacion_texto_fija_estatica(const DLibV::Superficie * p_rep, const char p_char, unsigned int p_ancho, unsigned int p_alto)
				:Representacion_texto_fija(p_rep, p_char, p_ancho, p_alto) {hacer_estatica();}
			Representacion_texto_fija_estatica(const DLibV::Superficie * p_rep, const char * p_char, unsigned int p_ancho, unsigned int p_alto)
				:Representacion_texto_fija(p_rep, p_char, p_ancho, p_alto) {hacer_estatica();}
			Representacion_texto_fija_estatica(const DLibV::Superficie * p_rep, std::string const& p_cad,unsigned int p_ancho, unsigned int p_alto)
				:Representacion_texto_fija(p_rep, p_cad, p_ancho, p_alto) {hacer_estatica();}
			Representacion_texto_fija_estatica(const Representacion_texto_fija_estatica& v)
				:Representacion_texto_fija(v) {hacer_estatica();}
			Representacion_texto_fija_estatica& operator=(const Representacion_texto_fija_estatica& v)
			{
				Representacion_texto_fija::operator=(v);
				hacer_estatica();
				return *this;
			}
	virtual 	~Representacion_texto_fija_estatica() {}
};

class Representacion_texto_fija_dinamica
	:public Representacion_texto_fija
{
	public:
			Representacion_texto_fija_dinamica(const DLibV::Superficie * p_rep, const char p_char, unsigned int p_ancho, unsigned int p_alto)
				:Representacion_texto_fija(p_rep, p_char, p_ancho, p_alto) {hacer_dinamica();}
			Representacion_texto_fija_dinamica(const DLibV::Superficie * p_rep, const char * p_char, unsigned int p_ancho, unsigned int p_alto)
				:Representacion_texto_fija(p_rep, p_char, p_ancho, p_alto) {hacer_dinamica();}
			Representacion_texto_fija_dinamica(const DLibV::Superficie * p_rep, std::string const& p_cad,unsigned int p_ancho, unsigned int p_alto)
				:Representacion_texto_fija(p_rep, p_cad, p_ancho, p_alto) {hacer_dinamica();}
			Representacion_texto_fija_dinamica(const Representacion_texto_fija_dinamica& v)
				:Representacion_texto_fija(v) {hacer_dinamica();}
			Representacion_texto_fija_dinamica& operator=(const Representacion_texto_fija_dinamica& v)
			{
				Representacion_texto_fija::operator=(v);
				hacer_dinamica();
				return *this;
			}
	virtual 	~Representacion_texto_fija_dinamica() {}
};

/** Representacion TTF */

class Representacion_TTF_estatica
	:public Representacion_TTF
{
	public:

			Representacion_TTF_estatica(const Fuente_TTF& a, SDL_Color b, const std::string& c)
				:Representacion_TTF(a, b, c) {hacer_estatica();}
			Representacion_TTF_estatica(const Fuente_TTF& f, Uint8 r, Uint8 g, Uint8 b, Uint8 a, const std::string& c)
				:Representacion_TTF(f, r, g, b, a, c) {hacer_estatica();}
			Representacion_TTF_estatica(const Fuente_TTF& a, SDL_Color b)
				:Representacion_TTF(a, b) {hacer_estatica();}
			Representacion_TTF_estatica(const Fuente_TTF& f, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
				:Representacion_TTF(f, r, g, b, a) {hacer_estatica();}
			Representacion_TTF_estatica(const Representacion_TTF_estatica& v)
				:Representacion_TTF(v) {hacer_estatica();}
			Representacion_TTF_estatica& operator=(const Representacion_TTF_estatica& v)
			{
				Representacion_TTF::operator=(v);
				hacer_estatica();
				return *this;
			}
	virtual		~Representacion_TTF_estatica() {}
};

class Representacion_TTF_dinamica:
	public Representacion_TTF
{
	public:

			Representacion_TTF_dinamica(const Fuente_TTF& a, SDL_Color b, const std::string& c)
				:Representacion_TTF(a, b, c) {hacer_dinamica();}
			Representacion_TTF_dinamica(const Fuente_TTF& f, Uint8 r, Uint8 g, Uint8 b, Uint8 a, const std::string& c)
				:Representacion_TTF(f, r, g, b, a, c) {hacer_dinamica();}
			Representacion_TTF_dinamica(const Fuente_TTF& a, SDL_Color b)
				:Representacion_TTF(a, b) {hacer_dinamica();}
			Representacion_TTF_dinamica(const Fuente_TTF& f, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
				:Representacion_TTF(f, r, g, b, a) {hacer_dinamica();}
			Representacion_TTF_dinamica(const Representacion_TTF_dinamica& v)
				:Representacion_TTF(v) {hacer_dinamica();}
			Representacion_TTF_dinamica& operator=(const Representacion_TTF_dinamica& v)
			{
				Representacion_TTF::operator=(v);
				hacer_dinamica();
				return *this;
			}
	virtual		~Representacion_TTF_dinamica() {}
};

/** Representacion_primitiva_caja */

class Representacion_primitiva_caja_estatica
	:public Representacion_primitiva_caja
{
	public:
	
				Representacion_primitiva_caja_estatica(const SDL_Rect& p_pos, Uint8 pr, Uint8 pg, Uint8 pb)
					:Representacion_primitiva_caja(p_pos, pr, pg, pb) {hacer_estatica();}
				Representacion_primitiva_caja_estatica(const Representacion_primitiva_caja_estatica& v)
					:Representacion_primitiva_caja(v) {hacer_estatica();}
				Representacion_primitiva_caja_estatica& operator=(const Representacion_primitiva_caja_estatica& v)
				{
					Representacion_primitiva_caja::operator=(v);
					hacer_estatica();
					return * this;
				}
	virtual 		~Representacion_primitiva_caja_estatica() {}
};

class Representacion_primitiva_caja_dinamica
	:public Representacion_primitiva_caja
{
	public:
	
				Representacion_primitiva_caja_dinamica(const SDL_Rect& p_pos, Uint8 pr, Uint8 pg, Uint8 pb)
					:Representacion_primitiva_caja(p_pos, pr, pg, pb) {hacer_dinamica();}
				Representacion_primitiva_caja_dinamica(const Representacion_primitiva_caja_dinamica& v)
					:Representacion_primitiva_caja(v) {hacer_dinamica();}
				Representacion_primitiva_caja_dinamica& operator=(const Representacion_primitiva_caja_dinamica& v)
				{
					Representacion_primitiva_caja::operator=(v);
					hacer_dinamica();
					return * this;
				}
	virtual 		~Representacion_primitiva_caja_dinamica() {}
};

/** Representacion_primitiva_caja_líneas */

class Representacion_primitiva_caja_lineas_estatica
	:public Representacion_primitiva_caja_lineas
{
	public:

				Representacion_primitiva_caja_lineas_estatica(const SDL_Rect& p_pos, Uint8 pr, Uint8 pg, Uint8 pb)
					:Representacion_primitiva_caja_lineas(p_pos, pr, pg, pb){hacer_estatica();}
				Representacion_primitiva_caja_lineas_estatica(const Representacion_primitiva_caja_lineas_estatica& v)
					:Representacion_primitiva_caja_lineas(v) {hacer_estatica();}
				Representacion_primitiva_caja_lineas_estatica& operator=(const Representacion_primitiva_caja_lineas_estatica& v)
				{
					Representacion_primitiva_caja_lineas::operator=(v);
					hacer_estatica();
					return * this;
				}
	virtual 		~Representacion_primitiva_caja_lineas_estatica() {}
};

class Representacion_primitiva_caja_lineas_dinamica
	:public Representacion_primitiva_caja_lineas
{
	public:

				Representacion_primitiva_caja_lineas_dinamica(const SDL_Rect& p_pos, Uint8 pr, Uint8 pg, Uint8 pb)
					:Representacion_primitiva_caja_lineas(p_pos, pr, pg, pb){hacer_dinamica();}
				Representacion_primitiva_caja_lineas_dinamica(const Representacion_primitiva_caja_lineas_dinamica& v)
					:Representacion_primitiva_caja_lineas(v) {hacer_dinamica();}
				Representacion_primitiva_caja_lineas_dinamica& operator=(const Representacion_primitiva_caja_lineas_dinamica& v)
				{
					Representacion_primitiva_caja_lineas::operator=(v);
					hacer_dinamica();
					return * this;
				}
	virtual 		~Representacion_primitiva_caja_lineas_dinamica() {}
};

/** Representación primitiva línea */

class Representacion_primitiva_linea_estatica
	:public Representacion_primitiva_linea
{
	public:
	
				Representacion_primitiva_linea_estatica(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b)
					:Representacion_primitiva_linea(x1, y1, x2, y2, r, g, b) {hacer_estatica();}
				Representacion_primitiva_linea_estatica(const Representacion_primitiva_linea_estatica& v)
					:Representacion_primitiva_linea(v) {hacer_estatica();}
				Representacion_primitiva_linea_estatica& operator=(const Representacion_primitiva_linea_estatica& v)
				{
					Representacion_primitiva_linea::operator=(v);
					hacer_estatica();
					return *this;
				}
	virtual 		~Representacion_primitiva_linea_estatica() {}
};

class Representacion_primitiva_linea_dinamica
	:public Representacion_primitiva_linea
{
	public:
	
				Representacion_primitiva_linea_dinamica(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b)
					:Representacion_primitiva_linea(x1, y1, x2, y2, r, g, b) {hacer_dinamica();}
				Representacion_primitiva_linea_dinamica(const Representacion_primitiva_linea_dinamica& v)
					:Representacion_primitiva_linea(v) {hacer_dinamica();}
				Representacion_primitiva_linea_dinamica& operator=(const Representacion_primitiva_linea_dinamica& v)
				{
					Representacion_primitiva_linea::operator=(v);
					hacer_dinamica();
					return *this;
				}
	virtual 		~Representacion_primitiva_linea_dinamica() {}
};

/** Representacion primitiva puntos ... */

class Representacion_primitiva_puntos_estatica
	:public Representacion_primitiva_puntos
{
	public:
	
				Representacion_primitiva_puntos_estatica(Uint8 r, Uint8 g, Uint8 b)
					:Representacion_primitiva_puntos(r, g, b) {hacer_estatica();}
				Representacion_primitiva_puntos_estatica(int x, int y, Uint8 r, Uint8 g, Uint8 b)
					:Representacion_primitiva_puntos(x, y, r, g, b) {hacer_estatica();}
				Representacion_primitiva_puntos_estatica(const Representacion_primitiva_puntos_estatica& v)
					:Representacion_primitiva_puntos(v) {hacer_estatica();}
				Representacion_primitiva_puntos_estatica& operator=(const Representacion_primitiva_puntos_estatica& v)
				{
					Representacion_primitiva_puntos::operator=(v);
					hacer_estatica();
					return *this;
				}
	virtual			~Representacion_primitiva_puntos_estatica() {}
};

class Representacion_primitiva_puntos_dinamica
	:public Representacion_primitiva_puntos
{
	public:
	
				Representacion_primitiva_puntos_dinamica(Uint8 r, Uint8 g, Uint8 b)
					:Representacion_primitiva_puntos(r, g, b) {hacer_dinamica();}
				Representacion_primitiva_puntos_dinamica(int x, int y, Uint8 r, Uint8 g, Uint8 b)
					:Representacion_primitiva_puntos(x, y, r, g, b) {hacer_dinamica();}
				Representacion_primitiva_puntos_dinamica(const Representacion_primitiva_puntos_dinamica& v)
					:Representacion_primitiva_puntos(v) {hacer_dinamica();}
				Representacion_primitiva_puntos_dinamica& operator=(const Representacion_primitiva_puntos_dinamica& v)
				{
					Representacion_primitiva_puntos::operator=(v);
					hacer_dinamica();
					return *this;
				}
	virtual			~Representacion_primitiva_puntos_dinamica() {}
};


} //Fin namespace.

#endif

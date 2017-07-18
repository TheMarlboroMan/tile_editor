#include "representacion_ttf.h"

using namespace DLibV;

extern DLibH::Log_base LOG;

Representacion_TTF::Representacion_TTF(const Fuente_TTF& fuente, SDL_Color color, const std::string& texto)
	:Representacion_grafica(), 
	fuente(&fuente),
	cadena(texto),
	color(color)
{

}

Representacion_TTF::Representacion_TTF(const Fuente_TTF& fuente, Uint8 r, Uint8 g, Uint8 b, Uint8 a, const std::string& texto)
	:Representacion_grafica(), 
	fuente(&fuente),
	cadena(texto),
	color{r, g, b, a}
{

}

Representacion_TTF::Representacion_TTF(const Fuente_TTF& fuente, SDL_Color color)
	:Representacion_grafica(), 
	fuente(&fuente),
	cadena(""),
	color(color)
{

}

Representacion_TTF::Representacion_TTF(const Fuente_TTF& fuente, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
	:Representacion_grafica(), 
	fuente(&fuente),
	cadena(""),
	color{r, g, b, a}
{

}

Representacion_TTF::Representacion_TTF(const Representacion_TTF& o)
	:Representacion_grafica(o), 
	fuente(o.fuente),
	cadena(o.cadena),
	color(o.color)
{
	marcar_como_no_preparada();
}

Representacion_TTF::~Representacion_TTF()
{
	liberar_textura();
}

Representacion_TTF& Representacion_TTF::operator=(const Representacion_TTF& o)
{
	Representacion_grafica::operator=(o);
	fuente=o.fuente;
	cadena=o.cadena;
	color=o.color;
	marcar_como_no_preparada();

	return *this;
}

void Representacion_TTF::preparar(const SDL_Renderer * renderer)
{
	//Si el texto tiene newlines no va a funcionar: tenemos que montar
	//nosotros el código. Empezamos por partir el texto en varias líneas...
	std::vector<std::string> lineas=DLibH::Herramientas::explotar(cadena, '\n');

	//Y ahora medimos el que sería el tamaño total de la superficie del texto.
	int total_h=0, h=0, w=0, tw=0;

	for(const std::string& c : lineas)
	{
		TTF_SizeUTF8(const_cast<TTF_Font*>(fuente->acc_fuente()), c.c_str(), &tw, &h);
		if(tw > w) w=tw;
	}

	total_h=(lineas.size() * h);
;// TTF_FontLineSkip(const_cast<TTF_Font*>(fuente->acc_fuente()))) + h;
	
	//Podemos preparar una superficie de ese tamaño... Vamos a sacar una
	//superficie primero para obtener el formato... Es una mierda pero
	//me vale.
	SDL_Surface * s=TTF_RenderUTF8_Blended
			(const_cast<TTF_Font*>(fuente->acc_fuente()), 
			"a",
			color);
	std::unique_ptr<DLibV::Lienzo> lienzo(DLibV::Lienzo::generar_nuevo(w, total_h, s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, s->format->Bmask, s->format->Amask));
	SDL_FreeSurface(s);

	//Y ahora, por cada línea, crear una superficie y pegarla en el lienzo...
	int y=0;

	for(std::string& c : lineas)
	{
		reemplazar(c, "\t", "    ");
		const char * cad=c.size() ? c.c_str() : " \0";

		//TODO: Hay varios tipos de "blend". Podemos ponerlos aquí.
		SDL_Surface * s=TTF_RenderUTF8_Blended
			(const_cast<TTF_Font*>(fuente->acc_fuente()), 
			cad,
			color);	

		if(!s)
		{
			throw std::runtime_error("Error preparando Representacion_TTF : "+std::string(TTF_GetError())+" : "+c);
		}
		else
		{
			SDL_Rect pos{0, y, 0, 0}; 
			SDL_BlitSurface(s, NULL, lienzo->acc_superficie(), &pos);
			SDL_FreeSurface(s);
			y+=h;
		}		
	}

	DLibV::Textura * tex=new DLibV::Textura(renderer, lienzo->acc_superficie());

	establecer_textura(tex);
	establecer_modo_blend(Representacion::BLEND_ALPHA);

	establecer_recorte(0,0, tex->acc_w(), tex->acc_h());
	establecer_posicion(0, 0, tex->acc_w(), tex->acc_h(), FRECT_W|FRECT_H);	//Esto debemos llamarlo aquí, de lo contrario se queda con ancho y alto 0, dando problemas con las cámaras.

	Representacion_grafica::preparar(renderer);
}

void Representacion_TTF::modificar_fuente(const Fuente_TTF& f)
{
	fuente=&f;
	liberar_textura();
	marcar_como_no_preparada();
}

void Representacion_TTF::asignar(const char c)
{
	//Hay que joderse...
	std::string temp("");
	temp+=c;
	interno_asignar(temp);
}

void Representacion_TTF::asignar(const char * c)
{
	interno_asignar(c);
}

void Representacion_TTF::asignar(const std::string& c)
{
	interno_asignar(c);
}

void Representacion_TTF::interno_asignar(const std::string& c)
{
	if(c!=cadena)
	{
		cadena=c;
		liberar_textura();
		marcar_como_no_preparada();
	}
}

void Representacion_TTF::reemplazar(std::string& sujeto, const std::string& busca, const std::string& reemplaza)
{
	size_t pos = 0, l=reemplaza.length();
	while ((pos = sujeto.find(busca, pos)) != std::string::npos) 
	{
		sujeto.replace(pos, busca.length(), reemplaza);
		pos += l;
	}
}

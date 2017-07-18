#include <SDL2/SDL2_gfxPrimitives.h>
#include "representacion_primitiva_poligono.h"

using namespace DLibV;

Representacion_primitiva_poligono_base::Representacion_primitiva_poligono_base(const std::vector<punto>& puntos, Uint8 pr, Uint8 pg, Uint8 pb)
	:Representacion_primitiva(pr, pg, pb)
{
	//Crear array de puntos...
	for(const auto& p : puntos)
	{
		puntos_x.push_back(p.x);
		puntos_y.push_back(p.y);
	}

	//Calcular el rectángulo de posición para clip de cámara...
	this->preparar_posicion();
}

Representacion_primitiva_poligono_base::Representacion_primitiva_poligono_base(const Representacion_primitiva_poligono_base& p_otra)
	:Representacion_primitiva(p_otra)
{

}

Representacion_primitiva_poligono_base& Representacion_primitiva_poligono_base::operator=(const Representacion_primitiva_poligono_base& p_otro)
{
	Representacion_primitiva::operator=(p_otro);
	return *this;
}

Representacion_primitiva_poligono_base::~Representacion_primitiva_poligono_base()
{

}

bool Representacion_primitiva_poligono_base::volcado(SDL_Renderer * p_renderer)
{
	if(!es_visible()) return false;

	//Test para no dibujar fuera de pantalla...
	SDL_Rect pos=acc_posicion();

	int w=0, h=0;
	SDL_RenderGetLogicalSize(p_renderer, &w, &h);
	if( (pos.x + pos.w < 0) || (pos.y + pos.h < 0) || pos.x >= w || pos.y >= h)
	{
		return false;
	}

	SDL_RenderSetClipRect(p_renderer, NULL);

	if(es_rellena()) filledPolygonRGBA(p_renderer, puntos_x.data(), puntos_y.data(), puntos_x.size(), acc_r(), acc_g(), acc_b(), acc_alpha());
	else polygonRGBA(p_renderer, puntos_x.data(), puntos_y.data(), puntos_x.size(), acc_r(), acc_g(), acc_b(), acc_alpha());
	return true;
}

bool Representacion_primitiva_poligono_base::volcado(SDL_Renderer * p_renderer, const SDL_Rect& p_enfoque, const SDL_Rect& p_posicion, double zoom)
{
	if(!es_visible()) return false;

	SDL_Rect pos=copia_posicion();
	SDL_Rect clip=p_posicion;

	auto cpuntos_x=puntos_x, cpuntos_y=puntos_y;

	if(this->es_estatica())
	{
		//TODO: Check this...
		//Comprobar si, como estática, estaría dentro del enfoque de la cámara (básicamente 0,0 ancho y alto).
		SDL_Rect caja_cam=DLibH::Herramientas_SDL::nuevo_sdl_rect(0, 0, p_enfoque.w, p_enfoque.h);

		if(!DLibH::Herramientas_SDL::rectangulos_superpuestos(caja_cam, pos, true))
		{
			return false;
		}
	}
	else
	{
		if(!DLibH::Herramientas_SDL::rectangulos_superpuestos(p_enfoque, pos, true))
		{
			return false;
		}

		//Transformar todos los puntos!!!
		for(auto &p : cpuntos_x) p-=p_enfoque.x;
		for(auto &p : cpuntos_y) p-=p_enfoque.y;
	}

	SDL_RenderSetClipRect(p_renderer, &clip);

	if(zoom!=1.0)
	{
		auto proc=[zoom](Sint16& n) {n/=zoom;};
		for(auto& p : cpuntos_x) proc(p);
		for(auto& p : cpuntos_y) proc(p);
	}

	if(es_rellena()) filledPolygonRGBA(p_renderer, cpuntos_x.data(), cpuntos_y.data(), puntos_x.size(), acc_r(), acc_g(), acc_b(), acc_alpha());
	else polygonRGBA(p_renderer, cpuntos_x.data(), cpuntos_y.data(), puntos_x.size(), acc_r(), acc_g(), acc_b(), acc_alpha());
	return true;
}

void Representacion_primitiva_poligono_base::preparar_posicion()
{
	int x=puntos_x[0], y=puntos_y[0], maxx=x, maxy=y;

	for(const auto& px : puntos_x)
	{
		if(px < x) x=px;
		else if(px > maxx) maxx=px;
	}

	for(const auto& py : puntos_y)
	{
		if(py < y) y=py;
		else if(py > maxy) maxy=py;
	}

	SDL_Rect p_pos {x, y, maxx-x, maxy-y};
	establecer_posicion(p_pos);
}	

Representacion_primitiva_poligono& Representacion_primitiva_poligono::operator=(const Representacion_primitiva_poligono& p_otro)
{
	Representacion_primitiva_poligono_base::operator=(p_otro);
	return *this;
}

Representacion_primitiva_poligono::Representacion_primitiva_poligono(const std::vector<punto> &puntos, Uint8 pr, Uint8 pg, Uint8 pb)
	:Representacion_primitiva_poligono_base(puntos, pr, pg, pb)
{

}

Representacion_primitiva_poligono::Representacion_primitiva_poligono(const Representacion_primitiva_poligono& p_otra)
	:Representacion_primitiva_poligono_base(p_otra) 
{

}

Representacion_primitiva_poligono_lineas& Representacion_primitiva_poligono_lineas::operator=(const Representacion_primitiva_poligono_lineas& p_otro)
{
	Representacion_primitiva_poligono_base::operator=(p_otro);
	return *this;
}

Representacion_primitiva_poligono_lineas::Representacion_primitiva_poligono_lineas(const std::vector<punto>& puntos, Uint8 pr, Uint8 pg, Uint8 pb)
	:Representacion_primitiva_poligono_base(puntos, pr, pg, pb)
{

}

Representacion_primitiva_poligono_lineas::Representacion_primitiva_poligono_lineas(const Representacion_primitiva_poligono_lineas& p_otra)
	:Representacion_primitiva_poligono_base(p_otra) 
{

}

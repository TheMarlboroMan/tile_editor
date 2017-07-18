#include "representacion_grafica.h"

#include <algorithm>
#include "../../../herramientas/poligono_2d/poligono_2d.h"

using namespace DLibV;

extern DLibH::Log_base LOG;

Representacion_grafica::Representacion_grafica()
	:Representacion(), textura(nullptr), preparada(false)
{
	this->reiniciar_posicion();
	this->reiniciar_recorte();
	actualizar_caja_rotacion();
}

Representacion_grafica::Representacion_grafica(const Representacion_grafica& o)
	:Representacion(o) ,textura(o.textura), preparada(o.preparada),
	posicion_rotada(o.posicion_rotada)
{

}

Representacion_grafica& Representacion_grafica::operator=(const Representacion_grafica& o)
{
	Representacion::operator=(o);
	textura=o.textura;
	preparada=o.preparada;
	posicion_rotada=o.posicion_rotada;

	return *this;
}

Representacion_grafica::~Representacion_grafica()
{
	//OJO: No se borra el recurso gráfico porque se asume que lo hemos
	//obtenido de un gestor de recursos. Esta clase base NO gestiona los
	//recursos asignados. Una superior, en cambio, si podría.
}

void Representacion_grafica::recorte_a_medidas_textura()
{
	establecer_recorte(0,0, textura->acc_w(), textura->acc_h());
}

bool Representacion_grafica::realizar_render(SDL_Renderer * p_renderer, SDL_Rect& rec, SDL_Rect& pos)
{
	SDL_Texture * tex=ref_textura_sdl();

	switch(acc_modo_blend())
	{
		case BLEND_NADA: SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_NONE); break;
		case BLEND_ALPHA: SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND); break;
		case BLEND_SUMA: SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_ADD); break;
		case BLEND_MODULAR: SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_MOD); break;
	};

	SDL_SetTextureAlphaMod(tex, acc_alpha());
	SDL_SetTextureColorMod(tex, acc_mod_color_r(), acc_mod_color_g(), acc_mod_color_b());

	if(!es_transformada())
	{
		return SDL_RenderCopy(p_renderer, tex, &rec,&pos) >= 0;
	}
	else
	{
		DLibV::Representacion_grafica_transformacion t=acc_transformacion();

		float angulo=t.obtener_angulo_rotacion();
		SDL_RendererFlip flip=t.obtener_flip();

		if(t.es_cambia_centro_rotacion())
		{
			SDL_Point centro=t.obtener_centro_rotacion();
			return SDL_RenderCopyEx(p_renderer, tex, &rec, &pos, angulo, &centro, flip) >= 0;
		}
		else
		{
			return SDL_RenderCopyEx(p_renderer, tex, &rec, &pos, angulo, NULL, flip) >= 0;
		}
	}
}

bool Representacion_grafica::volcado(SDL_Renderer * p_renderer)
{
	//Si no está en la pantalla no perdemos tiempo.
	if(!this->es_visible())
	{
		return false;
	}
	else
	{
		if(!this->es_preparada()) 
		{
			this->preparar(p_renderer);
		}


		//Definimos aquí estas variables: puede que al "preparar" hayan cambiado los valores.
		SDL_Rect rec=acc_recorte();
		SDL_Rect pos=acc_posicion();
		SDL_Rect temp=DLibH::Herramientas_SDL::copiar_sdl_rect(pos, 0, 0);

		//Esto es especialmente útil para cuando vamos a hacer 
		//bitmaps en patrón... Para poco más, normalmente es la
		//posición de la representación.
		auto clip_rect=obtener_caja_clip();
		SDL_RenderSetClipRect(p_renderer, &clip_rect);

		return realizar_render(p_renderer, rec, temp);
	}
}

bool Representacion_grafica::volcado(SDL_Renderer * p_renderer, const SDL_Rect& p_foco, const SDL_Rect& p_pos, double zoom)
{
	if(!this->es_visible())
	{
		return false;
	}
	else
	{	
		//Se prepara antes de comparar si está dentro o fuera de la toma: es 
		//posible que el tamaño se establezca después de prepararla!!!.
		if(!this->es_preparada()) this->preparar(p_renderer);

		//Las representaciones estáticas están SIEMPRE en las mismas
		//posiciones para la cámara que la vea. Simplemente veremos
		//si está dentro de la caja de la cámara en 0,0.

		SDL_Rect pos=copia_posicion_rotada();

		bool en_toma=true;

		if(this->es_estatica())
		{
			SDL_Rect caja_cam=DLibH::Herramientas_SDL::nuevo_sdl_rect(0, 0, p_foco.w, p_foco.h);
			en_toma=DLibH::Herramientas_SDL::rectangulos_superpuestos(caja_cam, pos, false);
		}
		else
		{
			en_toma=DLibH::Herramientas_SDL::rectangulos_superpuestos(p_foco, pos, false);
		}

		if(!en_toma)
		{
			return false;
		}
		else
		{
			SDL_Rect rec=copia_recorte();
			pos=copia_posicion();	//Again, por si al preparar ha cambiado.

			//Una representación estática aparecerá en la posición absoluta del objeto más la posición de la cámara.	
			pos.x+=p_pos.x;
			pos.y+=p_pos.y;

			//Una representación dinámica tiene un cálculo distinto, relativo a la cámara.
			if(!this->es_estatica())
			{
				pos.x-=p_foco.x;
				pos.y-=p_foco.y;
			}

			//Proceso del zoom...
			procesar_zoom(pos, zoom);

			SDL_Rect clip_rect=p_pos; //La representación no puede salirse de la cámara. Fin.
			SDL_RenderSetClipRect(p_renderer, &clip_rect);
			return realizar_render(p_renderer, rec, pos);
		}
	}
}


//Eso sólo deberíamos llamarlo en aquellas para las cuales hemos creado una
//textura que poseen. Si es parte de un recurso que no es de su propiedad
//te vas a encontrar con un problema.

void Representacion_grafica::liberar_textura()
{
	if(this->textura)
	{
		delete this->textura;
		this->textura=NULL;
	}
}

void Representacion_grafica::transformar_rotar(float v) 
{
	transformacion.rotar(v);
	actualizar_caja_rotacion();
}

void Representacion_grafica::transformar_cancelar_rotar() 
{
	transformacion.cancelar_rotar();
	actualizar_caja_rotacion();
}

void Representacion_grafica::transformar_centro_rotacion(float x, float y) 
{
	transformacion.centro_rotacion(x, y);
	actualizar_caja_rotacion();
}

void Representacion_grafica::transformar_centro_rotacion_cancelar() 
{
	transformacion.cancelar_centro_rotacion();
	actualizar_caja_rotacion();
}

void Representacion_grafica::actualizar_caja_rotacion()
{
	const auto& p=acc_posicion();

	if(!transformacion.es_transformacion())
	{
		posicion_rotada=p;
	}
	else
	{
		auto c=transformacion.obtener_centro_rotacion();
		DLibH::Poligono_2d_vertices<double> polig(
			{ 
				{(double)p.x, (double)p.y},
				{(double)(p.x+p.w), (double)p.y},
				{(double)(p.x+p.w), (double)(p.y+p.h)},
				{(double)p.x, (double)(p.y+p.h)},        
			}, {(double)c.x+p.x, (double)c.y+p.y});

		//Las rotaciones de SDL son "clockwise"... Las reales son "counter-clockwise"...
		float a=transformacion.obtener_angulo_rotacion();
		polig.rotar(a);

		//Sacar las medidas para la nueva caja...
		std::vector<double> xs={polig.vertice(0).x, polig.vertice(1).x, polig.vertice(2).x, polig.vertice(3).x};
		std::vector<double> ys={polig.vertice(0).y, polig.vertice(1).y, polig.vertice(2).y, polig.vertice(3).y};

		posicion_rotada.x=*std::min_element(std::begin(xs), std::end(xs));
		posicion_rotada.y=*std::min_element(std::begin(ys), std::end(ys));
		posicion_rotada.w=*std::max_element(std::begin(xs), std::end(xs))-posicion_rotada.x;
		posicion_rotada.h=*std::max_element(std::begin(ys), std::end(ys))-posicion_rotada.y;
	}
}

void Representacion_grafica::establecer_posicion(int x, int y, int w, int h, int f)
{
	Representacion::establecer_posicion(x, y, w, h, f);
	actualizar_caja_rotacion();
}

void Representacion_grafica::establecer_posicion(SDL_Rect c)
{
	Representacion::establecer_posicion(c);
	actualizar_caja_rotacion();
}

SDL_Rect Representacion_grafica::copia_posicion_rotada() const
{
	return SDL_Rect{posicion_rotada.x, posicion_rotada.y, posicion_rotada.w, posicion_rotada.h};
}

void Representacion_grafica::preparar(const SDL_Renderer * renderer)
{
	actualizar_caja_rotacion();
	marcar_como_preparada();
}

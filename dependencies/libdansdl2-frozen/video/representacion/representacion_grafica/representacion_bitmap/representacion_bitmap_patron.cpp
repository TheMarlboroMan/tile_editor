#include "representacion_bitmap_patron.h"
using namespace DLibV;

Representacion_bitmap_patron::Representacion_bitmap_patron()
	:Representacion_bitmap(), 
	pincel{0,0,0,0}, pincel_x(0), pincel_y(0)
{

}

Representacion_bitmap_patron::Representacion_bitmap_patron(const Representacion_bitmap_patron& r)
	:Representacion_bitmap(r),
	pincel{0,0,0,0}, pincel_x(0), pincel_y(0)
{

}

Representacion_bitmap_patron::Representacion_bitmap_patron(const Textura * t)
	:Representacion_bitmap(t),
	pincel{0,0,0,0}, pincel_x(0), pincel_y(0)
{

}

Representacion_bitmap_patron& Representacion_bitmap_patron::operator=(const Representacion_bitmap_patron &o)
{
	Representacion_bitmap::operator=(o);
	pincel=o.pincel;
	pincel_x=0;
	pincel_y=0;

	return *this;
}

Representacion_bitmap_patron::~Representacion_bitmap_patron()
{

}

void Representacion_bitmap_patron::establecer_pincel(SDL_Rect r)
{
	pincel=r;
}

/*
* Llamaremos varias veces a la función de volcado original. Contaría como un
* único render para la cuenta de pantalla, pero me da igual.
*/

bool Representacion_bitmap_patron::volcado(SDL_Renderer * p_renderer)
{
	//Si no está en la pantalla no perdemos tiempo.
	if(!this->es_visible())
	{
		return false;
	}

	SDL_Rect copia_posicion=acc_posicion();

	//Asignamos para que haga de clip.
	caja_clip=copia_posicion;

	int max_x=copia_posicion.x+copia_posicion.w,
		max_y=copia_posicion.y+copia_posicion.h;

	int y=copia_posicion.y-pincel.y;

	bool resultado=true;

	while(y < max_y)
	{
		int x=copia_posicion.x-pincel.x;

		pincel_y=y;
		while(x < max_x)
		{
			pincel_x=x;
			resultado=resultado && Representacion_grafica::volcado(p_renderer);
			marcar_como_no_preparada();
			establecer_posicion(copia_posicion);
			x+=pincel.w;
		}
		y+=pincel.h;
	};

	establecer_posicion(copia_posicion);
	return resultado;
}

bool Representacion_bitmap_patron::volcado(SDL_Renderer * p_renderer, const SDL_Rect& caja_a, const SDL_Rect& caja_b, double zoom)
{
	//Si no está en la pantalla no perdemos tiempo.
	if(!this->es_visible())
	{
		return false;
	}

	SDL_Rect copia_posicion=acc_posicion();

	//Asignamos para que haga de clip.
	caja_clip=copia_posicion;

	int max_x=copia_posicion.x+copia_posicion.w,
		max_y=copia_posicion.y+copia_posicion.h;

	int y=copia_posicion.y-pincel.y;

	bool resultado=true;

	while(y < max_y)
	{
		int x=copia_posicion.x-pincel.x;

		pincel_y=y;
		while(x < max_x)
		{
			pincel_x=x;
			resultado=resultado && Representacion_grafica::volcado(p_renderer, caja_a, caja_b, zoom);
			marcar_como_no_preparada();
			establecer_posicion(copia_posicion);
			x+=pincel.w;
		}
		y+=pincel.h;
	};

	establecer_posicion(copia_posicion);
	return resultado;
}

/*
* La llamada a preparar es un super hack... Al preparar cambiamos la posición
* para engañar a la función "volcado" base y decirle que la posición es el 
* trozo que estamos dibujando ahora. Después de volcarla la marcamos como no
* preparada para que haya que hacer el hack de nuevo.
*
* Todo eso está envuelto en otro hack más, que es hacer una copia de la posición
* que se restaura al final de la llamada. Esa copia se usará como caja de clip.
*/

void Representacion_bitmap_patron::preparar(const SDL_Renderer * renderer)
{
	//Super hack..
	establecer_posicion(pincel_x, pincel_y, pincel.w, pincel.h);
	Representacion_grafica::preparar(renderer);
}

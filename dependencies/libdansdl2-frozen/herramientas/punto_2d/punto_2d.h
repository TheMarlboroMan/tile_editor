#ifndef PUNTO_2D_LIBDANSDL_H
#define PUNTO_2D_LIBDANSDL_H

#include <cmath>
#include "../herramientas/herramientas.h" //Por si acaso no hay M_PI...

namespace DLibH
{

template<typename T>
struct Punto_2d
{
	public:

	T x,y;

	Punto_2d(): x(0), y(0) {}
	Punto_2d(T px, T py):x(px), y(py) {}
	Punto_2d(const Punto_2d<T>& p):x(p.x), y(p.y) {}
	Punto_2d& operator=(const Punto_2d<T>& p)
	{
		x=p.x;
		y=p.y;
		return *this;
	}

	Punto_2d operator+(const Punto_2d<T>& p) const
	{
		return Punto_2d<T> {x+p.x, y+p.y};
	}

	Punto_2d operator*(T p) const
	{
		return Punto_2d<T> {x*p, y*p};
	}

	Punto_2d operator*=(T p)
	{
		x*=p;
		y*=p;
		return *this;
	}

	Punto_2d operator/=(T p)
	{
		x/=p;
		y/=p;
		return *this;
	}

	Punto_2d operator/(T p) const
	{
		return Punto_2d<T> {x/p, y/p};
	}

	Punto_2d operator-(const Punto_2d& p) const
	{
		return Punto_2d<T> {x-p.x, y-p.y};
	}

	Punto_2d& operator+=(const Punto_2d<T>& p)
	{
		x+=p.x;
		y+=p.y;
		return *this;
	}

	Punto_2d& operator-=(const Punto_2d& p)
	{
		x-=p.x;
		y-=p.y;
		return *this;
	}

	bool 		operator==(const Punto_2d& p) const
	{
		return p.x==x && p.y==y;
	}

	T distancia_hasta(const Punto_2d<T>& p) const
	{
		T x=(this->x-p.x)*(this->x-p.x);
		T y=(this->y-p.y)*(this->y-p.y);
		return sqrt(x+y);
	}

	static T distancia_entre(const Punto_2d<T>& p1, const Punto_2d<T>& p2)
	{
		T x=(p1.x-p2.x)*(p1.x-p2.x);
		T y=(p1.y-p2.y)*(p1.y-p2.y);
		return sqrt(x+y);
	}

	//La rotación es negativa si va en sentido de las agujas del reloj...
	void rotar(T grados, const Punto_2d<T> centro)
	{
		//Llevar a origen...
		T ox=x - centro.x;
		T oy=y - centro.y;

		//Precalculos...
		T rad=Herramientas::grados_a_radianes(grados);
		T sr=sin(rad);
		T cr=cos(rad);

		//Rotar...
		T rx=(ox * cr) - (oy * sr);
		T ry=(oy * cr) + (ox * sr);

		//Desplazar de nuevo...
		x = rx + centro.x;
		y = ry + centro.y;
	}

};

}//Fin namespace...

#endif

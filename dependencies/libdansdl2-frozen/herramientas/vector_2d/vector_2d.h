#ifndef VECTOR_2D_H
#define VECTOR_2D_H

#include <cmath>
#include "../herramientas/herramientas.h"

namespace DLibH
{

/**
* Vector simple... Para uso normal y corriente.
*/

template<typename T>
struct Vector_2d
{
	T x;
	T y;

						Vector_2d()
		:x(), y() 
	{

	}

						Vector_2d(const Vector_2d<T>& o)
		:x(o.x), y(o.y) 
	{

	}

						Vector_2d(T p_x, T p_y)
		:x(p_x), y(p_y) 
	{
	
	}

	Vector_2d<T> operator+(const Vector_2d<T> &otro)
	{
		return Vector_2d<T>(this->x+otro.x, this->y+otro.y);
	}

	Vector_2d<T> 				operator-(const Vector_2d<T> &otro)
	{
		return Vector_2d<T>(this->x-otro.x, this->y-otro.y);
	}

	Vector_2d<T> 				operator*(const Vector_2d<T> &otro)
	{
		return Vector_2d<T>(this->x*otro.x, this->y*otro.y);
	}

	Vector_2d<T> 				operator/(const Vector_2d<T> &otro)
	{
		return Vector_2d<T>(this->x/otro.x, this->y/otro.y);
	}

	Vector_2d<T>& 				operator=(const Vector_2d<T> &otro)
	{
		this->x=otro.x;
		this->y=otro.y;
		return *this;
	}

	Vector_2d<T>& 				operator+=(const Vector_2d<T> &otro)
	{
		this->x+=otro.x;
		this->y+=otro.y;
		return *this;
	}

	Vector_2d<T>&				operator-=(const Vector_2d<T> &otro)
	{
		this->x-=otro.x;
		this->y-=otro.y;
		return *this;
	}

	Vector_2d<T>&				operator*=(const Vector_2d<T> &otro)
	{
		this->x*=otro.x;
		this->y*=otro.y;
		return *this;
	}

	Vector_2d<T>& 				operator/=(const Vector_2d<T> &otro)
	{
		this->x/=otro.x;
		this->y/=otro.y;
		return *this;
	}

	Vector_2d<T>& 				operator*(const T p_multiplicador)
	{
		this->x=this->x*p_multiplicador;
		this->y=this->y*p_multiplicador;
		return *this;
	}

	Vector_2d<T>& 				operator/(const T p_divisor)
	{
		this->x=this->x/p_divisor;
		this->y=this->y/p_divisor;
		return *this;
	}

	Vector_2d<T>& 				operator*=(const T p_multiplicador)
	{
		this->x*=p_multiplicador;
		this->y*=p_multiplicador;
		return *this;
	}

	Vector_2d<T>& 				operator/=(const T p_divisor)
	{
		this->x/=p_divisor;
		this->y/=p_divisor;
		return *this;
	}

	Vector_2d<T>				perpendicular() const
	{
		return Vector_2d<T>{this->y, -this->x};
	}

	void 					normalizar()
	{
		T m=this->longitud();
		this->x=this->x / m;
		this->y=this->y / m;
	}

	T 					longitud() const
	{
		return sqrtf(this->x*this->x + this->y*this->y);
	}

	T 					angulo_radianes() const
	{
		auto vec(*this);
		vec.normalizar();
		return obtener_angulo_para_vector_unidad_radianes(vec);
	}

	T 					angulo_grados() const
	{
		auto vec(*this);
		vec.normalizar();
		return obtener_angulo_para_vector_unidad_grados(vec);
	}
};

/**
* Este otro vector responde a las coordenadas de pantalla. Y negativo es arriba.
* Y positivo es abajo. Simplemente existe para ser un tipo separado y que no
* haya confusiones luego en el código cliente.
*/

template<typename T>
struct Vector_2d_pantalla:
	public Vector_2d<T>
{
						Vector_2d_pantalla():Vector_2d<T>() 
	{

	}

						Vector_2d_pantalla(T px, T py):Vector_2d<T>(px, py) 
	{

	}

						Vector_2d_pantalla(const Vector_2d_pantalla<T>& v)
	{
		this->x=v.x;
		this->y=v.x;
	}

						Vector_2d_pantalla& operator=(const Vector_2d_pantalla<T>& v)
	{
		Vector_2d<T>::operator=(v);
		return *this;
	}

	Vector_2d_pantalla 			operator+(const Vector_2d_pantalla<T> &otro)
	{
		return Vector_2d_pantalla(this->x+otro.x, this->y+otro.y);
	}

	Vector_2d_pantalla 			operator-(const Vector_2d_pantalla<T> &otro)
	{
		return Vector_2d_pantalla(this->x-otro.x, this->y-otro.y);
	}

	Vector_2d_pantalla 			operator*(const Vector_2d_pantalla<T> &otro)
	{
		return Vector_2d_pantalla(this->x*otro.x, this->y*otro.y);
	}

	Vector_2d_pantalla 			operator/(const Vector_2d_pantalla<T> &otro)
	{
		return Vector_2d_pantalla(this->x/otro.x, this->y/otro.y);
	}

	Vector_2d_pantalla& 			operator+=(const Vector_2d_pantalla<T> &otro)
	{
		this->x+=otro.x;
		this->y+=otro.y;
		return *this;
	}

	Vector_2d_pantalla& 			operator-=(const Vector_2d_pantalla<T> &otro)
	{
		this->x-=otro.x;
		this->y-=otro.y;
		return *this;
	}

	Vector_2d_pantalla& 			operator*=(const Vector_2d_pantalla<T> &otro)
	{
		this->x*=otro.x;
		this->y*=otro.y;
		return *this;
	}

	Vector_2d_pantalla& 			operator/=(const Vector_2d_pantalla<T> &otro)
	{
		this->x/=otro.x;
		this->y/=otro.y;
		return *this;
	}

	Vector_2d_pantalla&			operator*(const T p_multiplicador)
	{
		this->x=this->x*p_multiplicador;
		this->y=this->y*p_multiplicador;
		return *this;
	}

	Vector_2d_pantalla& 			operator/(const T p_divisor)
	{
		this->x=this->x/p_divisor;
		this->y=this->y/p_divisor;
		return *this;
	}

	Vector_2d_pantalla& 			operator*=(const T p_multiplicador)
	{
		this->x*=p_multiplicador;
		this->y*=p_multiplicador;
		return *this;
	}

	Vector_2d_pantalla& 			operator/=(const T p_divisor)
	{
		this->x/=p_divisor;
		this->y/=p_divisor;
		return *this;
	}

	Vector_2d_pantalla<T>			perpendicular() const
	{
		return Vector_2d_pantalla<T>{this->y, -this->x};
	}
};

/**
* Este vector es cartesiano: y positivo es arriba, y negativo es abajo.
*/

template<typename T>
struct Vector_2d_cartesiano:
	public Vector_2d<T>
{
						Vector_2d_cartesiano():Vector_2d<T>() 
	{

	}

						Vector_2d_cartesiano(T px, T py):Vector_2d<T>(px, py) 
	{

	}

						Vector_2d_cartesiano(const Vector_2d_cartesiano<T>& v)
	{
		this->x=v.x;
		this->y=v.y;
	}

						Vector_2d_cartesiano& operator=(const Vector_2d_cartesiano<T>& v)
	{
		Vector_2d<T>::operator=(v);
		return *this;
	}

	Vector_2d_cartesiano 			operator+(const Vector_2d_cartesiano<T> &otro)
	{
		return Vector_2d_cartesiano(this->x+otro.x, this->y+otro.y);
	}

	Vector_2d_cartesiano 			operator-(const Vector_2d_cartesiano<T> &otro)
	{
		return Vector_2d_cartesiano(this->x-otro.x, this->y-otro.y);
	}

	Vector_2d_cartesiano 			operator*(const Vector_2d_cartesiano<T> &otro)
	{
		return Vector_2d_cartesiano(this->x*otro.x, this->y*otro.y);
	}

	Vector_2d_cartesiano 			operator/(const Vector_2d_cartesiano<T> &otro)
	{
		return Vector_2d_cartesiano(this->x/otro.x, this->y/otro.y);
	}

	Vector_2d_cartesiano& 			operator+=(const Vector_2d_cartesiano<T> &otro)
	{
		this->x+=otro.x;
		this->y+=otro.y;
		return *this;
	}

	Vector_2d_cartesiano& 			operator-=(const Vector_2d_cartesiano<T> &otro)
	{
		this->x-=otro.x;
		this->y-=otro.y;
		return *this;
	}

	Vector_2d_cartesiano& 			operator*=(const Vector_2d_cartesiano<T> &otro)
	{
		this->x*=otro.x;
		this->y*=otro.y;
		return *this;
	}

	Vector_2d_cartesiano& 			operator/=(const Vector_2d_cartesiano<T> &otro)
	{
		this->x/=otro.x;
		this->y/=otro.y;
		return *this;
	}

	Vector_2d_cartesiano&			operator*(const T p_multiplicador)
	{
		this->x=this->x*p_multiplicador;
		this->y=this->y*p_multiplicador;
		return *this;
	}

	Vector_2d_cartesiano& 			operator/(const T p_divisor)
	{
		this->x=this->x/p_divisor;
		this->y=this->y/p_divisor;
		return *this;
	}

	Vector_2d_cartesiano& 			operator*=(const T p_multiplicador)
	{
		this->x*=p_multiplicador;
		this->y*=p_multiplicador;
		return *this;
	}

	Vector_2d_cartesiano& 			operator/=(const T p_divisor)
	{
		this->x/=p_divisor;
		this->y/=p_divisor;
		return *this;
	}

	Vector_2d_cartesiano<T>			perpendicular() const
	{
		return Vector_2d_cartesiano<T>{this->y, -this->x};
	}
};

//Funciones de cálculo...
template<typename T>
T obtener_angulo_para_vector_unidad_radianes(const Vector_2d<T>& p_vector)
{
	T rad=std::atan2(p_vector.y, p_vector.x);
	return rad;
}

template<typename T>
T obtener_angulo_para_vector_unidad_grados(const Vector_2d<T>& p_vector)
{
	T rad=obtener_angulo_para_vector_unidad_radianes(p_vector);
	T grados=(rad / M_PI) * 180.0;
	return grados;
}

template<typename T>
Vector_2d<T> vector_unidad_para_angulo(T p_angulo)
{
	T rad=DLibH::Herramientas::grados_a_radianes(p_angulo);
	T v_x=sin(rad);
	T v_y=cos(rad);

	Vector_2d<T> r(v_x, v_y);
	return r;
}

template<typename T>
Vector_2d<T> vector_unidad_para_angulo_cartesiano(T p_angulo)
{
	T rad=DLibH::Herramientas::grados_a_radianes(p_angulo);
	T v_x=cos(rad);
	T v_y=sin(rad);

	Vector_2d<T> r(v_x, v_y);
	return r;
}

template<typename T>
T producto_vectorial(const Vector_2d<T>& a, const Vector_2d<T>& b)
{
	return a.x*b.x+a.y*b.y;
}

template<typename T>
T determinante(const Vector_2d<T>& a, const Vector_2d<T>& b)
{
	return (a.x*b.y)-(a.y*b.x);
}

/*El orden de los factores PUEDE alterar el producto.*/

template<typename T>
Vector_2d<T> obtener_para_puntos(T p_xa, T p_ya, T p_xb, T p_yb)
{
	Vector_2d<T> r;

	if(! (p_xa==p_xb && p_ya==p_yb))
	{
		r.x=p_xb-p_xa;
		r.y=p_yb-p_ya;
		r.normalizar();
	}

	return r;
}

template<typename T>
Vector_2d_pantalla<T> obtener_para_puntos_pantalla(T p_xa, T p_ya, T p_xb, T p_yb)
{
	Vector_2d_pantalla<T> r;

	if(! (p_xa==p_xb && p_ya==p_yb))
	{
		r.x=p_xa-p_xb;
		r.y=p_ya-p_yb;
		r.normalizar();
	}

	return r;
}

template<typename T>
Vector_2d_cartesiano<T> obtener_para_puntos_cartesiano(T p_xa, T p_ya, T p_xb, T p_yb, bool normalizar=true)
{
	Vector_2d_cartesiano<T> r;

	if(! (p_xa==p_xb && p_ya==p_yb))
	{
		r.x=p_xb-p_xa;
		r.y=p_yb-p_ya;
		if(normalizar) r.normalizar();
	}

	return r;
}

//Funciones para conversión de cartesiano a pantalla...
template<typename T>
Vector_2d_cartesiano<T>	a_cartesiano(const Vector_2d_pantalla<T>& v)
{
	return Vector_2d_cartesiano<T>(v.x, -v.y);
}

template<typename T>
Vector_2d_pantalla<T> a_pantalla(const Vector_2d_cartesiano<T>& v)
{
	return Vector_2d_pantalla<T>(v.x, -v.y);
}

} //Fin namespace DLibH
#endif

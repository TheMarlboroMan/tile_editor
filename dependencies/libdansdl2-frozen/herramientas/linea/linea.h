#ifndef LINEA_H
#define LINEA_H

#include <cmath>
#include <cstdlib>


namespace DLibH
{

template<typename T>
class Linea
{
	private:

	Linea(){}

	protected:

	bool vertical;
	bool horizontal;
	T m;	//Pendiente.
	T b;	//Constante.

	public:

	struct Punto
	{
		T x;
		T y;
		Punto(T p_x, T p_y):x(p_x), y(p_y)
		{}
	};

	struct Set_puntos
	{
		Punto p1;
		Punto p2;
		bool erroneo;
		Set_puntos(Punto p_1, Punto p_2):p1(p_1), p2(p_2), erroneo(false)
		{}
	};

	~Linea();

	static Linea crear_desde_puntos(T xa, T ya, T xb, T yb)
	{
		Linea resultado=Linea();

		//Es vertical??
		if(xa==xb)
		{
			resultado.vertical=true;
			resultado.horizontal=false;
			resultado.m=0;
			resultado.b=0;
		}
		//Es horizontal
		else if(ya==yb)
		{
			resultado.vertical=false;
			resultado.horizontal=true;
			resultado.m=0;
			resultado.b=0;
		}
		else
		{       
			resultado.vertical=false;
			resultado.horizontal=false;

			//La pendiente: m=(yb-ya)/(xb-xa)...            
			resultado.m=((T)yb-(T)ya)/((T)xb-(T)xa);  

			//La constante b... b=y-(mx)
			resultado.b=ya-(resultado.m*xa);
		}

		return resultado;
	}

	static Linea crear_desde_parametros(T p_m, T p_b)
	{
		Linea resultado=Linea();
		resultado.m=p_m;
		resultado.b=p_b;
		resultado.horizontal=false;	
		resultado.vertical=false;

		if(p_b==0) 
		{
			resultado.horizontal=true;
		}

		return resultado;
	}

	static T distancia_entre_puntos(T xa, T ya, T xb, T yb)
	{
		T x=(xa-xb)*(xa-xb);
		T y=(ya-yb)*(ya-yb);
		return sqrt(x+y);
	}

/*Obtiene una línea metida dentro de una caja recortándo
la existente, si procede... Los cuatro primeros parámetros
son la caja, el resto los puntos... Ojo: sólo funciona
cuando realmente la línea PASA por el rectángulo. 
Realmente no devuelve una línea, sino dos puntos en un
*/

	static Set_puntos obtener_para_linea_y_caja_limite(T x, T y, T w, T h, T px1, T py1, T px2, T py2)
	{
		Linea linea=crear_desde_puntos(px1, py1, px2, py2);
		return obtener_para_linea_y_caja_limite(x, y, w, h, linea);	
	}

	static Set_puntos obtener_para_linea_y_caja_limite(T x, T y, T w, T h, Linea linea)
	{
		T t;
		
		//En primer lugar, buscamos los puntos en los que la línea corta
		//con cada "lado". Los ejes serán 1, 2, 3 y 4 como 
		//arriba, derecha, abajo e izquierda.
	
		//La línea que va por arriba... Buscamos la x para la y enviada.
		t=linea.obtener_x_para_y(y);
		Punto p1=Punto(t, y);

		//La línea de la derecha... Buscamos la y para una x.
		t=linea.obtener_y_para_x(x+w);
		Punto p2=Punto(x+w, t);	

		//La línea de abajo: buscamos la x para una y.
		t=linea.obtener_x_para_y(y+h);
		Punto p3=Punto(t, y);

		//La línea de la izquierda...
		t=linea.obtener_y_para_x(x);
		Punto p4=Punto(x, t);

		//De estos cuatro puntos ahora vemos cuales estarían "dentro"
		//del rectángulo. Serán aquellos en los que al menos una coordenada
		//coincida...

		Set_puntos resultado=Set_puntos(Punto(0,0), Punto(0,0));

		if(p1.x >= x && p1.y <= x+w)
		{
			resultado.p1=p1;
		}
		else if(p3.x >= x && p3.y <= x+w)
		{
			resultado.p1=p3;
		}
		else
		{
			resultado.erroneo=true;
		}

		if(p2.y >= y && p2.y <= y+h)
		{
			resultado.p2=p2;
		}
		else if(p4.y >= y && p4.y <= y+h)
		{
			resultado.p2=p4;
		}
		else
		{
			resultado.erroneo=true;
		}

		return resultado;
	}

	bool es_vertical() const {return this->vertical;}
	bool es_horizontal() const {return this->horizontal;}
	T obtener_y_para_x(T p_x) const
	{
		//y=mx+b;
		T resultado;

		if(this->vertical)
		{
			resultado=0;
		}
		else if(this->horizontal)
		{
			resultado=this->b;
		}
		else
		{
			if(p_x==0) 
			{
				resultado=this->b;
			}
			else
			{
				resultado=(this->m*p_x) + this->b;
			}
		}

		return resultado;
	}

	T obtener_x_para_y(T p_y) const
	{
	//x=(y-b)/m
		T resultado;

		if(this->horizontal)
		{
			resultado=0;
		}
		else if(this->vertical)
		{
			resultado=this->b;	//Quien sabe...
		}
		else
		{
			resultado=(p_y-this->b) / this->m;
		}

		return resultado;
	}

	T acc_m() const {return this->m;}	
	T acc_b() const {return this->b;}
};

}
#endif

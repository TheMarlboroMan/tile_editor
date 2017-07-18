#include <iostream>
#include "../def_herramientas.h"

typedef Poligono_2d<float> Poligono;
typedef Poligono_2d_vertices<float> vPoligono;

template<typename T>
void tell_vertex(const Punto_2d<T>& p)
{
	std::cout<<"["<<p.x<<", "<<p.y<<"] ";
}

template<typename T>
void tell_segment(const Segmento_2d<T>& s)
{
	tell_vertex(s.v1);
	tell_vertex(s.v2);
	std::cout<<"("<<s.direccion.x<<","<<s.direccion.y<<") ";
}

template<typename T>
void tell_polygon(const Poligono_2d<T> p)
{
//	tell_vertex(p.acc_centro());
//	std::cout<<"Vertices: "<<std::endl;
	for(auto &v : p.acc_vertices()) tell_vertex(v); 
	std::cout<<std::endl;

//	std::cout<<" --- SEGMENTOS ---"<<std::endl;
//	for(auto &s : p.acc_segmentos()) tell_segment(s);
//	std::cout<<std::endl;
}

template<typename T>
void tell_polygon(const Poligono_2d_vertices<T> p)
{
//	tell_vertex(p.acc_centro());
//	std::cout<<"Vertices: "<<std::endl;
	for(auto &v : p.acc_vertices()) tell_vertex(v); 
	std::cout<<std::endl;
}

void test_rotation()
{
/*
	std::cout<<" *** PROBANDO ROTACION ***"<<std::endl;

	vPoligono pv( {{1.0, 1.0}, {1.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}}, {0.5, 0.5});
	pv.rotar(-45.0);
	std::cout<<" ### POLIGONO VERTICES ROTADO ###"<<std::endl;
	tell_polygon(pv);

	Poligono p( {{1.0, 1.0}, {1.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}}, {0.5, 0.5});
	p.rotar(-45.0);
	std::cout<<" ### POLIGONO ROTADO ###"<<std::endl;
	tell_polygon(p);
*/
}

int main(int argc, char ** argv)
{
	using namespace DLibH;
	test_rotation();


	std::cout<<" *** PROBANDO POLIGONOS P1: AUTOMÁTICO ***"<<std::endl;
	Poligono polig1( {{0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}, {0.0, 0.0}}, {0.5, 0.5});
//	tell_polygon(polig1);
/*
	std::cout<<" *** PROBANDO POLIGONOS P2: MANUAL ***"<<std::endl;

	Poligono polig2;
	polig2.insertar_vertice({1.0, 0.0});
	polig2.insertar_vertice({1.0, 1.0});
	polig2.insertar_vertice({1.0, 0.0});
	polig2.insertar_vertice({0.0, 0.0});
	polig2.cerrar();
	tell_polygon(polig2);

	std::cout<<" *** PROBANDO COLISION P1 vs P2 ***"<<std::endl;
	std::cout<<colision_poligono_SAT(polig1, polig2)<<std::endl;
*/
	std::cout<<" *** CREANDO POLIGONO P3 ***"<<std::endl;
	Poligono polig3( {{2.0, 1.0}, {2.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}}, {1.5, 0.5});
	tell_polygon(polig3);

	std::cout<<" *** COMPROBANDO P1 vs P3 ***"<<std::endl;
	std::cout<<colision_poligono_SAT(polig1, polig3)<<std::endl;

	std::cout<<" *** ROTANDO P3 POR 45 ***"<<std::endl;
	polig3.rotar(45.0);
	tell_polygon(polig3);

	std::cout<<" *** COMPROBANDO P1 vs P3 ***"<<std::endl;
	std::cout<<colision_poligono_SAT(polig1, polig3)<<std::endl;

	return 1;

}

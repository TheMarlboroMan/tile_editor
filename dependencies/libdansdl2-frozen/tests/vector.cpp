#include <iostream>
#include "../def_herramientas.h"

template<typename T>
void tell_me(const Vector_2d<T>& v)
{
	std::cout<<v.x<<","<<v.y<<std::endl;
}

int main(int argc, char ** argv)
{
	using namespace DLibH;

	Vector_2d<double> v{0.0, 1.0};
	tell_me(v);
	auto normal=v.perpendicular();
	tell_me(normal);

	Vector_2d_cartesiano<double> vc{0.0, 1.0};
	tell_me(vc);
	auto normalc=vc.perpendicular();
	tell_me(normalc);

	Vector_2d_pantalla<double> vp{0.0, 1.0};
	tell_me(vp);
	auto normalp=vp.perpendicular();
	tell_me(normalp);

	auto derivando=a_cartesiano(normalp);
	tell_me(derivando);

	return 1;

}

#include "localizador.h"

Herramientas_proyecto::t_lista_nombres_archivo Localizador::obtener_lista_archivos()
{
	Herramientas_proyecto::t_lista_nombres_archivo resultado;

	resultado.push_back(ruta);

	return resultado;
}

void Localizador::inicializar(unsigned short int p_idioma)
{		
	Herramientas_proyecto::Localizador_base::cambiar_idioma(p_idioma);
}

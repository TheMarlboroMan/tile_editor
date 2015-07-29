#ifndef RECURSOS_PROYECTO_H
#define RECURSOS_PROYECTO_H

class Recursos_graficos
{
	private:

	Recursos_graficos();

	public:

	enum superficies{
		RS_FUENTE_BASE=1
	};

	enum texturas{
		RT_DEFECTO=1
	};
};

class Recursos_audio
{
	private:

	Recursos_audio();

	public:

	enum musicas{
		RM_DEFECTO=1
	};

	enum sonidos{
		RS_DEFECTO=1
	};
};

#endif

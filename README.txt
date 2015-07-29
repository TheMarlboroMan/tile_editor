Esta herramienta depende de la libdansdl2 en
	https://github.com/TheMarlboroMan/libdansdl2

El generador de hojas de sprites está en
	https://github.com/TheMarlboroMan/tabla_sprites

Se incluyen ejemplos de los archivos de configuración en el directorio ejemplo_config.

Uso de la aplicación: ejecutable res=WxH out=fichero_salida cfg=fichero_configuracion
	./a.out res=800x600 out=salida.dat cfg=config.dat

PARÁMETROS:
	res: resolución de pantalla expresada en ancho x alto
	out: nombre del fichero a abrir y que se usará para guardar
	cfg: fichero de configuración que consta de N lineas con el formato siguiente:

		T	nombre_hoja_sprites	caracter comentario	|	ruta recurso gráfico	transparencia	r transparencia	g transparencia	b transparencia
		O	ruta definición objetos

	La T o O iniciales discrimian si es tipo Tile o Objeto.

	La hoja de sprites se genera con el programa editor_sprites. 

	El formato de los objetos es:
		id	nombre	ancho	alto	color_defecto
			nombre_propiedad	valor_defecto
			nombre_propiedad	valor_defecto
			nombre_propiedad	valor_defecto
		id	nombre	ancho	alto	color_defecto
			nombre_propiedad	valor_defecto
		[...]

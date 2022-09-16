#regla : dependencias
#	acciones  'La tabulación es necesaria'

ARGS = "2"

a.out: main.cpp image.png
	#La @ sirve para omitir información a la hora de ejecutar el make
	@echo "Compilando el proyecto..."
	g++ main.cpp -Dcimg_use_png -lpng -lz -lX11 -lpthread -ljpeg

run: ./a.out
	@echo "Ejecutando el proyecto..."
	./a.out $(ARGS)

zip: Makefile main.cpp image.png documentacion.pdf README.md
	zip practica2.zip Makefile main.cpp image.png documentacion.pdf README.md

#Hay que hacer que elimine las fotos reescaladas
clean:
	@echo "Limpiando directorio de trabajo..."
	rm a.out change.png practica2.zip

#define cimg_use_jpeg
#include "CImg.h"
#include <iostream>
#include <assert.h>
#include <ctime>

using namespace cimg_library; //Contiene todas las clases y funciones de la librería
using namespace std;


//Función que explica el funcionamiento del programa desde la terminal de comandos
void usage() {
    cout << "./a.out [scale factor]" << endl;
}

double interpolacionCubica (double p[4], double x) {
	return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
}

double interpolacionBicubica (double p[4][4], double x, double y) {
	double arr[4];
	arr[0] = interpolacionCubica(p[0], y);
	arr[1] = interpolacionCubica(p[1], y);
	arr[2] = interpolacionCubica(p[2], y);
	arr[3] = interpolacionCubica(p[3], y);
	return interpolacionCubica(arr, x);
}

void buscarValores(int scaleFactor, CImg<unsigned char> src, double colorMatrix[4][4], int cordX, int cordY, char color){
    //Pasamos el char al valor correspondiente para la funcion src(x,i, 0, RGB)
    int rgb = -1;

    switch(color){
        case 'R': rgb = 0;
        break;

        case 'G': rgb = 1;
        break;

        case 'B': rgb = 2;
        break;

        default: 
            cout << "buscarValores() mal referenciada"<< endl;
            exit(0);
    }


    //Bucle para rellenar la matriz
    for(int x = 0; x < 4; x++){
        for(int y = 0; y < 4; y++){
            int pixelXOriginal;
            int pixelYOriginal;

            //Pixeles adyacentes
            if(cordX/scaleFactor - 1 <= 0)
                pixelXOriginal = 0;
            else pixelXOriginal = cordX/scaleFactor - 1 ;

            if(cordY/scaleFactor - 1 <= 0)
                pixelYOriginal = 0;
            else pixelYOriginal = cordY/scaleFactor - 1 ;

            pixelXOriginal++;
            pixelYOriginal++;
            
            colorMatrix[x][y] = (double)src(pixelXOriginal, pixelYOriginal, 0, rgb); 
        }
    }
}

int main(int argc, char* argv[])
{
    unsigned tiempoInicio = clock();

    //Uno de los argumentos de entrada debe de ser el factor de escalado
    //que deseamos aplicarle a la imagen

    if(argc != 2) {
        usage();
        exit(0);
    }

    CImg<unsigned char> src("image.png"); //Imagen origen
    
    int scaleFactor = atoi(argv[1]); //Factor por el cual queremos escalar la imagen 
    int width = src.width(); //Anchura de la imagen original
    int height = src.height(); //Altura de la imagen original
    
    //Imagen destino
    CImg<unsigned char> dst(width*scaleFactor, height*scaleFactor, 1, 3, 255); 

    //Reescalamos la imágen conservando los pixeles iniciales
    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
            dst(x * scaleFactor, y * scaleFactor, 0, 0) = src(x, y, 0, 0);
            dst(x * scaleFactor, y * scaleFactor, 0, 1) = src(x, y, 0, 1);
            dst(x * scaleFactor, y * scaleFactor, 0, 2) = src(x, y, 0, 2);
        }    
    }
    

    //Recorremos pixel a pixel la imágen dst
    for(int i = 0; i < width*scaleFactor; i++){
        for(int j = 0; j < height*scaleFactor; j++){
            //Si el pixel no es original...
            if(i % scaleFactor != 0 || j % scaleFactor != 0){
                /*Debemos hallar los 4x4 pixeles originales mas cercanos. Como no siempre tenemos tanta información
                  por ahora solo vamos a interpolar el centro de la imagen y no los bordes
                */
                double colorMatrix[4][4];

                if(i != 0 && i != 1 && j != 0 && j != 1){
                    
                    buscarValores(scaleFactor, src, colorMatrix, i, j, 'R');
                    //Otorgamos a cada color el resultado de interpolar cada una de las matrices 4x4
                    dst(i, j, 0, 0) = interpolacionBicubica(colorMatrix, i, j); //Interpolamos el valor R

                    buscarValores(scaleFactor, src, colorMatrix, i, j, 'G');
                    dst(i, j, 0, 1) = interpolacionBicubica(colorMatrix, i, j); //Interpolamos el valor G
                    
                    buscarValores(scaleFactor, src, colorMatrix, i, j, 'B');
                    dst(i, j, 0, 2) = interpolacionBicubica(colorMatrix, i, j); //Interpolamos el valor B   
                    
                }             
            }
        }
    }
    
    //Creamos una ventana para mostrar la imagen resultante
    //CImgDisplay disp1(width*scaleFactor, height*scaleFactor, "Imagen resultante");
    //disp1.display(dst);    //Mostramos la imagen

    dst.save("change.png");//Guardamos el fichero

    unsigned tiempoFin = clock();

    cout << "Tiempo requerido en el procesamiento de la imágen: " << (double (tiempoFin-tiempoInicio)/CLOCKS_PER_SEC) << " s." << endl;

    return 0;
}



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


#define cimg_use_jpeg
#include "CImg.h"
#include <iostream>
#include <ctime>

using namespace cimg_library;
using namespace std;

void print_usage() {
    cout << "./a.out [scale factor]" << endl;
}

double cubicInterpolate(double p[4], double x) {
    return p[1] + 0.5 * x * (p[2] - p[0] + x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] + x * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
}

double bicubicInterpolate(double p[4][4], double x, double y) {
    double arr[4];
    arr[0] = cubicInterpolate(p[0], y);
    arr[1] = cubicInterpolate(p[1], y);
    arr[2] = cubicInterpolate(p[2], y);
    arr[3] = cubicInterpolate(p[3], y);
    return cubicInterpolate(arr, x);
}

void getNeighborhood(int scaleFactor, CImg<unsigned char> src, double colorMatrix[4][4], int coordX, int coordY, int channel) {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            int pixelX = coordX / scaleFactor - 1 + x;
            int pixelY = coordY / scaleFactor - 1 + y;

            // Mirror edge handling
            if (pixelX < 0) pixelX = -pixelX;
            if (pixelX >= src.width()) pixelX = 2 * src.width() - pixelX - 2;
            if (pixelY < 0) pixelY = -pixelY;
            if (pixelY >= src.height()) pixelY = 2 * src.height() - pixelY - 2;

            pixelX = std::max(0, std::min(src.width() - 1, pixelX));
            pixelY = std::max(0, std::min(src.height() - 1, pixelY));

            colorMatrix[x][y] = (double)src(pixelX, pixelY, 0, channel);
        }
    }
}

int main(int argc, char* argv[]) {
    unsigned startTime = clock();

    if (argc != 2) {
        print_usage();
        exit(0);
    }

    CImg<unsigned char> src("input-image.png");
    int scaleFactor = atoi(argv[1]);
    int width = src.width();
    int height = src.height();
    int channels = src.spectrum();

    CImg<unsigned char> dst(width * scaleFactor, height * scaleFactor, 1, channels, 0);

    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
            for (int c = 0; c < channels; c++) {
                dst(x * scaleFactor, y * scaleFactor, 0, c) = src(x, y, 0, c);
            }
        }
    }

    for (int i = 0; i < width * scaleFactor; i++) {
        for (int j = 0; j < height * scaleFactor; j++) {
            if (i % scaleFactor != 0 || j % scaleFactor != 0) {
                double colorMatrix[4][4];
                double fracX = (double)(i % scaleFactor) / (double)scaleFactor;
                double fracY = (double)(j % scaleFactor) / (double)scaleFactor;
                for (int c = 0; c < channels; c++) {
                    getNeighborhood(scaleFactor, src, colorMatrix, i, j, c);
                    double interpolated = bicubicInterpolate(colorMatrix, fracX, fracY);
                    interpolated = std::max(0.0, std::min(255.0, interpolated));
                    dst(i, j, 0, c) = (unsigned char)interpolated;
                }
            }
        }
    }

    dst.save("output-image.png");

    unsigned endTime = clock();
    cout << "Processing time: " << (double(endTime - startTime) / CLOCKS_PER_SEC) << " s." << endl;
    return 0;
}
         
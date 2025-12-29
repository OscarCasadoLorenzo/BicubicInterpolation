#define cimg_use_jpeg
#include "CImg.h"
#include <iostream>
#include <ctime>
#include <algorithm>

using namespace cimg_library;
using namespace std;

// Constants
const int NEIGHBORHOOD_SIZE = 4;
const int RGB_CHANNELS = 3;
const int RGBA_CHANNELS = 4;
const int RED_CHANNEL = 0;
const int GREEN_CHANNEL = 1;
const int BLUE_CHANNEL = 2;
const int ALPHA_CHANNEL = 3;
const char* DEFAULT_INPUT_IMAGE = "image.png";
const char* DEFAULT_OUTPUT_IMAGE = "change.png";

// Channel identifiers
enum Channel {
    RED = 'R',
    GREEN = 'G',
    BLUE = 'B',
    ALPHA = 'A'
};

void printUsage() {
    cout << "Usage: ./bicubic [scale_factor]" << endl;
    cout << "  scale_factor: Integer value to scale the image (e.g., 2, 3, 5)" << endl;
}

int getChannelIndex(char channelId) {
    switch(channelId) {
        case RED:   return RED_CHANNEL;
        case GREEN: return GREEN_CHANNEL;
        case BLUE:  return BLUE_CHANNEL;
        case ALPHA: return ALPHA_CHANNEL;
        default:
            cerr << "Error: Invalid channel identifier '" << channelId << "'" << endl;
            exit(1);
    }
}

/**
 * Performs 1D cubic interpolation using Catmull-Rom spline
 * @param points Array of 4 surrounding pixel values
 * @param t Position between points[1] and points[2] (0.0 to 1.0)
 * @return Interpolated value
 */
double cubicInterpolate(const double points[4], double t) {
    return points[1] + 0.5 * t * (points[2] - points[0] + 
           t * (2.0 * points[0] - 5.0 * points[1] + 4.0 * points[2] - points[3] + 
           t * (3.0 * (points[1] - points[2]) + points[3] - points[0])));
}

/**
 * Performs 2D bicubic interpolation on a 4x4 grid of pixels
 * @param grid 4x4 matrix of pixel values
 * @param x Horizontal interpolation position
 * @param y Vertical interpolation position
 * @return Interpolated pixel value
 */
double bicubicInterpolate(const double grid[4][4], double x, double y) {
    double interpolatedRows[4];
    
    for(int i = 0; i < NEIGHBORHOOD_SIZE; i++) {
        interpolatedRows[i] = cubicInterpolate(grid[i], y);
    }
    
    return cubicInterpolate(interpolatedRows, x);
}

/**
 * Extracts a 4x4 neighborhood of pixel values around a target position
 * @param sourceImage Source image
 * @param scaleFactor Scale factor being applied
 * @param targetX Target X coordinate in scaled image
 * @param targetY Target Y coordinate in scaled image
 * @param channelId Channel identifier (R, G, B, or A)
 * @param outputMatrix Output 4x4 matrix to fill with pixel values
 */
void extractNeighborhood(const CImg<unsigned char>& sourceImage, 
                         int scaleFactor,
                         int targetX, 
                         int targetY, 
                         char channelId,
                         double outputMatrix[4][4]) {
    
    int channelIndex = getChannelIndex(channelId);
    int sourceWidth = sourceImage.width();
    int sourceHeight = sourceImage.height();
    
    // Calculate base position in source image
    int baseX = max(0, targetX / scaleFactor - 1);
    int baseY = max(0, targetY / scaleFactor - 1);
    
    // Extract 4x4 neighborhood, clamping at image boundaries
    for(int row = 0; row < NEIGHBORHOOD_SIZE; row++) {
        for(int col = 0; col < NEIGHBORHOOD_SIZE; col++) {
            int srcX = min(baseX + row, sourceWidth - 1);
            int srcY = min(baseY + col, sourceHeight - 1);
            outputMatrix[row][col] = static_cast<double>(sourceImage(srcX, srcY, 0, channelIndex));
        }
    }
}

/**
 * Copies original pixels from source to destination at scaled positions
 */
void copyOriginalPixels(const CImg<unsigned char>& sourceImage,
                        CImg<unsigned char>& destinationImage,
                        int scaleFactor,
                        int numChannels) {
    int width = sourceImage.width();
    int height = sourceImage.height();
    
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            int destX = x * scaleFactor;
            int destY = y * scaleFactor;
            
            // Copy RGB channels
            for(int channel = 0; channel < RGB_CHANNELS; channel++) {
                destinationImage(destX, destY, 0, channel) = sourceImage(x, y, 0, channel);
            }
            
            // Copy alpha channel if present
            if(numChannels == RGBA_CHANNELS) {
                destinationImage(destX, destY, 0, ALPHA_CHANNEL) = sourceImage(x, y, 0, ALPHA_CHANNEL);
            }
        }
    }
}

/**
 * Checks if a pixel position is an original (uninterpolated) pixel
 */
bool isOriginalPixel(int x, int y, int scaleFactor) {
    return (x % scaleFactor == 0) && (y % scaleFactor == 0);
}

/**
 * Checks if pixel is in border region where interpolation should be skipped
 */
bool isInBorderRegion(int x, int y) {
    // Skip first two rows/columns to ensure we have complete 4x4 neighborhoods
    return (x <= 1 || y <= 1);
}

/**
 * Interpolates a single channel value for a pixel
 */
double interpolateChannel(const CImg<unsigned char>& sourceImage,
                          int scaleFactor,
                          int x, 
                          int y,
                          char channelId) {
    double neighborhoodMatrix[4][4];
    extractNeighborhood(sourceImage, scaleFactor, x, y, channelId, neighborhoodMatrix);
    
    // Calculate fractional position within the cell (0.0 to 1.0)
    double fractionalX = static_cast<double>(x % scaleFactor) / scaleFactor;
    double fractionalY = static_cast<double>(y % scaleFactor) / scaleFactor;
    
    return bicubicInterpolate(neighborhoodMatrix, fractionalX, fractionalY);
}

/**
 * Fills interpolated pixels in the destination image
 */
void interpolatePixels(const CImg<unsigned char>& sourceImage,
                       CImg<unsigned char>& destinationImage,
                       int scaleFactor,
                       int numChannels) {
    int scaledWidth = destinationImage.width();
    int scaledHeight = destinationImage.height();
    
    for(int x = 0; x < scaledWidth; x++) {
        for(int y = 0; y < scaledHeight; y++) {
            // Skip original pixels
            if(isOriginalPixel(x, y, scaleFactor)) {
                continue;
            }
            
            // Skip border regions to avoid incomplete neighborhoods
            if(isInBorderRegion(x, y)) {
                continue;
            }
            
            // Interpolate RGB channels
            destinationImage(x, y, 0, RED_CHANNEL) = interpolateChannel(sourceImage, scaleFactor, x, y, RED);
            destinationImage(x, y, 0, GREEN_CHANNEL) = interpolateChannel(sourceImage, scaleFactor, x, y, GREEN);
            destinationImage(x, y, 0, BLUE_CHANNEL) = interpolateChannel(sourceImage, scaleFactor, x, y, BLUE);
            
            // Interpolate alpha channel if present
            if(numChannels == RGBA_CHANNELS) {
                destinationImage(x, y, 0, ALPHA_CHANNEL) = interpolateChannel(sourceImage, scaleFactor, x, y, ALPHA);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    clock_t startTime = clock();
    
    // Validate command-line arguments
    if(argc != 2) {
        printUsage();
        return 1;
    }
    
    int scaleFactor = atoi(argv[1]);
    if(scaleFactor <= 0) {
        cerr << "Error: Scale factor must be a positive integer" << endl;
        return 1;
    }
    
    // Load source image
    CImg<unsigned char> sourceImage(DEFAULT_INPUT_IMAGE);
    
    int width = sourceImage.width();
    int height = sourceImage.height();
    int numChannels = sourceImage.spectrum();
    
    cout << "Processing image: " << width << "x" << height 
         << " (" << numChannels << " channels)" << endl;
    cout << "Scale factor: " << scaleFactor << "x" << endl;
    
    // Create destination image with scaled dimensions
    int scaledWidth = width * scaleFactor;
    int scaledHeight = height * scaleFactor;
    CImg<unsigned char> destinationImage(scaledWidth, scaledHeight, 1, numChannels, 0);
    
    // Initialize background: transparent for RGBA, white for RGB
    if(numChannels == RGB_CHANNELS) {
        destinationImage.fill(255);
    }
    
    // Copy original pixels to scaled positions
    copyOriginalPixels(sourceImage, destinationImage, scaleFactor, numChannels);
    
    // Interpolate missing pixels using bicubic interpolation
    interpolatePixels(sourceImage, destinationImage, scaleFactor, numChannels);
    
    // Save result
    destinationImage.save(DEFAULT_OUTPUT_IMAGE);
    
    clock_t endTime = clock();
    double elapsedSeconds = static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC;
    
    cout << "Output saved to: " << DEFAULT_OUTPUT_IMAGE << endl;
    cout << "Processing time: " << elapsedSeconds << " seconds" << endl;
    
    return 0;
}



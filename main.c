//To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
//To run (linux/mac): ./main.out example.bmp example_inv.bmp

//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

// Function to invert pixels of an image (negative)
void invert(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGTH; y++) {
      for (int c = 0; c < BMP_CHANNELS; c++) {
        output_image[x][y][c] = 255 - input_image[x][y][c];
      }
    }
  }
}


void grayscale(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS],unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGTH; y++) {
      for (int c = 0; c < BMP_CHANNELS; c++) {
        output_image[x][y][c] = (input_image[x][y][0]+input_image[x][y][1]+input_image[x][y][2])/3;
      }
    }
  }
}

void binary_colour(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS],unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGTH; y++) {
      if (input_image[x][y][0] > 90){
        for(int i=0; i< BMP_CHANNELS; i++){
          binary_image[x][y][i] = 255;
        }
      } else {
        for(int j=0; j< BMP_CHANNELS; j++){
          binary_image[x][y][j] = 0;
        }
        
      }
    }
  }
}



// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char gray_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];

// Main function
int main(int argc, char** argv) {
  // Checking that 2 arguments are passed
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <input file path> <output file path>\n", argv[0]);
    exit(1);
  }

  printf("Example program - 02132 - A1\n");

  // Load image from file
  read_bitmap(argv[1], input_image);

  // Run inversion
  // invert(input_image, output_image);

  // grayscale(input_image, gray_image);

  grayscale(input_image, gray_image);

  binary_colour(gray_image, binary_image);

  // Save image to file
  write_bitmap(binary_image, argv[2]);



  printf("Done!\n");
  return 0;
}


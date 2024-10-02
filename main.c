//To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
//To run (linux/mac): ./main.out example.bmp example_inv.bmp

//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"
#include <time.h>


void detect_cells(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2][BMP_CHANNELS], int *pCell_count);
void check_square(int i, int j, unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2][BMP_CHANNELS], int *pCell_count);
void check_exclusion(int i, int j, unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2][BMP_CHANNELS], int *pCell_count);
void cell_detected(int i, int j, unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2][BMP_CHANNELS], int *pCell_count);
void draw_cell(int i, int j, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);


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


int check_pixel(int cor1, int cor2, unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2][BMP_CHANNELS]){
  if(new_image[cor1][cor2+1][0] == 0 || new_image[cor1][cor2-1][0] == 0 || new_image[cor1+1][cor2][0] == 0 || new_image[cor1-1][cor2][0] == 0){
    return 0;
  } else{return 255;}
}


void add_layer(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2][BMP_CHANNELS]){
  for(int i=0; i<BMP_WIDTH+2; i++){
    for(int j=0; j<BMP_HEIGTH+2; j++){
      for(int k=0; k< BMP_CHANNELS; k++){
          new_image[i][j][k] = 0;
        }
    }
  }
  for(int i=1; i<BMP_WIDTH+1; i++){
    for(int j=1; j<BMP_HEIGTH+1; j++){
      for(int k=0; k< BMP_CHANNELS; k++){
        new_image[i][j][k] = binary_image[i-1][j-1][k];
      }
    }
  }
}

void erode(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2][BMP_CHANNELS]){
  add_layer(binary_image, new_image);

  for(int i=1; i<BMP_WIDTH+1; i++){
    for(int j=1; j<BMP_HEIGTH+1; j++){
      if(new_image[i][j][0] == 255){
        for(int k=0; k< BMP_CHANNELS; k++){
          binary_image[i-1][j-1][k] = check_pixel(i, j, new_image);
        }
      }
    }
  }
}


void detect_cells(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2][BMP_CHANNELS], int *pCell_count){
  for(int i=1; i<BMP_WIDTH-11; i++){
    for(int j=1; j<BMP_HEIGTH-11; j++){
      check_square(i, j, input_image, new_image, pCell_count);
    }
  }
}

void check_square(int i, int j, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2][BMP_CHANNELS], int *pCell_count){
  for(int x = i; x<i+11; x++){
    for(int y = j; y<j+11; y++){
      if(new_image[x][y][0] == 255){
        check_exclusion(x, y, input_image, new_image, pCell_count);
        return;
      }
    }
  }
}


void check_exclusion(int x, int y, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2][BMP_CHANNELS], int *pCell_count){
  
  for(int i = x-1; i<x+12; i++){
    if(new_image[i][y-1][0] == 255){return;}
  }

  for(int i = x-1; i<x+12; i++){
    if(new_image[i][y+12][0] == 255){return;}
  }

  for(int j = y-1; j<y+12; j++){
    if(new_image[x-1][j][0] == 255){return;}
  }

  for(int j = y-1; j<y+12; j++){
    if(new_image[x+12][j][0] == 255){return;}
  }
  
  cell_detected(x, y, input_image, new_image, pCell_count);
}


void cell_detected(int i, int j, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2][BMP_CHANNELS], int *cell_count) {
  *cell_count += 1;
  printf("%d, %d\n", i, j);
  
  for (int x = i; x < i + 11; x++) {
    for (int y = j; y < j + 11; y++) {
        new_image[x][y][0] = 0;
        new_image[x][y][1] = 0;
        new_image[x][y][2] = 0;
    }
  }

  draw_cell(i, j, input_image);
}

void draw_cell(int i, int j, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
  for(int x=i+1; x<i+9; x++){
    for(int y=j+1; y<j+9; y++){
      input_image[x-1][y-1][0] = 255;
      input_image[x-1][y-1][1] = 0;
      input_image[x-1][y-1][2] = 0;
    }
  }
}



// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char gray_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2][BMP_CHANNELS];
//unsigned char *pNew_image = &new_image;
int cell_count = 0;
int *pCell_count = &cell_count;

// Main function
int main(int argc, char** argv) {
  // Checking that 2 arguments are passed
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <input file path> <output file path>\n", argv[0]);
    exit(1);
  }

  clock_t t1 = clock();

  printf("Example program - 02132 - A1\n");

  // Load image from file
  read_bitmap(argv[1], input_image);

  // Run inversion
  // invert(input_image, output_image);

  // grayscale(input_image, gray_image);

  grayscale(input_image, gray_image);

  binary_colour(gray_image, binary_image);

  // for(int i=0; i<15; i++){
  //   erode(binary_image,new_image);
  //   detect_cells(input_image, new_image, pCell_count);
  //   //printf("%d \n",cell_count);
  // }
  

  // Save image to file
  write_bitmap(binary_image, argv[2]);


  printf("Done!\n%d cells detected.\n", cell_count);

  clock_t t2 = clock();
  double time = ((double) (t2-t1))/CLOCKS_PER_SEC;
  printf("%lf ", time);

  return 0;
}


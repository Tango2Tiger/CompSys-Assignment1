//To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
//To run (linux/mac): ./main.out example.bmp example_inv.bmp

//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"
#include <time.h>
#include <unistd.h>
#include <math.h>

#define MAX_INTENSITY 256

void gethist(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], int histo[MAX_INTENSITY]){
  for(int k = 0; k < MAX_INTENSITY; k++){
    for(int i=0; i<BMP_WIDTH; i++){
      for(int j=0; j<BMP_HEIGTH; j++){
        if(binary_image[i][j] == k){
          histo[k]++;
        }
      }
    }
  }
}



int otsu_threshold(int histogram[MAX_INTENSITY], int total_pixels) {
    int sumB = 0; 
    int wB = 0;
    int wF = 0; 
    int total_sum = 0;
    float max_variance = 0.0;
    int threshold = 0;
    
    float sum = 0.0;
    for (int t = 0; t < MAX_INTENSITY; t++) {
        sum += t * histogram[t]; 
    }
    
    
    for (int t = 0; t < MAX_INTENSITY; t++) {
        wB += histogram[t];  
        if (wB == 0) continue; 
        
        wF = total_pixels - wB; 
        if (wF == 0) break;
        
        sumB += t * histogram[t]; 
        
        float mB = (float) sumB / wB; 
        float mF = (float) (sum - sumB) / wF; 
        
        float variance = (float) wB * wF * (mB - mF) * (mB - mF);
        
        if (variance > max_variance) {
          max_variance = variance;
          threshold = t;
        }
    }
    
    return threshold;
}



int histo[MAX_INTENSITY];


void draw_cell(int i, int j, unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
  for(int x=i; x<i+8; x++){
    for(int y=j; y<j+8; y++){
      input_image[x-1][y-1][0] = 255;
      input_image[x-1][y-1][1] = 0;
      input_image[x-1][y-1][2] = 0;
    }
  }
}


void cell_detected(int i, int j, unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2], int *cell_count) {
  *cell_count += 1;
  printf("%d, %d \n", i, j);
  
  for (int x = i; x < i + 12; x++) {
    for (int y = j; y < j + 12; y++) {
        binary_image[x-1][y-1] = 0;
        new_image[x][y] = 0;
    }
  }

  draw_cell(i, j, input_image);
}



void check_exclusion(int i, int j, unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2], int *cell_count){
  
  for(int x = i-1; x<i+9; x++){
    if(new_image[x][j-1] == 255) return;
  }

  for(int x = i-1; x<i+9; x++){
    if(new_image[x][j+9] == 255) return;
  }

  for(int y = j-1; y<j+9; y++){
    if(new_image[i-1][y] == 255) return;
  }

  for(int y = j-1; y<j+9; y++){
    if(new_image[i+9][y] == 255) return;
  }
  
  cell_detected(i, j, binary_image, input_image, new_image, cell_count);
}



void check_square(int i, int j, unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2], int *cell_count){
  for(int x = i; x<i+8; x++){
    for(int y = j; y<j+8; y++){
      if(new_image[x][y] == 255){
        check_exclusion(i, j, binary_image, input_image, new_image, cell_count);
      }
    }
  }
}



void detect_cells(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2], int *cell_count){
  for(int i=1; i<BMP_WIDTH-8; i++){
    for(int j=1; j<BMP_HEIGTH-8; j++){
      check_square(i, j, binary_image, input_image, new_image, cell_count);
    }
  }
}



void grayscale(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS],unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH]){
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGTH; y++) {
        binary_image[x][y] = (input_image[x][y][0]+input_image[x][y][1]+input_image[x][y][2])/3;
    }
  }
}



void binary_colour(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], int optimal_threshold){
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGTH; y++) {
      if (binary_image[x][y] > optimal_threshold){
          binary_image[x][y] = 255;
      } else {
          binary_image[x][y] = 0;
      }
    }
  }
}



int check_pixel(int cor1, int cor2, unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2]){
  if(new_image[cor1][cor2+1] == 0 || new_image[cor1][cor2-1] == 0 || new_image[cor1+1][cor2] == 0 || new_image[cor1-1][cor2] == 0){
    return 0;
  } else{return 255;}
}



void add_layer(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2]){
  for(int i=0; i<BMP_WIDTH+2; i++){
    for(int j=0; j<BMP_HEIGTH+2; j++){
          new_image[i][j] = 0;
    }
  }
  for(int i=1; i<BMP_WIDTH+1; i++){
    for(int j=1; j<BMP_HEIGTH+1; j++){
        new_image[i][j] = binary_image[i-1][j-1];
    }
  }
}



void erode(unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH], unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2]){
  add_layer(binary_image, new_image);

  for(int i=1; i<BMP_WIDTH+1; i++){
    for(int j=1; j<BMP_HEIGTH+1; j++){
      if(new_image[i][j] == 255){
          binary_image[i-1][j-1] = check_pixel(i, j, new_image);
      }
    }
  }
}



unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char new_image[BMP_WIDTH+2][BMP_HEIGTH+2];
int cell_count = 0;


int main(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <input file path> <output file path>\n", argv[0]);
    exit(1);
  }

  clock_t c1 = clock();

  printf("Example program - 02132 - A1\n");

  read_bitmap(argv[1], input_image);

  grayscale(input_image, binary_image);
  int total_pixels = BMP_HEIGTH*BMP_WIDTH;

  gethist(binary_image,histo);
  int optimal_threshold = otsu_threshold(histo, total_pixels);

  printf("%d ",optimal_threshold);
  binary_colour(binary_image, optimal_threshold);

  for(int i=0; i<30; i++){
    erode(binary_image, new_image);
    detect_cells(binary_image, input_image, new_image, &cell_count);
    
    // sleep(1);
  }
  write_bitmap(input_image, argv[2]);

  printf("Done!\n%d cells detected.\n", cell_count);

  clock_t c2 = clock();
  double time = ((double) (c2-c1))/CLOCKS_PER_SEC;
  printf("%lf ", time);

  return 0;
}


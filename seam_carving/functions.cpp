#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

void initializeImage(Pixel image[][MAX_HEIGHT]) {
  // iterate through columns
  for (unsigned int col = 0; col < MAX_WIDTH; col++) {
    // iterate through rows
    for (unsigned int row = 0; row < MAX_HEIGHT; row++) {
      // initialize pixel
      image[col][row] = {0, 0, 0};
    }
  }
}

void loadImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int& width, unsigned int& height) {
  // TODO: implement (part 1)
  string fileType;
  double widthCheck = 0;
  double heightCheck = 0;
  int color = 0;
  int maxColor = 255;
  unsigned int count = 0;
  std::ifstream fileFS;
  fileFS.open(filename);
  fileFS >> fileType;
  if (!fileFS.is_open()){
    string error = "Failed to open " + filename;
    throw std::runtime_error(error);
  }else if (fileType != "P3" && fileType != "p3"){
    string error = "Invalid type " + fileType;
    throw std::runtime_error(error);
  }
  fileFS >> widthCheck;
  fileFS >> heightCheck;
  width = static_cast<int>(widthCheck);
  height = static_cast<int>(heightCheck);
  if (widthCheck != width || heightCheck != height){
    string error = "Invalid dimensions";
    throw std::runtime_error(error);
  }else if ((0>=width)||(width>MAX_WIDTH)||(0>=height)||(height>MAX_HEIGHT)){
    string error = "Invalid dimensions";
    throw std::runtime_error(error);
  }
  fileFS >> color;
  if (color != maxColor){
    string error = "Invalid max color value";
    throw std::runtime_error(error);
  }
  string value1;
	string value2;
	string value3;
	string finalCheck;
	unsigned int row = 0;
	unsigned int column = 0;
	while(!fileFS.eof()){
		fileFS >> value1;
    //cout << value1 << " ";
		if (std::stoi(value1)<0||std::stoi(value1)>255){
        	string error = "Invalid color value";
        	throw std::runtime_error(error);
      }else if (fileFS.fail()){
		    break;
	  }

    ++count;
	  fileFS >> value2;
    //cout << value2 << " ";
		if (std::stoi(value2)<0||std::stoi(value2)>255){
        	string error = "Invalid color value";
        	throw std::runtime_error(error);
      }else if (fileFS.fail()){
		    break;
	  }
    ++count;
	  fileFS >> value3;
    //cout << value3 << " ";
		if (std::stoi(value3)<0||std::stoi(value3)>255){
        	string error = "Invalid color value";
        	throw std::runtime_error(error);
      }else if (fileFS.fail()){
		    break;
	  }
    ++count;
    Pixel pixel;
    pixel.r = std::stoi(value1);
    pixel.g = std::stoi(value2);
    pixel.b = std::stoi(value3);
	  image[row][column] = pixel;
	  ++row;
	  if ((row % (width)) == 0){
		row = 0;
		++column;
	  }
	  if (column==(height)){
		break;
	  }
	}
	fileFS >> finalCheck;
  if (finalCheck != ""){
    string error = "Too many values";
    throw std::runtime_error(error);
  }
	if (count > width*height*3){
		string error = "Too many values";
    throw std::runtime_error(error);
	}
  if (count < width*height*3){
		string error = "Not enough values";
    throw std::runtime_error(error);
	}
  fileFS.close();
}

void outputImage(string filename, Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height) {
  // TODO: implement (part 1)
  std::ofstream outFS;
  outFS.open(filename);
  if (!outFS.is_open()){
    string error = "Failed to open " + filename;
    throw std::runtime_error(error);
  }
  outFS << "P3" << endl;
  outFS << width << " " << height << endl;
  outFS << "255" << endl;
  for (unsigned int c=0;c<height;++c){
    for (unsigned int r=0;r<width;++r){
      outFS << image[r][c].r << " ";
      outFS << image[r][c].g << " ";
      outFS << image[r][c].b << endl;
    }
  }
  outFS.close();
}

unsigned int energy(Pixel image[][MAX_HEIGHT], unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
  // TODO: implement (part 1)
  int left;
  int right;
  int up;
  int down;
  int energy = 0;
  //for (unsigned int c=0;c<height;++c){
  //  for (unsigned int r=0;r<width;++r){
      //Check for border left and right
      if (x == 0){
        left = width-1;
        right = 1;
      }else if (x == width-1){
        left = width - 2;
        right = 0;
      }else {
        left = x - 1;
        right = x + 1;
      }
      if (y == 0){
        up = height-1;
        down = 1;
      }else if (y == height-1){
        up = height - 2;
        down = 0;
      }else {
        up = y - 1;
        down = y + 1;
      }
      int redSide = image[right][y].r-image[left][y].r;
      int greenSide = image[right][y].g-image[left][y].g;
      int blueSide = image[right][y].b-image[left][y].b;
      int energySide = (redSide*redSide)+(greenSide*greenSide)+(blueSide*blueSide);
      int redVert = image[x][down].r-image[x][up].r;
      int greenVert = image[x][down].g-image[x][up].g;
      int blueVert = image[x][down].b-image[x][up].b;
      int energyVert = (redVert*redVert)+(greenVert*greenVert)+(blueVert*blueVert);
      energy = energySide + energyVert;
  //  }
  //}
  return energy;
}

// uncomment functions as you implement them (part 2)

unsigned int loadVerticalSeam(Pixel image[][MAX_HEIGHT], unsigned int start_col, unsigned int width, unsigned int height, unsigned int seam[]) {
  // TODO: implement (part 2)
  unsigned int r;
  unsigned int c = start_col;
  int totalEnergy = 0;
  unsigned int energyL = 0;
  unsigned int energyM = 0;
  unsigned int energyR = 0;
  for (r=0;r<height;r=r+1){
    totalEnergy = totalEnergy + energy(image,c,r,width,height);
    seam[r]=c;
    if (r==height-1)
      break;
    if (c==0){
      energyM = energy(image,c,r+1,width,height);
      energyL = energy(image,c+1,r+1,width,height);
      if (energyL < energyM){
        c = c + 1;
      }
      continue;
    }
    if (c==width-1){
      energyM = energy(image,c,r+1,width,height);
      energyR = energy(image,c-1,r+1,width,height);
      if (energyR < energyM){
        c = c - 1;
      }
      continue;
    }
    energyR = energy(image,c-1,r+1,width,height);
    energyM = energy(image,c,r+1,width,height);
    energyL = energy(image,c+1,r+1,width,height);
    if ((energyM<=energyR) && (energyM<=energyL)){
      continue;
    }else if ((energyL<energyM) && (energyL<=energyR)){
      c = c + 1;
      continue;
    }else if ((energyR<energyM) && (energyR<energyL)){
      c = c - 1;
      continue;
    }
  }
   return totalEnergy;
 }

unsigned int loadHorizontalSeam(Pixel image[][MAX_HEIGHT], unsigned int start_row, unsigned int width, unsigned int height, unsigned int seam[]) {
  // TODO: implement (part 2)
  unsigned int r = start_row;
  unsigned int c;
  int totalEnergy = 0;
  unsigned int energyL = 0;
  unsigned int energyM = 0;
  unsigned int energyR = 0;
  for (c=0;c<width;c++){
    totalEnergy = totalEnergy + energy(image,c,r,width,height);
    seam[c]=r;
    if (c==width-1)
      break;
    if (r==0){
      energyM = energy(image,c+1,r,width,height);
      energyR = energy(image,c+1,r+1,width,height);
      if (energyR < energyM){
        r = r + 1;
      }
      continue;
    }
    if (r==height-1){
      energyM = energy(image,c+1,r,width,height);
      energyL = energy(image,c+1,r-1,width,height);
      if (energyL < energyM){
        r = r - 1;
      }
      continue;
    }
    energyL = energy(image,c+1,r-1,width,height);
    energyM = energy(image,c+1,r,width,height);
    energyR = energy(image,c+1,r+1,width,height);
    if ((energyM<=energyL) && (energyM<=energyR)){
      continue;
    }else if ((energyL<energyM) && (energyL<=energyR)){
      r = r - 1;
      continue;
    }else if ((energyR<energyM) && (energyR<energyL)){
      r = r + 1;
      continue;
    }
  }
  return totalEnergy;
}

void findMinVerticalSeam(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height, unsigned int seam[]) {
  // TODO: implement (part 2)
  unsigned int totalEnergy = 0;
  unsigned int minEnergy = loadVerticalSeam(image,0,width,height,seam);
  unsigned int start_col = 0;
  for (unsigned int c=1;c<width;c++){
    totalEnergy = loadVerticalSeam(image,c,width,height,seam);
    if (totalEnergy<minEnergy){
      minEnergy = totalEnergy;
      start_col = c;
    }
  }
  loadVerticalSeam(image,start_col,width,height,seam);
}

void findMinHorizontalSeam(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int height, unsigned int seam[]) {
  // TODO: implement (part 2)
  unsigned int totalEnergy = 0;
  unsigned int minEnergy = loadHorizontalSeam(image,0,width,height,seam);
  unsigned int start_row = 0;
  for (unsigned int r=1;r<height;r++){
    totalEnergy = loadHorizontalSeam(image,r,width,height,seam);
    if (totalEnergy<minEnergy){
      minEnergy = totalEnergy;
      start_row = r;
    }
  }
  loadHorizontalSeam(image,start_row,width,height,seam);
}

void removeVerticalSeam(Pixel image[][MAX_HEIGHT], unsigned int& width, unsigned int height, unsigned int verticalSeam[]) {
  // TODO: implement (part 2)
  for (unsigned int r=0;r<height;r=r+1){
    int col_index = verticalSeam[r];
    for (unsigned int i=col_index;i<width-1;i++){
      image[i][r] = image[i+1][r];
    }
  }
  width = width-1;
}

void removeHorizontalSeam(Pixel image[][MAX_HEIGHT], unsigned int width, unsigned int& height, unsigned int horizontalSeam[]) {
  // TODO: implement (part 2)
  for (unsigned int c=0;c<width;c++){
    int row_index = horizontalSeam[c];
    for (unsigned int i=row_index;i<height-1;i++){
      image[c][i] = image[c][i+1];
    }
  }
  height = height-1;
}

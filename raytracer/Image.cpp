#include "Image.hpp"
#include "Math.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

namespace rt {
  
  Image::Image() : mWidth(0), mHeight(0)
  {
  }
  
  Image::Image(size_t width, size_t height)
  {
	assert(width>0 && height>0);
	this->init(width,height);
  }
  
  void Image::init(size_t width, size_t height)
  {
	mWidth=width;
	mHeight=height;
	mData.clear();
	mData.resize(width*height);
  }
  
  typedef struct
  {
    unsigned char imageTypeCode;
    short int imageWidth;
    short int imageHeight;
    unsigned char bitCount;
    unsigned char *imageData;
  } TGAFILE;
  
  bool LoadTGAFile(const char *filename, TGAFILE *tgaFile)
  {
    FILE *filePtr;
    unsigned char ucharBad;
    short int sintBad;
    long imageSize;
    int colorMode;
    unsigned char colorSwap;
	
    // Open the TGA file.
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
    {
	  return false;
    }
	
    // Read the two first bytes we don't need.
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
	
    // Which type of image gets stored in imageTypeCode.
    fread(&tgaFile->imageTypeCode, sizeof(unsigned char), 1, filePtr);
	
    // For our purposes, the type code should be 2 (uncompressed RGB image)
    // or 3 (uncompressed black-and-white images).
    if (tgaFile->imageTypeCode != 2 && tgaFile->imageTypeCode != 3)
    {
	  fclose(filePtr);
	  return false;
    }
	
    // Read 13 bytes of data we don't need.
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
	
    // Read the image's width and height.
    fread(&tgaFile->imageWidth, sizeof(short int), 1, filePtr);
    fread(&tgaFile->imageHeight, sizeof(short int), 1, filePtr);
	
    // Read the bit depth.
    fread(&tgaFile->bitCount, sizeof(unsigned char), 1, filePtr);
	
    // Read one byte of data we don't need.
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
	
    // Color mode -> 3 = BGR, 4 = BGRA.
    colorMode = tgaFile->bitCount / 8;
    imageSize = tgaFile->imageWidth * tgaFile->imageHeight * colorMode;
	
    // Allocate memory for the image data.
    tgaFile->imageData = (unsigned char*)malloc(sizeof(unsigned char)*imageSize);
	
    // Read the image data.
    fread(tgaFile->imageData, sizeof(unsigned char), imageSize, filePtr);
	
    // Change from BGR to RGB so OpenGL can read the image data.
    for (int imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
    {
	  colorSwap = tgaFile->imageData[imageIdx];
	  tgaFile->imageData[imageIdx] = tgaFile->imageData[imageIdx + 2];
	  tgaFile->imageData[imageIdx + 2] = colorSwap;
    }
	
    fclose(filePtr);
    return true;
  }
  
  Image::Image(std::string filename)
  {
	TGAFILE	file;
	LoadTGAFile(filename.c_str(), &file);
	mWidth = file.imageWidth;
	mHeight = file.imageHeight;
	unsigned int position = 0;
	for (unsigned int y = 0; y < mHeight; y++)
	  for (unsigned int x = 0; x < mWidth; x++) {
		Vec4 pixel;
		pixel[0] = file.imageData[position++] / 255.;
		pixel[1] = file.imageData[position++] / 255.;
		pixel[2] = file.imageData[position++] / 255.;
		pixel[3] = file.imageData[position++] / 255.;
		mData.push_back(pixel);
	  }
	
  }
  
  bool Image::saveToTGA(std::string fileName) const
  {
	//add .tga file extension if not yet present
	std::string suffix(".tga"), outName(fileName), outNameLower(fileName);
	
	std::transform(outNameLower.begin(),
				   outNameLower.end(),
				   outNameLower.begin(),
				   ::tolower);
	
	if (!std::equal(suffix.rbegin(),
					suffix.rend(),
					outNameLower.rbegin()))
	  outName+=suffix;
	
	std::ofstream f(outName, std::ios::binary | std::ios::out);
	
	if (!f.is_open())
	{
	  std::cerr<<"Image::saveToBitmap: could not open file." <<
	  outName <<std::endl;
	  return false;
	}
	
	if (!mData.size())
	{
	  std::cerr<<"Image::saveToBitmap: image data uninitialized." << std::endl;
	  return false;
	}
	
	if (mWidth<1 || mHeight<1)
	{
	  std::cerr << "Image::saveToBitmap: image dimensions too small to write ("
	  << mWidth << "," << mHeight << ")" << std::endl;
	  return false;
	}
	
	//Write the header
	f.put(0);
	f.put(0);
	f.put(2);                       /* uncompressed RGB */
	f.put(0); 	f.put(0);
	f.put(0); 	f.put(0);
	f.put(0);
	f.put(0); 	f.put(0);           /* X origin */
	f.put(0); 	f.put(0);           /* y origin */
	f.put((mWidth & 0x00FF));
	f.put((unsigned char)((mWidth & 0xFF00) / 256));
	f.put((unsigned char)(mHeight & 0x00FF));
	f.put((unsigned char)((mHeight & 0xFF00) / 256));
	f.put(32);                      /* 32 bit RGBA bitmap */
	f.put(0);
	
	//Write the pixel data in BGRA
	for (size_t i=0; i<mHeight*mWidth; i++)
	{
	  f.put((unsigned char)(Math::clamp(mData[i](2))*255));
	  f.put((unsigned char)(Math::clamp(mData[i](1))*255));
	  f.put((unsigned char)(Math::clamp(mData[i](0))*255));
	  f.put((unsigned char)(Math::clamp(mData[i](3))*255));
	}
	
	f.close();
	
	return true;
  }
  
} //namespace rt

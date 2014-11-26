//
//  ImageShapeObject.cpp
//  basicExample
//
//  Created by Philipp Schoessler on 4/2/14.
//
//

#include "ImageShapeObject.h"

ImageShapeObject::ImageShapeObject(){};

void ImageShapeObject::setup(string path)
{
    filePath = path;
    listAllImages();
    allPixels = new unsigned char[RELIEF_SIZE];
}

//----------------------------------------------------

void ImageShapeObject::listAllImages() {
    
    dir.listDir(filePath); // @todo move to constant
    dir.sort();
    
    // if directory is not empty, assign memory in vector
    // @todo check for dot files
    
    int newSize = (int)dir.size();
    
    if(newSize > 0) {
        images.assign(newSize, ofImage());
        
        // load each movie and set to first frame for display
        for(int i = 0; i < newSize; i++) {
            
            images.resize(newSize);
            loadedImagesPaths.resize(newSize);
            loadedImagesFilenames.resize(newSize);
            
            cout << "== Loading new Image " << dir.getPath(i) << endl;
            images[i].loadImage(dir.getPath(i));
            loadedImagesPaths[i] = dir.getPath(i);
            loadedImagesFilenames[i] = dir.getName(i);
        }
    }
}



void ImageShapeObject::update(float dt)
{
    
}

//----------------------------------------------------

void ImageShapeObject::renderShape()
{
    if(nowDrawing.bAllocated()) nowDrawing.draw(0,0, RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y);
}

//----------------------------------------------------

void ImageShapeObject::renderGraphics(int x, int y, int w, int h)
{
    if(nowDrawing.bAllocated()) nowDrawing.draw(0,0);
}

//----------------------------------------------------

vector<string> ImageShapeObject::getLoadedImagesPaths()
{
    return loadedImagesPaths;
}

//----------------------------------------------------

vector <string> ImageShapeObject::getLoadedImagesFilenames()
{
    return loadedImagesFilenames;
}

//----------------------------------------------------

void ImageShapeObject::drawGuiScreen(int x, int y, int w, int h)
{
    
}

//----------------------------------------------------

void ImageShapeObject::drawImageByPath(string path)
{
    for (int i = 0; i < loadedImagesPaths.size(); i++) {
        if(path == loadedImagesPaths[i])
        {
            nowDrawing = images[i];
            nowDrawing.draw(0, 0, RELIEF_PROJECTOR_SIZE_X, RELIEF_PROJECTOR_SIZE_Y);
            cout << " now drawing image by path " << path;
        }
    }
}

//----------------------------------------------------

void ImageShapeObject::drawImageByFileName(string filename)
{
    filename = filePath + "/" + filename; // prefix this folder path
    drawImageByPath(filename);
}

//----------------------------------------------------

void ImageShapeObject::setTableValuesForShape(ShapeIOManager *pIOManager)
{
    
}

//----------------------------------------------------

unsigned char* ImageShapeObject::getPixels()
{
    return allPixels;
}


//
//  ImageShapeObject.h
//  basicExample
//
//  Created by Philipp Schoessler on 4/2/14.
//
//

#ifndef __basicExample__ImageShapeObject__
#define __basicExample__ImageShapeObject__

#include <iostream>
#include "ShapeObject.h"
#include "Constants.h"

class ImageShapeObject : public ShapeObject {
public:
    ImageShapeObject();
    void setup(string path);
    void update(float dt);
    void renderShape();
    void renderGraphics(int x, int y, int w, int h);
    vector <string> getLoadedImagesPaths();
    vector <string> getLoadedImagesFilenames();
    void drawGuiScreen(int x, int y, int w, int h);
    void drawImageByPath(string path);
    void drawImageByFileName(string filename);
    void setTableValuesForShape(ShapeIOManager *pIOManager);
    unsigned char* getPixels();
    
    string get_shape_name() {return shape_name; };
    string shape_name = "Image";

    
private:
    ofImage nowDrawing;
    string filePath;
    ofDirectory dir;
    vector<ofImage> images;
    vector<string> loadedImagesPaths;
    vector<string> loadedImagesFilenames;
    void listAllImages();
    unsigned char* allPixels;
};

#endif /* defined(__basicExample__ImageShapeObject__) */

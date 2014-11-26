//
//  CalmShapeObject.h
//  basicExample
//
//  Created by Daniel Leithinger on 3/20/14.
//
//

#ifndef __basicExample__CalmShapeObject__
#define __basicExample__CalmShapeObject__

#include "ShapeObject.h"
#include <iostream>

class CalmShapeObject : public ShapeObject {
public:
    CalmShapeObject();
    string get_shape_name() {return shape_name; };
    
    void setTableValuesForShape(ShapeIOManager *pIOManager);

private:
    unsigned char* getPixels();
    unsigned char* allPixels;
    string shape_name = "Calm";

};

#endif /* defined(__basicExample__CalmShapeObject__) */

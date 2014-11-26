//
//  CalmShapeObject.cpp
//  basicExample
//
//  Created by Tangible Media Group on 3/20/14.
//
//

#include "CalmShapeObject.h"

CalmShapeObject::CalmShapeObject(){
    allPixels = new unsigned char[RELIEF_PHYSICAL_SIZE_X * RELIEF_PHYSICAL_SIZE_Y];
};

void CalmShapeObject::setTableValuesForShape(ShapeIOManager *pIOManager) {
    pIOManager->set_max_speed(0);
    pIOManager->set_gain_p(0.0f);
    pIOManager->set_gain_i(0.0f);
    pIOManager->set_max_i(0);
    pIOManager->set_deadzone(0);
};

unsigned char* CalmShapeObject::getPixels()
{
    for(int i = 0; i < RELIEF_PHYSICAL_SIZE_X * RELIEF_PHYSICAL_SIZE_Y; i++)
        allPixels[i] = 0;
    return allPixels;
}
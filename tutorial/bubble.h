//
//  bubble.h
//  tutorial
//
//  Created by 安田洋介 on 5/31/15.
//  Copyright (c) 2015 UT-PSI. All rights reserved.
//

#ifndef __tutorial__bubble__
#define __tutorial__bubble__

#include "shader.h"
#include <stdlib.h>

class Bubble{
public:
    GLfloat center[3];
    GLfloat color[4];
    GLfloat ambient_color[4] = {1.0, 1.0, 1.0, 0.8};
    GLfloat specular_color[4] = {1.0, 1.0, 1.0, 0.8};
    GLfloat shiness[1] = {100.0};
    GLfloat velocity[3];
    GLfloat dt = 0.001;
    GLfloat m = 3;
    GLfloat size;
    Bubble();
    void set_params();
    Bubble* proceed();
    Bubble* draw();
    Bubble* reset();
};

#endif /* defined(__tutorial__bubble__) */

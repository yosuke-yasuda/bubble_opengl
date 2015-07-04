//
//  bubble.cpp
//  tutorial
//
//  Created by 安田洋介 on 5/31/15.
//  Copyright (c) 2015 UT-PSI. All rights reserved.
//

#include "bubble.h"
#include <iostream>

using namespace std;
Bubble::Bubble(){
    Bubble::set_params();
}
void Bubble::set_params(){
    center[0] = ((rand()%101)-50)/10.0;
    center[1] = ((rand()%101)-50)/10.0;
    center[2] = ((rand()%51)-50)/10.0;
    velocity[0] = 0.0;
    velocity[1] = 10.0;
    velocity[2] = 0.0;
    size = 0.005;
}
Bubble* Bubble::proceed(){
    int rand_digit = 1001;
    if(this->center[1] > 10){
        this->reset();
    }
    this->velocity[0] += ((rand_digit-1)/2.0-(rand()%rand_digit))/m/1000.0;
    this->velocity[1] += this->size*this->size*this->size*1000;
    this->velocity[2] += ((rand_digit-1)/2.0-(rand()%rand_digit))/m/1000.0;
    this->center[0] += velocity[0] * dt;
    this->center[1] += velocity[1] * dt;
    this->center[2] += velocity[2] * dt;
    this->size += 0.0001;
    return this;
}
Bubble* Bubble::draw(){
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMatrixMode(GL_PROJECTION);
    glMatrixMode(GL_MODELVIEW);
    
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_DIFFUSE, difuse_color);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_color);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
    glTranslatef(this->center[0], this->center[1], this->center[2]);
    //glColor4f(1.0, 1.0, 1.0, 0.2);
    glutSolidSphere(this->size, 16, 16);
    glPopMatrix();
    return this;
}
Bubble* Bubble::reset(){
    set_params();
    return this;
}
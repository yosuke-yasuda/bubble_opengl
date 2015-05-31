//
//  shader.h
//  tutorial
//
//  Created by 安田洋介 on 5/31/15.
//  Copyright (c) 2015 UT-PSI. All rights reserved.
//

#ifndef __tutorial__shader__
#define __tutorial__shader__

#include <stdio.h>
#include <GLUT/glut.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#if defined(WIN32)
#  pragma warning(disable: 4996)
#  include <malloc.h>
#  include "glew.h"
#  include "glut.h"
#  include "glext.h"
#elif defined(__APPLE__) || defined(MACOSX)
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/glut.h>
#endif

void setShaders();
int readShaderSource(GLuint shader, const char *file);
void printShaderInfoLog(GLuint shader);
void printProgramInfoLog(GLuint program);

#endif /* defined(__tutorial__shader__) */

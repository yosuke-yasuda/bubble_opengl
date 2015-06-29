//
//  shader.cpp
//  tutorial
//
//  Created by 安田洋介 on 5/31/15.
//  Copyright (c) 2015 UT-PSI. All rights reserved.
//

#include "shader.h"
#include "iostream"

GLuint v, f, p;

char textFileRead(std::string str){
    char ch;
    std::fstream fin(str, std::fstream::in);
    while (fin >> std::noskipws >> ch) {
    }
    return ch;
}

int readShaderSource(GLuint shader, const char *file)
{
    FILE *fp;
    const GLchar *source;
    GLsizei length;
    int ret;
    
    /* ファイルを開く */
    fp = fopen(file, "rb");
    if (fp == NULL) {
        perror(file);
        return -1;
    }
    
    /* ファイルの末尾に移動し現在位置 (つまりファイルサイズ) を得る */
    fseek(fp, 0L, SEEK_END);
    length = ftell(fp);
    
    /* ファイルサイズのメモリを確保 */
    source = (GLchar *)malloc(length);
    if (source == NULL) {
        fprintf(stderr, "Could not allocate read buffer.\n");
        return -1;
    }
    
    /* ファイルを先頭から読み込む */
    fseek(fp, 0L, SEEK_SET);
    ret = fread((void *)source, 1, length, fp) != (size_t)length;
    fclose(fp);
    
    /* シェーダのソースプログラムのシェーダオブジェクトへの読み込み */
    if (ret)
        fprintf(stderr, "Could not read file: %s.\n", file);
    else
        glShaderSource(shader, 1, &source, &length);
    
    /* 確保したメモリの開放 */
    free((void *)source);
    
    return ret;
}

/*
 ** シェーダの情報を表示する
 */
void printShaderInfoLog(GLuint shader)
{
    GLsizei bufSize;
    
    /* シェーダのコンパイル時のログの長さを取得する */
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &bufSize);
    
    if (bufSize > 1) {
        GLchar *infoLog = (GLchar *)malloc(bufSize);
        
        if (infoLog != NULL) {
            GLsizei length;
            
            /* シェーダのコンパイル時のログの内容を取得する */
            glGetShaderInfoLog(shader, bufSize, &length, infoLog);
            fprintf(stderr, "InfoLog:\n%s\n\n", infoLog);
            free(infoLog);
        }
        else
            fprintf(stderr, "Could not allocate InfoLog buffer.\n");
    }
}
/*
 ** プログラムの情報を表示する
 */
void printProgramInfoLog(GLuint program)
{
    GLsizei bufSize;
    
    /* シェーダのリンク時のログの長さを取得する */
    glGetProgramiv(program, GL_INFO_LOG_LENGTH , &bufSize);
    
    if (bufSize > 1) {
        GLchar *infoLog = (GLchar *)malloc(bufSize);
        
        if (infoLog != NULL) {
            GLsizei length;
            
            /* シェーダのリンク時のログの内容を取得する */
            glGetProgramInfoLog(program, bufSize, &length, infoLog);
            fprintf(stderr, "InfoLog:\n%s\n\n", infoLog);
            free(infoLog);
        }
        else
            fprintf(stderr, "Could not allocate InfoLog buffer.\n");
    }
}

void setShaders() {
    
    char vs,fs;
    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
    
    vs = textFileRead("vert.vert");
    fs = textFileRead("frag.frag");
    
    const char * vv = &vs;
    const char * ff = &fs;
    //std::cout<<"check shader0"<<std::endl;
    glShaderSource(v, 1, &vv,NULL);
    glShaderSource(f, 1, &ff,NULL);
    //std::cout<<"check shader"<<std::endl;
    glCompileShader(v);
    glCompileShader(f);
    //std::cout<<"check shader1"<<std::endl;
    p = glCreateProgram();
    
    glAttachShader(p,v);
    glAttachShader(p,f);
    //std::cout<<"check shader2"<<std::endl;
    glLinkProgram(p);
    glUseProgram(p);
}
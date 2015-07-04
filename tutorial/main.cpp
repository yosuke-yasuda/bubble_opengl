#include "bubble.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

/*
 ** シェーダオブジェクト
 */
static GLuint vertShader;
static GLuint fragShader;
static GLuint gl2Program;

const char *filename[]={"image/inverse.jpg","image/inverse.jpg","image/bottom.jpg","image/top.jpg","image/inverse.jpg","image/inverse.jpg"};

unsigned int cubefaces[6] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

unsigned int texId;

void LoadTexture()
{
    //6枚を１つのテクスチャとして扱ってくれる
    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_CUBE_MAP, texId );
    
    //フィルタ設定
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//下地の色にマッピング
    
    
    cv::Mat imgA;//OpenCVの新しいC++インターフェイス
    for(int loop = 0;loop < 6;++loop)
    {
        imgA = cv::imread(filename[loop],CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
        if(imgA.empty())
        {
            std::cerr << filename[loop] << " : Can't Load Image\n";
            exit(EXIT_FAILURE);
        }
        
        
        glTexImage2D(cubefaces[loop],0,GL_RGB8,imgA.cols,imgA.rows, 0,GL_BGR,GL_UNSIGNED_BYTE,imgA.data);
        std::cout <<  filename[loop] << " : Loaded .\n";

        imgA.release();
    }
    
}

unsigned int backTexId;

void LoadBackTexture(){
    glGenTextures( 1, &backTexId );
    glBindTexture( GL_TEXTURE_2D, backTexId );
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    cv::Mat imgA;
    imgA = cv::imread("image/wall.jpg",CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
    if(imgA.empty())
    {
        std::cerr << "image/inverse.jpg" << " : Can't Load Image\n";
        exit(EXIT_FAILURE);
    }
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB8,imgA.cols,imgA.rows, 0,GL_BGR,GL_UNSIGNED_BYTE,imgA.data);
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 28, 28, GL_RGB, GL_UNSIGNED_BYTE, imgA.data);
    std::cout <<  "back texture" << " : Loaded .\n";
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    imgA.release();
}


GLint compiled, linked;

GLfloat red[] = { 0.8, 0.2, 0.2, 1.0 };
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat aquablue[] = {66.0/255, 74.0/255, 118.0/255};
GLfloat white[] = {1.0, 1.0, 1.0};
int bubble_num;

Bubble bubbles[100];

GLfloat light0pos[] = { 0.0, 10, 0.0, 1.0 };

void idle(void)
{
    glutPostRedisplay();
}

void display(void)
{
    glEnable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    /* 光源の位置設定 */
    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
    static GLfloat ambient [] = { aquablue[0], aquablue[1], aquablue[2], 0.1f};
    static GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
    static GLfloat diffuse [] = { 1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    
    /* 材質の設定 */
    static const GLfloat ambient_color[] = { 1.0, 1.0, 1.0, 1.0 };  /* 材質 (色) */
    static const GLfloat diffuse_color[] = { 1.0, 1.0, 1.0, 1.0};  /* 材質 (色) */
    static const GLfloat specular_color[] = { 1.0, 1.0, 1.0, 1.0 };  /* 材質 (色) */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_color);
    //glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backTexId);
    /*glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_GEN_Q);*/
    /* １枚の４角形を描く */
    //glNormal3d(0.0, 0.0, -10.0);
    glBegin(GL_QUADS);
    /*glTexCoord2d(0.0, 1.0);
     glVertex3d(-1000.0, -1000.0,  -200.0);
     glTexCoord2d(1.0, 1.0);
     glVertex3d( 1000.0, -1000.0,  -200.0);
     glTexCoord2d(1.0, 0.0);
     glVertex3d( 1000.0,  1000.0,  -200.0);
     glTexCoord2d(0.0, 0.0);
     glVertex3d(-1000.0,  1000.0,  -200.0);*/
    glTexCoord2d(1.0, 1.0);
    glVertex3d(-15.0, -10.0,  -30.0);
    glTexCoord2d(1.0, 0.0);
    
    glVertex3d(-15.0,  10.0,  -30.0);
    glTexCoord2d(0.0, 0.0);
    glVertex3d( 15.0,  10.0,  -30.0);
    glTexCoord2d(0.0, 1.0);
    glVertex3d( 15.0, -10.0,  -30.0);
    glEnd();
    /*glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);*/
    /* テクスチャマッピング終了 */
    glDisable(GL_TEXTURE_2D);
    
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    
    //glUseProgram(gl2Program);
    glActiveTexture(GL_TEXTURE0);
    glEnable( GL_TEXTURE_GEN_S );
    glEnable( GL_TEXTURE_GEN_T );
    glEnable( GL_TEXTURE_GEN_R );
    glEnable( GL_TEXTURE_CUBE_MAP );
    glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
    for(int i=0; i<sizeof(bubbles)/sizeof(bubbles[0]); i++){
        bubbles[i].proceed()->draw();
    }
    glDisable( GL_TEXTURE_GEN_S );
    glDisable( GL_TEXTURE_GEN_T );
    glDisable( GL_TEXTURE_GEN_R );
    glDisable( GL_TEXTURE_CUBE_MAP );
    
    glUseProgram(0);
    
    glPopMatrix();
    
    glutSwapBuffers();
    glutIdleFunc(idle);
}

void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    
    /* 透視変換行列の設定 */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
    
    /* モデルビュー変換行列の設定 */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);
}

void mouse(int button, int state, int x, int y)
{
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':
        case 'Q':
        case '\033':  /* '\033' は ESC の ASCII コード */
            exit(0);
        default:
            break;
    }
}

void init(GLfloat *color)
{
    glClearColor(color[0], color[1], color[2], 1.0);
    
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    resize(640,400);
    bubble_num = sizeof(bubbles)/sizeof(bubbles[0]);
    /*for(int i=0; i<bubble_num; i++){
        bubbles[i] = Bubble();
    }*/
    
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    /* シェーダのソースプログラムの読み込み */
    if (readShaderSource(vertShader, "/Users/yasudayousuke/Documents/study/lecture/7sem/cg/tutorial/tutorial/vert.vert")) exit(1);
    if (readShaderSource(fragShader, "/Users/yasudayousuke/Documents/study/lecture/7sem/cg/tutorial/tutorial/frag.frag")) exit(1);
    
    /* バーテックスシェーダのソースプログラムのコンパイル */
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    printShaderInfoLog(vertShader);
    if (compiled == GL_FALSE) {
        fprintf(stderr, "Compile error in vertex shader.\n");
        exit(1);
    }
    
    /* フラグメントシェーダのソースプログラムのコンパイル */
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    printShaderInfoLog(fragShader);
    if (compiled == GL_FALSE) {
        fprintf(stderr, "Compile error in fragment shader.\n");
        exit(1);
    }
    
    /* プログラムオブジェクトの作成 */
    gl2Program = glCreateProgram();
    
    /* シェーダオブジェクトのシェーダプログラムへの登録 */
    glAttachShader(gl2Program, vertShader);
    glAttachShader(gl2Program, fragShader);
    
    /* シェーダオブジェクトの削除 */
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    
    /* attribute 変数 position の index を 0 に指定する */
    glBindAttribLocation(gl2Program, 0, "position");
    /* シェーダプログラムのリンク */
    glLinkProgram(gl2Program);
    glGetProgramiv(gl2Program, GL_LINK_STATUS, &linked);
    printProgramInfoLog(gl2Program);
    if (linked == GL_FALSE) {
        fprintf(stderr, "Link error.\n");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    glutInitWindowSize(1200, 960);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    LoadTexture();
    LoadBackTexture();
    init(aquablue);
    glutMainLoop();
    return 0;
}
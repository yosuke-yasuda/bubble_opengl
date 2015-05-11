#include <GLUT/glut.h>
#include <stdlib.h>

GLdouble vertex[][3] = {
    { 0.0, 0.0, 0.0 }, /* A */
    { 1.0, 0.0, 0.0 }, /* B */
    { 1.0, 1.0, 0.0 }, /* C */
    { 0.0, 1.0, 0.0 }, /* D */
    { 0.0, 0.0, 1.0 }, /* E */
    { 1.0, 0.0, 1.0 }, /* F */
    { 1.0, 1.0, 1.0 }, /* G */
    { 0.0, 1.0, 1.0 }  /* H */
};

int edge[][2] = {
    { 0, 1 }, /* ア (A-B) */
    { 1, 2 }, /* イ (B-C) */
    { 2, 3 }, /* ウ (C-D) */
    { 3, 0 }, /* エ (D-A) */
    { 4, 5 }, /* オ (E-F) */
    { 5, 6 }, /* カ (F-G) */
    { 6, 7 }, /* キ (G-H) */
    { 7, 4 }, /* ク (H-E) */
    { 0, 4 }, /* ケ (A-E) */
    { 1, 5 }, /* コ (B-F) */
    { 2, 6 }, /* サ (C-G) */
    { 3, 7 }  /* シ (D-H) */
};

int face[][4] = {
    { 0, 1, 2, 3 },
    { 1, 5, 6, 2 },
    { 5, 4, 7, 6 },
    { 4, 0, 3, 7 },
    { 4, 5, 1, 0 },
    { 3, 2, 6, 7 }
};

GLdouble color[][3] = {
    { 1.0, 0.0, 0.0 }, /* 赤 */
    { 0.0, 1.0, 0.0 }, /* 緑 */
    { 0.0, 0.0, 1.0 }, /* 青 */
    { 1.0, 1.0, 0.0 }, /* 黄 */
    { 1.0, 0.0, 1.0 }, /* マゼンタ */
    { 0.0, 1.0, 1.0 }  /* シアン 　*/
};

GLdouble normal[][3] = {
    { 0.0, 0.0,-1.0 },
    { 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    {-1.0, 0.0, 0.0 },
    { 0.0,-1.0, 0.0 },
    { 0.0, 1.0, 0.0 }
};

GLfloat light0pos[] = { 0.0, 3.0, 5.0, 1.0 };
GLfloat light1pos[] = { 5.0, 3.0, 0.0, 1.0 };

GLfloat red[] = { 0.8, 0.2, 0.2, 1.0 };
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };

void cube(void)
{
    int i;
    int j;
    
    glBegin(GL_QUADS);
    for (j = 0; j < 6; ++j) {
        glNormal3dv(normal[j]);
        for (i = 0; i < 4; ++i) {
            glVertex3dv(vertex[face[j][i]]);
        }
    }
    glEnd();
}

void sphere(void)
{
    int i;
    int j;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
    glutSolidSphere(0.1,16,16);
}

void idle(void)
{
    glutPostRedisplay();
}

void display(void)
{
    static int r = 0; /* 回転角 */
    static float y = 0.0f;
    static int sphere_length = 4;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    /* 光源の位置設定 */
    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
    
    /* モデルビュー変換行列の保存 */
    glPushMatrix();
    
    /* 図形の回転 */
    glRotated((double)r, 0.0, 1.0, 0.0);
    
    /* 図形の色 (赤)  */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    
    /* 図形の描画 */
    cube();
    for (int i=0; i<sphere_length; i++) {
        glTranslated(0.0, y-0.1*i, 0.0);
        sphere();
    }
    
    /* 二つ目の図形の描画 */
    glPushMatrix();
    glTranslated(1.0, 1.0, 1.0);
    glRotated((double)(2 * r), 0.0, 1.0, 0.0);
    cube();
    y+=0.01;
    glPopMatrix();
    
    /* モデルビュー変換行列の復帰 */
    glPopMatrix();
    
    glutSwapBuffers();
    
    /* 一周回ったら回転角を 0 に戻す */
    if (++r >= 360) r = 0;
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
    gluLookAt(3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void mouse(int button, int state, int x, int y)
{
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                /* アニメーション開始 */
                glutIdleFunc(idle);
            }
            else {
                /* アニメーション停止 */
                glutIdleFunc(0);
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN) {
                /* コマ送り (1ステップだけ進める) */
                glutPostRedisplay();
            }
            break;
        default:
            break;
    }
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

void init(void)
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, green);
    glLightfv(GL_LIGHT1, GL_SPECULAR, green);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    init();
    glutMainLoop();
    return 0;
}
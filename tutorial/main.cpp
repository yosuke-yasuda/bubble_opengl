#include "bubble.h"

GLfloat red[] = { 0.8, 0.2, 0.2, 1.0 };
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat aquablue[] = {66.0/255, 74.0/255, 118.0/255};
GLfloat white[] = {1.0, 1.0, 1.0};
int bubble_num;

Bubble bubbles[100];

GLfloat light0pos[] = { 100.0, 100.0, 100.0, 1 };
GLfloat light1pos[] = { -100.0, -100.0, -100.0, 1 };

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
    glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
    
    if(rand()%1000 < bubble_num){
        int index = rand()%bubble_num;
        bubbles[index].reset();
    }
    for(int i=0; i<sizeof(bubbles)/sizeof(bubbles[0]); i++){
        bubbles[i].proceed()->draw();
        std::printf("%f\n", bubbles[i].center[0]);
    }
    
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    
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
    gluLookAt(3.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
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
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    resize(1200,600);
    bubble_num = sizeof(bubbles)/sizeof(bubbles[0]);
    /*for(int i=0; i<bubble_num; i++){
        bubbles[i] = Bubble();
    }*/
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
    init(aquablue);
    glutMainLoop();
    return 0;
}
/*
 * hellogl.cpp
 *
 *  Created on: Apr 03, 2012
 *      Author: martinez
 */

// #include <GL/glew.h>
#include <GLUT/glut.h>

void displayCB() {
  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(1.f, 1.f, 1.f);
  glBegin(GL_POLYGON);
    glVertex3f(0.25f, 0.25f, 0.f);
    glVertex3f(0.75f, 0.25f, 0.f);
    glVertex3f(0.75f, 0.75f, 0.f);
    glVertex3f(0.25f, 0.75f, 0.f);
  glEnd();
  
  glFlush();
}

void init() {
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

int main (int argc, char** argv) {
//  glewInit();

  glutInit              (&argc, argv);
  glutInitDisplayMode   (GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize    (500,500);
  glutInitWindowPosition(100,100);
  glutCreateWindow      ("HelloGL");
  glutDisplayFunc       (displayCB);
  
  init();
  
  glutMainLoop();
  
  return 0;
}

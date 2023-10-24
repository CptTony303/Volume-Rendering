#include <core/renderer.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <iostream>

Renderer::Renderer(GLFWwindow* window) {
	_window = window;
}
void Renderer::renderScene(){
    //std::cout << "rendering scene" << std::endl;
    // Hier kannst du OpenGL-Befehle zum Zeichnen platzieren
    glClear(GL_COLOR_BUFFER_BIT);

    // Beginne das Zeichnen von Dreiecken
    glBegin(GL_TRIANGLES);

    // Erster Punkt (unten links) - Rot
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-1.0f, -1.0f);

    // Zweiter Punkt (oben) - Grün
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(0.0f, 1.0f);

    // Dritter Punkt (unten rechts) - Blau
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(1.0f, -1.0f);

    // Beende das Zeichnen von Dreiecken
    glEnd();
    //glFlush();
}
#include "button.h"
#include "button-big.h"
#include "button-dpad.h"
#include "controller.h"
#include "joystick.h"
#include "shoulder.h"
#include "start.h"
#include "trigger.h"

#include <GL/glu.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MAX_CONTROLLERS 4

typedef struct {
    float color[3];
    float angle_x;
    float angle_y;
} Controller;

static Controller s_controllers[MAX_CONTROLLERS];
static int s_num_controllers = 0;

static void draw_controller(Controller *controller)
{
    glRotatef(controller->angle_x, 0.1, 0.0, 0.0);
    glRotatef(controller->angle_y, 0.0, 1.0, 0.0);
    glColor3fv(controller->color);
    controller_draw_all();

    glColor3f(0.2, 0.1, 0.2);
    float pressed_z = -0.0025;

    /* D-pad */
    glPushMatrix();
    glTranslatef(-0.05, 0.02, 0.02);
    {
        glPushMatrix();
        button_dpad_draw_all();
        glPopMatrix();
    }
    {
        glPushMatrix();
        glRotatef(90, 0.0, 0.0, 1.0);
        button_dpad_draw_all();
        glPopMatrix();
    }
    {
        glPushMatrix();
        glRotatef(180, 0.0, 0.0, 1.0);
        button_dpad_draw_all();
        glPopMatrix();
    }
    {
        glPushMatrix();
        glRotatef(270, 0.0, 0.0, 1.0);
        button_dpad_draw_all();
        glPopMatrix();
    }
    glPopMatrix();

    /* Joysticks */
    {
        glPushMatrix();
        glTranslatef(-0.028, -0.007, -0.015);
        joystick_draw_all();
        glPopMatrix();
    }
    {
        glPushMatrix();
        glTranslatef(0.028, -0.007, -0.015);
        joystick_draw_all();
        glPopMatrix();
    }

    /* Main buttons */
    glPushMatrix();
    glTranslatef(0.05, 0.02, -0.007);
    {
        glPushMatrix();
        button_big_draw_all();
        glPopMatrix();
    }
    {
        glPushMatrix();
        glRotatef(90, 0.0, 0.0, 1.0);
        button_big_draw_all();
        glPopMatrix();
    }
    {
        glPushMatrix();
        glRotatef(180, 0.0, 0.0, 1.0);
        button_big_draw_all();
        glPopMatrix();
    }
    {
        glPushMatrix();
        glRotatef(270, 0.0, 0.0, 1.0);
        button_big_draw_all();
        glPopMatrix();
    }
    glPopMatrix();

    /* Start and back */
    {
        glPushMatrix();
        glTranslatef(0.012, 0.02, -0.0055);
        start_draw_all();
        glPopMatrix();
    }
    {
        glPushMatrix();
        glTranslatef(-0.012, 0.02, -0.0055);
        glRotatef(180, 0.0, 0.0, 1.0);
        start_draw_all();
        glPopMatrix();
    }

    /* Guide */
    {
        glPushMatrix();
        glTranslatef(0.0, 0.02, -0.004);
        button_draw_all();
        glPopMatrix();
    }

    /* Shoulders */
    {
        glPushMatrix();
        glTranslatef(-0.055, 0.05, -0.018);
        shoulder_draw_all();
        glPopMatrix();
    }
    {
        glPushMatrix();
        glTranslatef(0.055, 0.05, -0.018);
        shoulder_draw_all();
        glPopMatrix();
    }

    /* Triggers */
    {
        glPushMatrix();
        float v = 0.0f;
        glTranslatef(-0.055, 0.038 - v, -0.026);
        trigger_draw_all();
        glPopMatrix();
    }
    {
        glPushMatrix();
        float v = 0.0f;
        glTranslatef(0.055, 0.038 - v, -0.026);
        trigger_draw_all();
        glPopMatrix();
    }
}

int main(int argc, char **argv)
{
    //putenv("OPENGX_DEBUG=all");
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_ShowCursor(SDL_DISABLE);

    SDL_Window *window = SDL_CreateWindow("EgcSdlTest",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          640, 480, SDL_WINDOW_OPENGL);
    if (!window) {
        fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL) {
        SDL_Log("Unable to create GL context: %s", SDL_GetError());
        return 1;
    }

    bool done = false;
    uint32_t lastTick = 0;

    glMatrixMode(GL_PROJECTION);
    gluPerspective( /* field of view in degree */ 40.0,
                    /* aspect ratio */ 1.0,
                    /* Z near */ 0.1, /* Z far */ 10.0);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, 0.4,  /* eye */
              0.0, 0.0, 0.0,      /* center */
              0.0, 1.0, 0.0);     /* up is in positive Y direction */

    static const GLfloat lightColor[] = {1.0, 1.0, 1.0, 1.0};
    static GLfloat lightPosition[] = {1.0, 1.0, 1.0, 0.0};
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05);
    glEnable(GL_LIGHT0);
    static const GLfloat ambientColor[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    s_num_controllers = 1;


    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                done = true;
                break;
            }
        }

        uint32_t newTick = SDL_GetTicks();
        int32_t elapsed = newTick - lastTick;
        lastTick = newTick;


        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

        for (int i = 0; i < s_num_controllers; i++) {
            glPushMatrix();
            draw_controller(&s_controllers[i]);
            glPopMatrix();
        }

        SDL_GL_SwapWindow(window);
    }

    SDL_Quit();

    return 0;
}

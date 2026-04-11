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
#include <embedded-game-controller/egc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MAX_CONTROLLERS 4

typedef struct {
    egc_input_device_t *egc;
    float color[3];
    float angle_x;
    float angle_y;
} Controller;

static Controller s_controllers[MAX_CONTROLLERS];
static int s_num_controllers = 0;

static inline bool has_button(const egc_device_description_t *desc,
                              egc_gamepad_button_e button)
{
    return desc->available_buttons & (1 << button);
}

static inline bool has_axis(const egc_device_description_t *desc,
                            egc_gamepad_axis_e axis)
{
    return desc->available_axes & (1 << axis);
}

static inline bool is_pressed(const egc_input_device_t *device, egc_gamepad_button_e button)
{
    return device->state.gamepad.buttons & (1 << button);
}

static int button_pressed_count(const egc_input_device_t *device)
{
    u32 buttons = device->state.gamepad.buttons;
    int count = 0;
    for (int i = 0; i < 32; i++) {
        if (buttons & (1 << i)) count++;
    }
    return count;
}

static void draw_controller(Controller *controller)
{
    const egc_input_device_t *device = controller->egc;
    const egc_device_description_t *desc = device->desc;

    glRotatef(controller->angle_x, 0.1, 0.0, 0.0);
    glRotatef(controller->angle_y, 0.0, 1.0, 0.0);
    glColor3fv(controller->color);
    controller_draw_all();

    glColor3f(0.2, 0.1, 0.2);
    float pressed_z = -0.0025;

    /* D-pad */
    glPushMatrix();
    glTranslatef(-0.05, 0.02, 0.02);
    if (has_button(desc, EGC_GAMEPAD_BUTTON_DPAD_LEFT)) {
        glPushMatrix();
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_DPAD_LEFT))
            glTranslatef(0.0, 0.0, pressed_z);
        button_dpad_draw_all();
        glPopMatrix();
    }
    if (has_button(desc, EGC_GAMEPAD_BUTTON_DPAD_DOWN)) {
        glPushMatrix();
        glRotatef(90, 0.0, 0.0, 1.0);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_DPAD_DOWN))
            glTranslatef(0.0, 0.0, pressed_z);
        button_dpad_draw_all();
        glPopMatrix();
    }
    if (has_button(desc, EGC_GAMEPAD_BUTTON_DPAD_RIGHT)) {
        glPushMatrix();
        glRotatef(180, 0.0, 0.0, 1.0);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_DPAD_RIGHT))
            glTranslatef(0.0, 0.0, pressed_z);
        button_dpad_draw_all();
        glPopMatrix();
    }
    if (has_button(desc, EGC_GAMEPAD_BUTTON_DPAD_UP)) {
        glPushMatrix();
        glRotatef(270, 0.0, 0.0, 1.0);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_DPAD_UP))
            glTranslatef(0.0, 0.0, pressed_z);
        button_dpad_draw_all();
        glPopMatrix();
    }
    glPopMatrix();

    /* Joysticks */
    if (has_axis(desc, EGC_GAMEPAD_AXIS_LEFTX)) {
        glPushMatrix();
        glTranslatef(-0.028, -0.007, -0.015);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_LEFT_STICK))
            glTranslatef(0.0, 0.0, pressed_z);
        glRotatef(device->state.gamepad.axes[EGC_GAMEPAD_AXIS_LEFTX] / 1500.0f,
                  0.0f, 1.0f, 0.0f);
        glRotatef(device->state.gamepad.axes[EGC_GAMEPAD_AXIS_LEFTY] / -1500.0f,
                  1.0f, 0.0f, 0.0f);
        joystick_draw_all();
        glPopMatrix();
    }
    if (has_axis(desc, EGC_GAMEPAD_AXIS_RIGHTX)) {
        glPushMatrix();
        glTranslatef(0.028, -0.007, -0.015);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_RIGHT_STICK))
            glTranslatef(0.0, 0.0, pressed_z);
        glRotatef(device->state.gamepad.axes[EGC_GAMEPAD_AXIS_RIGHTX] / 1500.0f,
                  0.0f, 1.0f, 0.0f);
        glRotatef(device->state.gamepad.axes[EGC_GAMEPAD_AXIS_RIGHTY] / -1500.0f,
                  1.0f, 0.0f, 0.0f);
        joystick_draw_all();
        glPopMatrix();
    }

    /* Main buttons */
    glPushMatrix();
    glTranslatef(0.05, 0.02, -0.007);
    if (has_button(desc, EGC_GAMEPAD_BUTTON_WEST)) {
        glPushMatrix();
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_WEST))
            glTranslatef(0.0, 0.0, pressed_z);
        button_big_draw_all();
        glPopMatrix();
    }
    if (has_button(desc, EGC_GAMEPAD_BUTTON_SOUTH)) {
        glPushMatrix();
        glRotatef(90, 0.0, 0.0, 1.0);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_SOUTH))
            glTranslatef(0.0, 0.0, pressed_z);
        button_big_draw_all();
        glPopMatrix();
    }
    if (has_button(desc, EGC_GAMEPAD_BUTTON_EAST)) {
        glPushMatrix();
        glRotatef(180, 0.0, 0.0, 1.0);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_EAST))
            glTranslatef(0.0, 0.0, pressed_z);
        button_big_draw_all();
        glPopMatrix();
    }
    if (has_button(desc, EGC_GAMEPAD_BUTTON_NORTH)) {
        glPushMatrix();
        glRotatef(270, 0.0, 0.0, 1.0);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_NORTH))
            glTranslatef(0.0, 0.0, pressed_z);
        button_big_draw_all();
        glPopMatrix();
    }
    glPopMatrix();

    /* Start and back */
    if (has_button(desc, EGC_GAMEPAD_BUTTON_START)) {
        glPushMatrix();
        glTranslatef(0.012, 0.02, -0.0055);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_START))
            glTranslatef(0.0, 0.0, pressed_z);
        start_draw_all();
        glPopMatrix();
    }
    if (has_button(desc, EGC_GAMEPAD_BUTTON_BACK)) {
        glPushMatrix();
        glTranslatef(-0.012, 0.02, -0.0055);
        glRotatef(180, 0.0, 0.0, 1.0);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_BACK))
            glTranslatef(0.0, 0.0, pressed_z);
        start_draw_all();
        glPopMatrix();
    }

    /* Guide */
    if (has_button(desc, EGC_GAMEPAD_BUTTON_GUIDE)) {
        glPushMatrix();
        glTranslatef(0.0, 0.02, -0.004);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_GUIDE))
            glTranslatef(0.0, 0.0, pressed_z);
        button_draw_all();
        glPopMatrix();
    }

    /* Shoulders */
    if (has_button(desc, EGC_GAMEPAD_BUTTON_LEFT_SHOULDER)) {
        glPushMatrix();
        glTranslatef(-0.055, 0.05, -0.018);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_LEFT_SHOULDER))
            glTranslatef(0.0, pressed_z, 0.0);
        shoulder_draw_all();
        glPopMatrix();
    }
    if (has_button(desc, EGC_GAMEPAD_BUTTON_RIGHT_SHOULDER)) {
        glPushMatrix();
        glTranslatef(0.055, 0.05, -0.018);
        if (is_pressed(device, EGC_GAMEPAD_BUTTON_RIGHT_SHOULDER))
            glTranslatef(0.0, pressed_z, 0.0);
        shoulder_draw_all();
        glPopMatrix();
    }

    /* Triggers */
    if (has_axis(desc, EGC_GAMEPAD_AXIS_LEFT_TRIGGER)) {
        glPushMatrix();
        float v = device->state.gamepad.axes[EGC_GAMEPAD_AXIS_LEFT_TRIGGER] / 2500000.0f;
        glTranslatef(-0.055, 0.038 - v, -0.026);
        trigger_draw_all();
        glPopMatrix();
    }
    if (has_axis(desc, EGC_GAMEPAD_AXIS_RIGHT_TRIGGER)) {
        glPushMatrix();
        float v = device->state.gamepad.axes[EGC_GAMEPAD_AXIS_RIGHT_TRIGGER] / 2500000.0f;
        glTranslatef(0.055, 0.038 - v, -0.026);
        trigger_draw_all();
        glPopMatrix();
    }
}

static bool animate_controller(Controller *controller, uint32_t elapsed_ms)
{
    const egc_input_device_t *device = controller->egc;
    s16 movement_x = 0.0f;
    if (is_pressed(device, EGC_GAMEPAD_BUTTON_DPAD_LEFT)) {
        movement_x = INT16_MIN;
    } else if (is_pressed(device, EGC_GAMEPAD_BUTTON_DPAD_RIGHT)) {
        movement_x = INT16_MAX;
    } else if (has_axis(device->desc, EGC_GAMEPAD_AXIS_LEFTX)) {
        movement_x = device->state.gamepad.axes[EGC_GAMEPAD_AXIS_LEFTX];
    }

    controller->angle_y += (double)elapsed_ms * movement_x / 300000.0;

    s16 movement_y = 0.0f;
    if (is_pressed(device, EGC_GAMEPAD_BUTTON_DPAD_DOWN)) {
        movement_y = INT16_MIN;
    } else if (is_pressed(device, EGC_GAMEPAD_BUTTON_DPAD_UP)) {
        movement_y = INT16_MAX;
    } else if (has_axis(device->desc, EGC_GAMEPAD_AXIS_LEFTY)) {
        movement_y = device->state.gamepad.axes[EGC_GAMEPAD_AXIS_LEFTY];
    }

    controller->angle_x -= (double)elapsed_ms * movement_y / 300000.0;

    return button_pressed_count(device) >= 4;
}

static void device_added_cb(egc_input_device_t *device, void *userdata)
{
    static const float colors[5][3] = {
        { 1.0, 1.0, 1.0 },
        { 0.5, 1.0, 0.6 },
        { 1.0, 0.5, 0.6 },
        { 0.5, 0.6, 1.0 },
        { 0.1, 0.1, 0.2 },
    };
    static int count = 0;
    Controller *controller = &s_controllers[s_num_controllers++];
    controller->egc = device;
    controller->angle_y = 0.0f;
    memcpy(controller->color, colors[(count++ % 5)], sizeof(float[3]));
}

static void device_removed_cb(egc_input_device_t *device, void *userdata)
{
    int num_controllers = s_num_controllers;
    for (int i = 0; i < num_controllers; i++) {
        if (s_controllers[i].egc == device) {
            s_num_controllers--;
        }
        if (s_num_controllers != num_controllers) {
            /* We removed one */
            s_controllers[i] = s_controllers[i + 1];
        }
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



    egc_initialize(device_added_cb, device_removed_cb, NULL);
    egc_bt_start_scan();

    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                done = true;
                break;
            }
        }

        egc_handle_events();

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
            bool quit = animate_controller(&s_controllers[i], elapsed);
            if (quit) done = true;
            draw_controller(&s_controllers[i]);
            glPopMatrix();
        }

        SDL_GL_SwapWindow(window);
    }

    SDL_Quit();

    return 0;
}

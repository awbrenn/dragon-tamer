//
// Created by awbrenn on 2/14/16.
//

#ifndef DRAGON_FIRE_VIEWPORT_NAVIGATION_H
#define DRAGON_FIRE_VIEWPORT_NAVIGATION_H

enum MOUSE_ACTION {
  left_mouse_button = 0,
  middle_mouse_button = 1,
  right_mouse_button = 2,
  mouse_wheel_forward = 3,
  mouse_wheel_backward = 4,
};

// camera manipulation functions
void initViewPortNavigation(float *_eye, float *_original_eye);
void zoomEye   (enum MOUSE_ACTION viewport_action, float* eye_position, float* view_point);
void rotateEye (int x, int y, int start_x, int start_y, float* eye_position, float* view_point, float sensitivity);
void panEye    (int x, int y, int start_x, int start_y, float* eye_position, float* view_point, float sensitivity);

void movePointToLocation(float *point, float *location);

void handleKeys(unsigned char key, int x, int y);
void handleMouse(int button, int state, int x, int y);
void handleMovedMouse(int x, int y);
void idle();

#endif //DRAGON_FIRE_VIEWPORT_NAVIGATION_H

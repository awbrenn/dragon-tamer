//
// Created by awbrenn on 2/14/16.
//

#include "viewport_navigation.h"
#include "math.h"

// global variables
float ZOOM_INCREMENT = 0.1f;
float ZOOM_MIN = 0.1f;


float getMagnitudeOfVector(float x, float y, float z) {
  return sqrtf(x*x+y*y+z*z);
}


void makeUnitLength(float *vector) {
  float magnitude_of_vector;

  magnitude_of_vector = getMagnitudeOfVector(vector[0], vector[1], vector[2]);
  vector[0] /= magnitude_of_vector;
  vector[1] /= magnitude_of_vector;
  vector[2] /= magnitude_of_vector;
}


void zoomEye(enum MOUSE_ACTION mouse_action, float* eye_position, float* view_point) {
  float zoom_direction;
  float view_direction_magnitude;
  float view_direction[3];

  zoom_direction = ((mouse_action == mouse_wheel_forward) ? 1.0f : -1.0f);

  // get normalized view direction
  view_direction[0] = *view_point - *eye_position;
  view_direction[1] = *(view_point+1) - *(eye_position+1);
  view_direction[2] = *(view_point+2) - *(eye_position+2);

  view_direction_magnitude = getMagnitudeOfVector(view_direction[0], view_direction[1], view_direction[2]);

  // don't zoom any further if zoom min is reached
  if (((view_direction_magnitude > 0 && view_direction_magnitude <= ZOOM_MIN) ||
        view_direction_magnitude < 0 && view_direction_magnitude >= (-1.0f * ZOOM_MIN))
        && mouse_action == mouse_wheel_forward)
  {
    return;
  }

  makeUnitLength(view_direction);

  // set new eye_position;
  eye_position[0] += view_direction[0] * zoom_direction * ZOOM_INCREMENT;
  eye_position[1] += view_direction[1] * zoom_direction * ZOOM_INCREMENT;
  eye_position[2] += view_direction[2] * zoom_direction * ZOOM_INCREMENT;
}


// yields the result of rotating the point (x,y,z) about the line through (a,b,c)
// with direction vector ⟨u,v,w⟩ (where u2 + v2 + w2 = 1) by the angle theta.
void rotateByArbitraryAxis(float x, float y, float z,
                           float a, float b, float c,
                           float u, float v, float w,
                           float theta, float *result)
{
  float cos_theta = cosf(theta);
  float sin_theta = sinf(theta);

  result[0] = (a*(v*v + w*w) - u*(b*v + c*w - u*x - v*y - w*z)) * (1-cos_theta) + x*cos_theta + ((-1.0f)*c*v + b*w - w*y + v*z)*sin_theta;
  result[1] = (b*(u*u + w*w) - v*(a*u + c*w - u*x - v*y - w*z)) * (1-cos_theta) + y*cos_theta + (        c*u - a*w + w*x - u*z)*sin_theta;
  result[2] = (c*(u*u + v*v) - w*(a*u + b*v - u*x - v*y - w*z)) * (1-cos_theta) + z*cos_theta + ((-1.0f)*b*u + a*v - v*x + u*y)*sin_theta;
}


// get the axis that is perpendicular to vector u and the y axis
void getUnitPerpendicularVector(float *u, float *result) {
  result[0] = (-1.0f)*u[2];
  result[1] = 0.0f;
  result[2] = u[0];

  // normalize perpendicular axis
  makeUnitLength(result);
}

void getUnitVectorViewPointToEye(float *view_point, float *eye_position, float *view_point_to_eye) {
  view_point_to_eye[0] = eye_position[0] - view_point[0];
  view_point_to_eye[1] = eye_position[1] - view_point[1];
  view_point_to_eye[2] = eye_position[2] - view_point[2];

  makeUnitLength(view_point_to_eye);
}


void rotateEye(int x, int y, int start_x, int start_y, float* eye_position, float* view_point, float sensitivity) {
  float view_point_to_eye[3];
  float change_in_yaw;
  float change_in_pitch;
  float rotation_axis[3];

  change_in_pitch =           ((float) y - start_y) / sensitivity;
  change_in_yaw   = (-1.0f) * ((float) x - start_x) / sensitivity;

  getUnitVectorViewPointToEye(view_point, eye_position, view_point_to_eye);

  // rotate the eye about the y axis relative to the view point
  rotateByArbitraryAxis(*(eye_position), *(eye_position+1), *(eye_position+2),
                        *(view_point)  , *(view_point+1)  , *(view_point+2),
                        0.0f, 1.0f, 0.0f, change_in_yaw, eye_position);

  getUnitPerpendicularVector(view_point_to_eye, rotation_axis);

  // rotate the eye about the rotation axis relative to the viewpoint
  rotateByArbitraryAxis(*(eye_position ), *(eye_position +1), *(eye_position +2),
                        *(view_point   ), *(view_point   +1), *(view_point   +2),
                        *(rotation_axis), *(rotation_axis+1), *(rotation_axis+2),
                        change_in_pitch, eye_position);
}


void panEye(int x, int y, int start_x, int start_y, float* eye_position, float* view_point, float sensitivity) {
  float change_in_x;
  float change_in_y;
  float perpendicular_axis[3];
  float view_point_to_eye[3];


  change_in_x = ((float) x - start_x)/sensitivity;
  change_in_y = ((float) y - start_y)/sensitivity;

  // pan along y axis
  eye_position[1] += change_in_y;
  view_point[1]   += change_in_y;

  // get the vector that is perpendicular to the y axis and the direction of view_point_to_eye
  getUnitVectorViewPointToEye(view_point, eye_position, view_point_to_eye);
  getUnitPerpendicularVector(view_point_to_eye, perpendicular_axis);

  // pan along x axis
  eye_position[0] += perpendicular_axis[0]*change_in_x;
  eye_position[2] += perpendicular_axis[2]*change_in_x;

  view_point[0] += perpendicular_axis[0]*change_in_x;
  view_point[2] += perpendicular_axis[2]*change_in_x;
}


void movePointToLocation(float *point, float *location) {
  point[0] = location[0];
  point[1] = location[1];
  point[2] = location[2];
}

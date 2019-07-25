#include "Camera.h"

Camera::Camera(float fx, float fy, float x0, float y0, int W, int H, float near_plane, float far_plane, float scale)
{
	float A, B;
	A = near_plane + far_plane;
	B = near_plane * far_plane;

	camera_intrinsics = { {fx,  0.0, x0},
						  {0.0, fy,  y0},
						  {0.0, 0.0, 1.0} };

	perspective_matrix = {{fx*scale,	0.0,		-x0 * scale,	0.0},
						  {0.0,			fy*scale,	-y0 * scale,	0.0},
						  {0.0,			0.0,		A,			    B},
						  {0.0,			0.0,		-1.0,		    0.0} };

	glortho = glm::ortho(0.0f, float(W), float(H), 0.0f, near_plane, far_plane);//column-major order

	perspective_NDC = perspective_matrix * glm::transpose(glortho);//row majored oder

}

Camera::~Camera()
{
}
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>
#include<gtc/random.hpp>

class Camera
{
public:
	Camera(float fx, float fy, float x0, float y0, int W, int H, float near_plane, float far_plane, float scale=1);
	~Camera();
	//camera intrinxics theta=90(no skew), 
	//fx: scale factor on x axis
	//fy: scale factor on y axis
	//x0: principle point x coordinate, in left top origin coordinate system
	//y0: principle point y coordinate, in left top origin coordinate system
	//near_plane: clipping near plane
	//far_plane: clipping far plane
	glm::mat3 camera_intrinsics;
	glm::mat4 perspective_matrix;
	glm::mat4 glortho;  //transform to NDC
	glm::mat4 perspective_NDC;
};



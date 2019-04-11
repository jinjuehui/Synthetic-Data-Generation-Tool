#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm.hpp>
#include<iostream>

#include "Renderer.h"


struct CameraOrientation//camera orientation , implement it to camera class in the future
{
	glm::vec3 camera_pose = glm::vec3{ 0.0f,0.10f,20.0f };//{ 0.0f, 10.0f, 20.0f };
	glm::vec3 camera_front = glm::vec3{ 0.0f,0.0f,0.0f }-camera_pose;//the target camera look at - camera position
	glm::vec3 camera_up = glm::vec3{ 0.0f,1.0f,0.0f };
};

struct object_setting_for_fragment_shader//define object fragment shader setup
{
	//default white plastic
	glm::vec3 color{ 1.0f,1.0f,1.0f };
	glm::vec3 position{ 0.0f,0.0f,0.0f };
	glm::vec3 ambient{ 1.0f,1.0f,1.0f };
	glm::vec3 diffuse{ 0.55f,0.55f,0.55f };
	glm::vec3 specular{ 0.7f,0.7f,0.7f };
	float shininess = 15.0f;
};

struct light
{
	glm::vec3 light_color = { 1.0f,1.0f,1.0f };
	glm::vec3 light_direction = { -0.2,-1.0f,-0.3f };
	glm::vec3 light_position = { 5.0f,0.0f,2.0f };

	glm::vec3 ambient = { 0.2f,0.2f,0.2f };
	glm::vec3 diffuse = { 0.1f,0.1f,0.1f };
	glm::vec3 specular = light_color;

	float constantoffset = 1.0f;
	float linearfactor = 0.09f;
	float quadraticfactor = 0.032f;

	float cutoff = glm::cos(glm::radians(12.5f));
	float outercutoff = glm::cos(glm::radians(15.0f));
}; //lightning setup for fragment shader

struct bounding_box 
{
	int x;
	int y;
	int w;
	int h;

};

CameraOrientation rotateCamera(int P, int Y, float distance);
GLFWwindow* initialize_window(int width, int height, const char* name);
bounding_box caculate_bounbox(float x_min, float x_max, float y_min, float y_max,int screen_w,int screen_h);


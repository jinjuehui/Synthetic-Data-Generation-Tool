#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm.hpp>
#include<iostream>
#include<algorithm>
#include<nlohmann/json.hpp>
#include<sstream>
#include "Renderer.h"
#include "Mesh.h"

using json = nlohmann::json;

//camera orientation , implement it to camera class in the future
struct CameraOrientation
{
	glm::vec3 camera_pose = glm::vec3{ 0.0f,0.10f,20.0f };//{ 0.0f, 10.0f, 20.0f };
	glm::vec3 camera_front = glm::vec3{ 0.0f,0.0f,0.0f }-camera_pose;//the target camera look at - camera position
	glm::vec3 camera_up = glm::vec3{ 0.0f,1.0f,0.0f };
};


//define object fragment shader setup
struct object_setting_for_fragment_shader
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

//bounding box in normal convention, the label is formed in this structure

class BoundingBox {
public:

	struct bounding_box //in pixel
	{
		int x;
		int y;
		int w;
		int h;
	
	}bb;
	
	//bounding box vertex used to draw the bb on the screen to visualize the result of the calculation
	struct bounding_box_2d_param
	{
		float x_min;
		float x_max;
		float y_min;
		float y_max;
	}bb_v;
	
	struct bounding_box_3d_param
	{
		float x_min;
		float x_max;
		float y_min;
		float y_max;
		float z_min;
		float z_max;
	}bb_v_3d;


	BoundingBox(Model train_object, int screen_w, int screen_h);
	void calculate_boundingbox(float x_min, float x_max, float y_min, float y_max, int screen_w, int screen_h);
	void generate_bounding_box_3d(Model train_object);
	void generate_bounding_box_labels_2d(Model train_object,
										int screen_w,
										int screen_h,
										int P,
										int Y,
										int R,
										glm::mat4 projection,
										glm::mat4 camera,
										glm::mat4 model,
										std::ofstream &jsonfile,
										std::string json_path
										);//very time consuming

	void generate_bounding_box_labels_3d(std::vector<glm::vec3> bb_glm_vec3,
										int screen_w,
										int screen_h,
										int P,
										int Y,
										int R,
										glm::mat4 projection,
										glm::mat4 camera,
										glm::mat4 model,
										std::ofstream &jsonfile,
										std::string json_path
										);
	
};


CameraOrientation rotateCamera(int P, int Y, float distance);
GLFWwindow* initialize_window(int width, int height, const char* name);
void generate_yaml_label(std::ofstream &jsonfile,std::string json_path,json labels, std::string object_path, BoundingBox::bounding_box bb, int P, int Y, int R);



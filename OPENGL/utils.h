#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<GL/GL.h>
#include<GL/GLU.h>
#include<GL/glut.h>
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>
#include<gtc/random.hpp>
#include<iostream>
#include<algorithm>
#include<nlohmann/json.hpp>
#include<sstream>

#include"VertexBuffer.h"
#include"Renderer.h"
#include"Mesh.h"
#include <FreeImage.h>
#include <random>
#include <iomanip>

#define RADPERDEG 0.0174533 // use to draw arrow for coordinate

using json = nlohmann::json;

//camera orientation , implement it to camera class in the future
struct CameraOrientation
{
	glm::vec3 camera_pose = glm::vec3{ 0.0f,0.0f,0.5f };//{ 0.0f, 10.0f, 20.0f };
	glm::vec3 camera_front = glm::vec3{ 0.0f,0.0f,0.0f }-camera_pose;//the target camera look at - camera position
	glm::vec3 camera_up = glm::vec3{ 0.0f,1.0f,0.0f };
	
};

//define object fragment shader setup
struct object_setting_for_fragment_shader
{
	//default white plastic												//change object material here
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

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;
};

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
	struct bounding_box_2d_param//might not needed sice in bounding_box_3d_param all these params are included
	{
		float x_min;
		float x_max;
		float y_min;
		float y_max;
	}bb_v;
	
	struct bounding_box_3d_param//calculate once when the object is created
	{
		float x_min;
		float x_max;
		float y_min;
		float y_max;
		float z_min;
		float z_max;
	}bb_v_3d;

	std::vector<glm::vec3> bb_glm_vec3;//for calculate the boundingbox3d

	//for visualization the vertex member:
	//3d:

	//float bounding_box_vertex_8point[24];
	unsigned int bounding_box_vertex_8point_indecies[36] =
	{
		0,1,2,
		0,2,3,
		4,5,6,
		3,6,7,
		0,1,5,
		0,5,4,
		3,2,6,
		3,6,7,
		0,4,7,
		0,3,7,
		1,5,6,
		1,6,2
	};

	//2d:
	//float bounding_box_vertex_4point[12];
	unsigned int bounding_box_vertex_4point_indecies[6] =
	{
		0,1,2,
		3,0,2
	};

	std::map<std::string, int> AttribPointer_BB_3d;

	//VertexBuffer* BB_2d;
	//VertexBuffer* BB_3d;




	BoundingBox(Model train_object);//the constrcutor fill the bb_v_3d
	~BoundingBox();
	//void construct_BB_2d(float* bounding_box_vertex_4point);//not working
	//void construct_BB_3d(float* bounding_box_vertex_8point);//not working
	void fill_bb_glm_vec3(float* bounding_box_vertex_8point);
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

	void generate_bounding_box_labels_3d(int screen_w,
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
	//void draw_2d(Shader shader, glm::mat4 model, glm::mat4 camera, glm::mat4 projection);//not working
	//void draw_3d(Shader shader, glm::mat4 model,glm::mat4 camera,glm::mat4 projection);//not working
};

std::string to_format(const int number);
glm::vec3 set_random_position(int range);
glm::vec3 set_random_with_distribution(std::default_random_engine &generator, float xy, float z, float sigma);
glm::vec3 random_vec3(std::default_random_engine &generator, float xy_min, float xy_max, float z_min, float z_max);
void rotate_object(glm::mat4 &model, int axis, float velocity);
void rotate_light(glm::mat4 &light_model, int P, int Y, float distance);
glm::mat4 rotate_object_3axis_randomly(glm::mat4 &model, std::default_random_engine &generator);
void inverse_object_3axis_rotation(glm::mat4 &model, glm::mat4 rotation_matrix);
float random_float(std::default_random_engine &generator, float v_min, float v_max);
glm::vec3 random_v3_norm(std::default_random_engine &generator, float a, float b, float c, float d);
std::vector<float> projection_single_point_on_creen(glm::vec3 point, glm::mat4 model, glm::mat4 camera, glm::mat4 projection);
CameraOrientation rotateCamera(int P, int Y, float distance);
GLFWwindow* initialize_window(int width, int height, const char* name);
void generate_yaml_label(std::ofstream &jsonfile,std::string json_path,json labels, std::string object_path, BoundingBox::bounding_box bb, int P, int Y, int R);
void screenshot_freeimage(const char* screenshotFile, int width, int height);
inline void calculateRotation(Quaternion& q, glm::mat4 a);
void Arrow(GLdouble x1, GLdouble y1, GLdouble z1, GLdouble x2, GLdouble y2, GLdouble z2, GLdouble D);
void drawAxes(GLdouble length);


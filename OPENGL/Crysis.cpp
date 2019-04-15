#include "utils.h"

//OpenGL mesh loader
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//write image output
#include <FreeImage.h>

//self defined headers
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "stb_image.h"


//C++ basics
#include <fstream>
#include <map>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;


//Triggers and Keys
//#define LOAD_MODEL "mesh/nanosuit/chess/queen.obj"
#define LOAD_CUBE_REFERENCE "mesh/nanosuit/chess/test/untitled.obj"
#define LOAD_MODEL "mesh/obj_05.stl"
#define ROTATE_CAMERA true
#define ENABLE_RANDOM_LIGHT_SOURCE_POSITION true
#define USE_SIMPLE_LIGHTNING_MODEL false
bool USE_BACKGROUND_IMAGE = true;
bool STATIC_CAMERA_VIEW = true;
bool ENABLE_USER_INPUT_TO_CONTROL_CAMERA = !STATIC_CAMERA_VIEW;
bool ROTATE_LIGHT = false;

//parameters
//Screen Parameters:
std::string const path = LOAD_MODEL;
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;
//System Time:
float deltaTime(0.0f), lastFrame(0.0f);//now the variables are only used for keyboard input callback functions		
//User Input Mouse and cursor
bool firstMouse(true);
double lastX(SCR_WIDTH / 2), lastY(SCR_HEIGHT / 2);
float yaw(-90.0f), pitch(0.0f), fov(45.0f);
//Drawing object	
glm::mat4 lamp, back_position;
glm::vec3 back_ground_position(1.0f,1.0f,1.0f);
glm::vec3 light_position(1.0f,0.0f,2.0f);

using json = nlohmann::json;

light dirLight, pointLight, spotLight,lightning;
object_setting_for_fragment_shader train_object,reference_object;
glm::vec3 light_positions[] =
{	glm::ballRand(10.0),
	glm::ballRand(15.0),
	glm::ballRand(20.0),
	glm::ballRand(50.0)
};

float cube_vertex[] = {
	0.1f,	-0.1f,	-0.1f,	0.0f,	-1.0f,	-0.0f,
	0.1f,	-0.1f,	0.1f,	0.0f,	-1.0f,	-0.0f,
	-0.1f,	-0.1f,	0.1f,	0.0f,	-1.0f,	-0.0f,
	-0.1f,	-0.1f,	-0.1f,  0.0f,	-1.0f,	-0.0f,
	0.1f,	0.1f,	-0.1f,  0.0f,    1.0f,	0.0f,
	-0.1f,	0.1f,	-0.1f,	0.0f,	1.0f,	0.0f,
	-0.1f,	0.1f,	0.1f,	0.0f,	1.0f,	0.0f,
	0.1f,	0.1f,	0.1f,	0.0f,	1.0f,	0.0f,
	0.1f,	-0.1f,	-0.1f,	1.0f,	0.0f,	0.0f,
	0.1f,	0.1f,	-0.1f,	1.0f,	0.0f,	0.0f,
	0.1f,	0.1f,	0.1f,	1.0f,	0.0f,	0.0f,
	0.1f,	-0.1f,	0.1f,	1.0f,	0.0f,	0.0f,
	0.1f,	-0.1f,	0.1f,	-0.0f,	-0.0f,	1.0f,
	0.1f,	0.1f,	0.1f,	-0.0f,	-0.0f,	1.0f,
	-0.1f,	0.1f,	0.1f,	-0.0f,	-0.0f,	1.0f,
	-0.1f,	-0.1f,	0.1f,	-0.0f,	-0.0f,	1.0f,
	-0.1f,	-0.1f,	0.1f,	-1.0f,	-0.0f,	-0.0f,
	-0.1f,	0.1f,	0.1f,	-1.0f,	-0.0f,	-0.0f,
	-0.1f,	0.1f,	-0.1f,	-1.0f,	-0.0f,	-0.0f,
	-0.1f,	-0.1f,	-0.1f,	-1.0f,	-0.0f,	-0.0f,
	0.1f,	0.1f,	-0.1f,	0.0f,	0.0f,	-1.0f,
	0.1f,	-0.1f,	-0.1f,	0.0f,	0.0f,	-1.0f,
	-0.1f,	-0.1f,	-0.1f,	0.0f,	0.0f,	-1.0f,
	-0.1f,	0.1f,	-0.1f,	0.0f,	0.0f,	-1.0f
};
unsigned int indicies_cube[] = {
	 0,  1,  2,
	 0,  2,  3,
	 4,  5,  6,
	 4,  6,  7,
	 8,  9,  10,
	 8,  10,  11,
	 12,  13,  14,
	 12,  14,  15,
	 16,  17,  18,
	 16,  18,  19,
	 20,  21,  22,
	 20,  22,  23
};

//camera setup with default parameters
CameraOrientation Setup;

glm::mat4 rotateLight(glm::mat4 light_model, int P, int Y, float distance)
{
	float x_direction = distance * glm::cos(glm::radians((float)P))*cos(glm::radians((float)Y));
	float y_direction = distance * glm::sin(glm::radians((float)P));
	float z_direction = -distance * glm::cos(glm::radians((float)P))*sin(glm::radians((float)Y));
	
	light_position = glm::vec3{ x_direction,y_direction,z_direction };
	
	light_model = glm::translate(light_model, glm::vec3{x_direction,y_direction,z_direction});
	return light_model;
}



std::map<std::string, int> read_images_in_folder(std::string path)
{
	std::map<std::string, int> files;
	//std::cout << "out side loop" << std::endl;
	for (const auto & entry : fs::directory_iterator(path))
	{
		//std::cout << "reading images" << std::endl;
		files[entry.path().string()];
		//std::cout << entry.path() << std::endl;
	}
	//std::cout << "finish reading" << std::endl;
	return files;

}

void rotate_object(glm::mat4 &model, int axis, float velocity)
{
	//model = glm::rotate(model, glm::radians(velocity), glm::vec3(0.0f, 0.0f, 0.0f));
	//std::cout << "Object Rotation Enabled!" << std::endl;
	switch (axis)
	{
		case 1:
			model = glm::rotate(model, glm::radians(velocity), glm::vec3(1.0f, 0.0f, 0.0f));
			break;
		case 2:
			model = glm::rotate(model, glm::radians(velocity), glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case 3:
			model = glm::rotate(model, glm::radians(velocity), glm::vec3(0.0f, 0.0f, 1.0f));
			break;
	}
}


//call back function for mouse scrolling to zoom the view
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
	{
		fov -= yoffset;
		//std::cout <<"fov: " <<fov << std::endl;
	}

	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}


//call back function for mouse move to view different orientations
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	glm::vec3 front;
	front.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw))*cos(glm::radians(pitch));
	Setup.camera_front =glm::normalize(front);

}

//move the camera forward, backward, sideways
void wasd_keyinput(GLFWwindow* window)
{
	float camera_speed =5.0f*deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Setup.camera_pose += camera_speed * glm::normalize(Setup.camera_front);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Setup.camera_pose -= camera_speed * glm::normalize(Setup.camera_front);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Setup.camera_pose -= camera_speed * glm::normalize(glm::cross(Setup.camera_front, Setup.camera_up));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Setup.camera_pose += camera_speed * glm::normalize(glm::cross(Setup.camera_front, Setup.camera_up));
	}
   if (glfwGetKey(window, GLFW_KEY_SPACE)== GLFW_PRESS)
   {
	   Setup.camera_pose += camera_speed * glm::normalize(Setup.camera_up);
   }
   if (glfwGetKey(window,GLFW_KEY_R)==GLFW_PRESS)
   {
	   Setup.camera_pose -= camera_speed * glm::normalize(Setup.camera_up);
   }
};


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{	
		std::cout << "escape pressed!" << std::endl;
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		//glfwTerminate();
	}
}


void screenshot_freeimage(const char* screenshotFile, int width, int height ) {

	// Make the BYTE array, factor of 3 because it's RBG.
	BYTE* pixels = new BYTE[3 * width * height];

	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

	// Convert to FreeImage format & save to file
	//FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
	FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3*width, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
	FreeImage_Save(FIF_JPEG, image, screenshotFile, 0);

	// Free resources
	FreeImage_Unload(image);
	delete[] pixels;
}


void convert_array(glm::mat4 mat, float pose[][4])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			pose[i][j] = glm::transpose(mat)[i][j];
		}
	}
}

float verticesLight[] = {
	-0.01f, -0.01f, -0.01f,
	 0.01f, -0.01f, -0.01f,
	 0.01f,  0.01f, -0.01f,
	 0.01f,  0.01f, -0.01f,
	-0.01f,  0.01f, -0.01f,
	-0.01f, -0.01f, -0.01f,

	-0.01f, -0.01f,  0.01f,
	 0.01f, -0.01f,  0.01f,
	 0.01f,  0.01f,  0.01f,
	 0.01f,  0.01f,  0.01f,
	-0.01f,  0.01f,  0.01f,
	-0.01f, -0.01f,  0.01f,

	-0.01f,  0.01f,  0.01f,
	-0.01f,  0.01f, -0.01f,
	-0.01f, -0.01f, -0.01f,
	-0.01f, -0.01f, -0.01f,
	-0.01f, -0.01f,  0.01f,
	-0.01f,  0.01f,  0.01f,

	 0.01f,  0.01f,  0.01f,
	 0.01f,  0.01f, -0.01f,
	 0.01f, -0.01f, -0.01f,
	 0.01f, -0.01f, -0.01f,
	 0.01f, -0.01f,  0.01f,
	 0.01f,  0.01f,  0.01f,

	-0.01f, -0.01f, -0.01f,
	 0.01f, -0.01f, -0.01f,
	 0.01f, -0.01f,  0.01f,
	 0.01f, -0.01f,  0.01f,
	-0.01f, -0.01f,  0.01f,
	-0.01f, -0.01f, -0.01f,

	-0.01f,  0.01f, -0.01f,
	 0.01f,  0.01f, -0.01f,
	 0.01f,  0.01f,  0.01f,
	 0.01f,  0.01f,  0.01f,
	-0.01f,  0.01f,  0.01f,
	-0.01f,  0.01f, -0.01f
};

//needed for print an image on the Background
float background[] = {
		 1.0f,	1.0f,	0.0f, 1.f, 1.0f, // top right
		 1.0f,  -1.0f,	0.0f, 1.0f, 0.0f, // bottom right
		-1.0f,	-1.0f,	0.0f, 0.0f, 0.0f, // bottom left
		-1.0f,	1.0f,	0.0f, 0.0f, 1.0f

	//-1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	//-1.0f, 0.0f,  1.0f, 0.0f, 1.0f,
	// 1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
	// 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,
};

unsigned int back_indicies[] = 
{
	0,1,2,
	3,0,2
};


//TODO:
//* Object Position randomization
//* bench marking
//* photo realistic rendering
//* output data in PascalVOC structure
//* add instance segmentation
//* add semantic segmentation
//* check rotation information
//* add xml generator
//* Image Loading problem, some image cause glTexImage2D exception break(walk arounded with deleting image from folder, probably related with aspect ration of the picture)
//* implement the split window to show both the rendered data and the ground truth data

//Done(need be checked):
//* fix no rendering happens when P=0  (walk arounded with P set to 1)
//* Generating bounding box
//* generating 3d bounding box
//* Add in plane rotation
//* set pictures as the background of the window
//* optimize all of the VAO and VBOs


//void window_size_callback(GLFWwindow* window, int width, int height)
//{
//	glfwGetWindowSize(window, &width, &height);
//}



int main()
{
	//0.create window====================================================================
	GLFWwindow* window[2];

	//initialize glfw
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//Why it doesn't work with 2,3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//if core_profile is chosen, the vertex array object need to be manually created
	window[0] = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, u8"Rendering 正在进行渲染...", NULL, NULL);
	//window[1] = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, u8"GroundTruth 正在进行渲染...", NULL, NULL);
	
	//setting up the first window
	if (!window[0])
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "Window creation succeed!" << std::endl;
	}

	glfwMakeContextCurrent(window[0]);
	//glfwSetWindowSizeCallback(window[0], window_size_callback);
	std::cout << "use ESC to exit the Window" << std::endl;
	glfwSetKeyCallback(window[0], key_callback);
	glfwSetWindowPos(window[0], 500, 500);
	
	if (glewInit() != GLEW_OK)
	{
		std::cout << "initialize glew failed" << std::endl;
	}
	else
	{
		std::cout << "glew initialization succeed!" << std::endl;
	}

	GLCall(glViewport(0,0,SCR_WIDTH,SCR_HEIGHT));


	std::map<std::string, int> AttribPointer_cube, AttribPointer_Background;
	AttribPointer_cube["layout_0"] = 0;
	AttribPointer_cube["size_of_vertex_0"] = 3;
	AttribPointer_cube["stride_0"] = 6*sizeof(float);
	AttribPointer_cube["offset_0"] = 0;
	AttribPointer_cube["layout_1"] = 1;
	AttribPointer_cube["size_of_vertex_1"] = 3;
	AttribPointer_cube["stride_1"] = 6*sizeof(float);
	AttribPointer_cube["offset_1"] = 3*sizeof(float);
	AttribPointer_Background["layout_0"] = 0;
	AttribPointer_Background["size_of_vertex_0"] = 3;
	AttribPointer_Background["stride_0"] = 5 * sizeof(float);
	AttribPointer_Background["offset_0"] = 0;
	AttribPointer_Background["layout_1"] = 1;
	AttribPointer_Background["size_of_vertex_1"] = 2;
	AttribPointer_Background["stride_1"] = 5 * sizeof(float);
	AttribPointer_Background["offset_1"] = 3 * sizeof(float);
	//draw lights
	VertexBuffer Lightning(verticesLight,108,sizeof(float),0,3,3*sizeof(float),0);


	VertexBuffer Cube(cube_vertex,
					  indicies_cube,
					  sizeof(cube_vertex)/sizeof(cube_vertex[0]),
					  sizeof(float), 
					  sizeof(indicies_cube)/sizeof(indicies_cube[0]),
					  sizeof(int),
					  AttribPointer_cube);

	std::cout << "create Background buffers and layout!" << std::endl;
	VertexBuffer Background(background,
							back_indicies,
							sizeof(background) / sizeof(background[0]),
							sizeof(float),
							sizeof(back_indicies) / sizeof(back_indicies[0]),
							sizeof(int),
							AttribPointer_Background,
							"generate texture");

	//read file list int the folder
	std::cout << "creating image list..." << std::endl;
	std::map<std::string,int> Filelist = read_images_in_folder("D:\\autoencoder_6d_pose_estimation\\backgrounimage\\VOCdevkit\\VOC2012\\JPEGImages");
	std::map<std::string, int>::iterator it = Filelist.begin();
	std::advance(it, 1);
	
	std::cout << "image list created!" << std::endl;
	Model TrainingObject(LOAD_MODEL);
	//std::cout << "number of model meshes: " << TrainingObject.meshes[0].Vertecies.size() << std::endl;
	Model ReferenceObject(LOAD_CUBE_REFERENCE);
	if (ENABLE_USER_INPUT_TO_CONTROL_CAMERA)
	{
		glfwSetInputMode(window[0], GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		std::cout << "user Input Enabled.." << std::endl;
		std::cout << "now you can move the camera view through keys: W,A,S,D " << std::endl;
		std::cout << "mouse to zoom.." << std::endl;
	}


	Shader Simple_shader("Simple_vertex.shader","Simple_Fragment.shader");
	Shader lightning_shader("Lightning_vertex.shader", "Lightning_fragment.shader");
	Shader multiple_lightning_shader("multipleLightSource_vertex.shader","multipleLightSource_fragment.shader");
	Shader Basic_shader("Basic_vertex.shader", "Basic_Fragment.shader");
	Shader Boundingbox_8p_shader("boundingbox_8p_vertex.shader","boundingbox_8p_fragment.shader");
	///////////////////////////////////////////using bounding box/////////////////////////////////////////////////////
	BoundingBox boundingbox(TrainingObject);
	float bounding_box_vertex_8point[24] =
	{
		boundingbox.bb_v_3d.x_max, boundingbox.bb_v_3d.y_min, boundingbox.bb_v_3d.z_max,
		boundingbox.bb_v_3d.x_max, boundingbox.bb_v_3d.y_min, boundingbox.bb_v_3d.z_min,
		boundingbox.bb_v_3d.x_max, boundingbox.bb_v_3d.y_max, boundingbox.bb_v_3d.z_min,
		boundingbox.bb_v_3d.x_max, boundingbox.bb_v_3d.y_max, boundingbox.bb_v_3d.z_max,
		boundingbox.bb_v_3d.x_min, boundingbox.bb_v_3d.y_min, boundingbox.bb_v_3d.z_max,
		boundingbox.bb_v_3d.x_min, boundingbox.bb_v_3d.y_min, boundingbox.bb_v_3d.z_min,
		boundingbox.bb_v_3d.x_min, boundingbox.bb_v_3d.y_max, boundingbox.bb_v_3d.z_min,
		boundingbox.bb_v_3d.x_min, boundingbox.bb_v_3d.y_max, boundingbox.bb_v_3d.z_max
	};

	std::map<std::string, int> AttribPointer_BB;//problem by extracting to calss: pass array result in uncorrect bing VAO
	AttribPointer_BB["layout_0"] = 0;
	AttribPointer_BB["size_of_vertex_0"] = 3;
	AttribPointer_BB["stride_0"] = 3 * sizeof(float);
	AttribPointer_BB["offset_0"] = 0;
	VertexBuffer BB_3d(bounding_box_vertex_8point,
		boundingbox.bounding_box_vertex_8point_indecies,
		sizeof(bounding_box_vertex_8point) / sizeof(bounding_box_vertex_8point[0]),
		sizeof(float),
		sizeof(boundingbox.bounding_box_vertex_8point_indecies) / sizeof(boundingbox.bounding_box_vertex_8point_indecies[0]),
		sizeof(int),
		AttribPointer_BB,
		"bb");

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	lamp = glm::translate(lamp, light_position);
	lamp = glm::scale(lamp, glm::vec3(10.0f,10.0f,10.0f));
	//back_position = glm::translate(back_position, back_ground_position);

	
	GLCall(glEnable(GL_DEPTH_TEST));
	std::cout << "rendering..." << std::endl;
	bool ground_truth = false;
	std::string json_path = "label_syn.json";
	json labels;

	labels["object_id"] = path.substr(0, path.find_last_of('/'));
	
	std::ofstream jsonfile;
	jsonfile.open(json_path);

	int delta_P(5), delta_Y(5), delta_R(10);


	while (!glfwWindowShouldClose(window[0]))  //start the game
	{
		//GLCall(glViewport(0,0,1024,768));

		glm::mat4 object_model = glm::mat4(1.0f);
		glm::mat4 cube = glm::mat4(1.0f);
		glm::mat4 camera = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		object_model  = glm::translate(object_model, train_object.position);
		object_model  = glm::scale(object_model, glm::vec3(1.0f, 1.0f, 1.0f));//for nanosuits default 0.4

		cube = glm::translate(cube, glm::vec3(3.0f,0.0f,0.0f));
		cube = glm::scale(cube, glm::vec3(0.3f, 0.3f, 0.3f));//for nanosuits default 0.4

		if (ground_truth)
		{
			USE_BACKGROUND_IMAGE = false;
		}

		for (int P = 0; P <361; P+=delta_P)
		{
			//GLCall(glClearColor(0.03f, 0.05f, 0.05f, 1.0f));
			projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

			/*std::cout << "first loop:" <<P<< std::endl;
				std::cout <<" "<< back_position[0][0] <<" "<< back_position[0][1] <<" "<< back_position[0][2] <<" "<< back_position[0][3] <<" "<< std::endl;
				std::cout <<" "<<back_position[1][0] <<" "<< back_position[1][1] << " " << back_position[1][2] <<" "<< back_position[1][3] << " " << std::endl;
				std::cout << " " << back_position[2][0] << " " << back_position[2][1] << " " << back_position[2][2] << " " << back_position[2][3] << " " << std::endl;
				std::cout << " " << back_position[3][0] << " " << back_position[3][1] << " " << back_position[3][2] << " " << back_position[3][3] << " " << std::endl;
			*/
			for (int Y = 0; Y < 361; Y+=delta_Y)
			{
				std::cout << "in loop: " << Y << std::endl;
				if (ENABLE_USER_INPUT_TO_CONTROL_CAMERA)
				{
					wasd_keyinput(window[0]);
					glfwSetCursorPosCallback(window[0], mouse_callback);
					glfwSetScrollCallback(window[0], scroll_callback);
				}
				float currentFrame = glfwGetTime();
				deltaTime = currentFrame - lastFrame;
				lastFrame = currentFrame;
				if (STATIC_CAMERA_VIEW == true)
				{
					float distance = 0.5f;

					if(ROTATE_CAMERA)
						Setup=rotateCamera(P, Y, distance);

				}
				camera = glm::lookAt(Setup.camera_pose, Setup.camera_pose + Setup.camera_front, Setup.camera_up);
				//create inplane rotation
				for (int R = 0; R < 361; R+=delta_R)
				{
					camera = glm::rotate(camera, glm::radians((float)R), Setup.camera_front);
					GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
					GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
					GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));//added for object (bounding box use line)
					//use background image
					if (USE_BACKGROUND_IMAGE)
					{
						
							if(it==Filelist.end())
								it=Filelist.begin();
							//background image setting
							int background_width, background_height, nrChannels;
							stbi_set_flip_vertically_on_load(true);
							unsigned char *data = stbi_load(it->first.c_str(), &background_width, &background_height, &nrChannels, 0);//Crynet_nanosuit.jpg
																																							  //std::cout << "background image size: " << (float)background_width / background_height << std::endl;
							std::cout << it->first.c_str() << std::endl;

							try
							{
								if (data) {
									//std::cout << "loaded data: "<<*data << std::endl;
									Background.load_texture_image(GL_TEXTURE_2D,
										GL_RGB,
										background_width,
										background_height,
										0,
										GL_RGB,
										data);
									std::cout << "texture loaded" << std::endl;
								}
							}
							catch (std::exception&)
							{
								continue;
							}
							it++;
							Shader background_shader("background_vertex.shader", "background_fragment.shader");
							background_shader.use();
							background_shader.setInt("texture1", 0);
							Background.Bind("bind_Texure");
							Background.Draw("draw_elements");
							GLCall(glClear(GL_DEPTH_BUFFER_BIT));//otherweise, it would be foreground
						
							Background.UnBind();
							stbi_image_free(data);
					}

					if (ROTATE_LIGHT)
					{
						lamp=rotateLight(lamp, P, Y ,5.0f);
					}

					//rotate_object(model, 2, 0.1f);
					glm::mat4 camera_transpose = glm::transpose(camera);

					//shader_program.use();
					//shader_program.setMatrix4fv("model", model);
					//shader_program.setMatrix4fv("projection", projection);
					//shader_program.setMatrix4fv("view", camera);
					//shader_program.setVector3f("lightPos", light_position);
					//shader_program.setVector3f("viewPos", Setup.camera_pose);
					//shader_program.setVector3f("LightColor",lightning.light_color);
					//nanosuits.Draw(shader_program);


					//object pose in Camera coordinate system:
					//pose = view * model
					glm::mat4 pose = camera*object_model;//<-------------------------after add roll angle this should be modified
					float pose_array[4][4];
					convert_array(pose, pose_array);
					labels["Orientation"] = pose_array;

					jsonfile << labels;


					//std::cout << pose_array[0][0] << std::endl;

					if (!USE_SIMPLE_LIGHTNING_MODEL)
					{
						multiple_lightning_shader.use();

						multiple_lightning_shader.setMatrix4fv("view", camera);
						multiple_lightning_shader.setVector3f("viewPos", Setup.camera_pose);
						multiple_lightning_shader.setMatrix4fv("projection", projection);
						multiple_lightning_shader.setMatrix4fv("model", cube);
						
						dirLight.ambient = glm::vec3{0.15f,0.15f,0.15f};
						dirLight.diffuse= glm::vec3{0.4f,0.4f,0.4f};
						dirLight.specular = glm::vec3{0.5f,0.5f,0.5f};

						multiple_lightning_shader.setVector3f("directionlight.direction", dirLight.light_direction);
						multiple_lightning_shader.setVector3f("directionlight.ambient", dirLight.ambient);
						multiple_lightning_shader.setVector3f("directionlight.diffuse", dirLight.diffuse);
						multiple_lightning_shader.setVector3f("directionlight.specular", dirLight.specular);

						//std::cout << "number of the light sources: " << (int)((sizeof(light_positions) / sizeof(glm::vec3))) << std::endl;
						
						pointLight.ambient = glm::vec3{0.05f,0.05f,0.05f};
						pointLight.diffuse = glm::vec3{ 0.8f,0.8f,0.8f };
						pointLight.specular = glm::vec3{ 1.0f,1.0,1.0f};
						
						if (!ground_truth) 
						{
							for (int i=0; i<(int)((sizeof(light_positions)/sizeof(glm::vec3)));i++)
							{
								if (ENABLE_RANDOM_LIGHT_SOURCE_POSITION)
								{
									light_positions[i] = glm::ballRand(15.0f);
								}
								std::string number = std::to_string(i);
								std::string uniform_position = "pointlights[].position";
								std::string uniform_ambient = "pointlights[].ambient";
								std::string uniform_diffuse = "pointlights[].diffuse";
								std::string uniform_specular = "pointlights[].specular";
								std::string uniform_constantoffset = "pointlights[].constantoffset";
								std::string uniform_linearfactor = "pointlights[].linearfactor";
								std::string uniform_quadraticfactor = "pointlights[].quadraticfactor";
								uniform_position.insert(12, number);
								uniform_ambient.insert(12, number);
								uniform_diffuse.insert(12, number);
								uniform_specular.insert(12, number);
								uniform_constantoffset.insert(12, number);
								uniform_linearfactor.insert(12, number);
								uniform_quadraticfactor.insert(12, number);

								//std::cout << uniform_position << std::endl;


								multiple_lightning_shader.setVector3f(uniform_position, light_positions[i]);
								multiple_lightning_shader.setVector3f(uniform_ambient, pointLight.ambient);
								multiple_lightning_shader.setVector3f(uniform_diffuse, pointLight.diffuse);
								multiple_lightning_shader.setVector3f(uniform_specular, pointLight.specular);
								multiple_lightning_shader.setFloat(uniform_constantoffset, pointLight.constantoffset);
								multiple_lightning_shader.setFloat(uniform_linearfactor, pointLight.linearfactor);
								multiple_lightning_shader.setFloat(uniform_quadraticfactor, pointLight.quadraticfactor);

							}//<--for different light position
						}//<-- ground truth

						spotLight.ambient = glm::vec3{0.0f,0.0f,0.0f};
						spotLight.diffuse = glm::vec3{ 1.0f,1.0f,1.0f};
						spotLight.specular = glm::vec3{ 1.0f,1.0f,1.0f};


						multiple_lightning_shader.setVector3f("spotlight.position",Setup.camera_pose);
						multiple_lightning_shader.setVector3f("spotlight.direction", Setup.camera_front);
						multiple_lightning_shader.setVector3f("spotlight.ambient", spotLight.ambient);
						multiple_lightning_shader.setVector3f("spotlight.diffuse", spotLight.diffuse);
						multiple_lightning_shader.setVector3f("spotlight.specular", spotLight.specular);
						multiple_lightning_shader.setFloat("spotlight.constantoffset", spotLight.constantoffset);
						multiple_lightning_shader.setFloat("spotlight.linearfactor", spotLight.linearfactor);
						multiple_lightning_shader.setFloat("spotlight.quadraticfactor", spotLight.quadraticfactor);
						multiple_lightning_shader.setFloat("spotlight.cutoff", spotLight.cutoff);
						multiple_lightning_shader.setFloat("spotlight.outercutoff", spotLight.outercutoff);


						multiple_lightning_shader.setVector3f("material.ambient", train_object.ambient);
						multiple_lightning_shader.setVector3f("material.diffuse", train_object.diffuse);
						multiple_lightning_shader.setVector3f("material.specular", train_object.specular);
						multiple_lightning_shader.setFloat("material.shininess", train_object.shininess);
						

						multiple_lightning_shader.use();
						multiple_lightning_shader.setMatrix4fv("model", object_model);
						TrainingObject.Draw(multiple_lightning_shader);//main object for training

						//////////////////////using bounding box//////////////////////////////////////////////////////

						boundingbox.fill_bb_glm_vec3(bounding_box_vertex_8point);
						boundingbox.generate_bounding_box_labels_3d(SCR_WIDTH, SCR_HEIGHT, P, Y, R, projection, camera, object_model, jsonfile, json_path);
						std::cout << "labels generated" << std::endl;
						//3d Bounding Box
						Boundingbox_8p_shader.use();
						Boundingbox_8p_shader.setMatrix4fv("model", object_model);
						Boundingbox_8p_shader.setMatrix4fv("view", camera);
						Boundingbox_8p_shader.setMatrix4fv("projection", projection);
						GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
						BB_3d.Draw("draw_elements");

						float bounding_box_vertex_4point[] = {
							boundingbox.bb_v.x_max,	boundingbox.bb_v.y_max,	0.f,  // top right
							boundingbox.bb_v.x_max,	boundingbox.bb_v.y_min,	0.f,  // bottom right
							boundingbox.bb_v.x_min,	boundingbox.bb_v.y_min,	0.f,  // bottom left
							boundingbox.bb_v.x_min,	boundingbox.bb_v.y_max,	0.f
						};

						//2dBounding Box
						VertexBuffer BB_2d(bounding_box_vertex_4point,
							boundingbox.bounding_box_vertex_4point_indecies,
							sizeof(bounding_box_vertex_4point) / sizeof(bounding_box_vertex_4point[0]),
							sizeof(float),
							sizeof(boundingbox.bounding_box_vertex_4point_indecies) / sizeof(boundingbox.bounding_box_vertex_4point_indecies[0]),
							sizeof(int),
							AttribPointer_BB,
							"bb");

						Basic_shader.use();
						Basic_shader.setMatrix4fv("model", object_model);
						Basic_shader.setMatrix4fv("view", camera);
						Basic_shader.setMatrix4fv("projection", projection);
						GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
						BB_2d.Draw("draw_elements");

						////////////////////////////////////////////////////////////////////////////////////////



						//draw reference cube, in order to check whether imported vertex data correctly generated by blender
						if (!ground_truth)
						{
							cube = glm::mat4(1.0f);
							cube = glm::translate(cube, glm::ballRand(0.3f));
							cube = glm::scale(cube, glm::vec3(0.3f));
							multiple_lightning_shader.use();
							multiple_lightning_shader.setMatrix4fv("model", cube);
							object_setting_for_fragment_shader obstacles;
							obstacles.ambient = glm::ballRand(1.0f);
							obstacles.diffuse = glm::ballRand(1.0f);
							multiple_lightning_shader.setVector3f("material.ambient", obstacles.ambient);
							multiple_lightning_shader.setVector3f("material.diffuse", obstacles.diffuse);
							ReferenceObject.Draw(multiple_lightning_shader);//obstacles

							glm::mat4 cube2 = glm::mat4(1.0f);
							cube2 = glm::translate(cube2, glm::vec3{ 4.0f,0.0f,0.0f });
							cube2 = glm::scale(cube2, glm::vec3(0.3f));
							multiple_lightning_shader.use();
							multiple_lightning_shader.setMatrix4fv("model", cube2);
							multiple_lightning_shader.setVector3f("material.ambient", reference_object.ambient);
							multiple_lightning_shader.setVector3f("material.diffuse", reference_object.diffuse);

							Cube.Draw("draw_elements");
							
							//GLCall(glDrawArrays(GL_TRIANGLES, 0, 36);
							lightning_shader.use();
							lightning_shader.setMatrix4fv("projection_light", projection);
							lightning_shader.setMatrix4fv("view_light", camera);
							lightning_shader.setVector3f("LightColor", lightning.light_color);
							for (int i = 0; i < sizeof(light_positions) / sizeof(glm::vec3); i++)
							{
								lamp = glm::mat4(1.0f);
								lamp = glm::translate(lamp, light_positions[i]);
								lamp = glm::scale(lamp, glm::vec3{ 10.0f,10.0f,10.0f });
								lightning_shader.setMatrix4fv("model_light", lamp);
								Lightning.Draw("draw_arrays");
							}
						}//<--ground truth

						
					}//<--use multiple light source

					glm::mat4 pose_camera_view = camera * object_model;
					pose_camera_view = glm::transpose(pose_camera_view);
						//std::cout<<"pose camera view piece matrix:"<<std::endl;
						//std::cout << "	" << pose_camera_view[0][0] << "	" << pose_camera_view[0][1] << "	" << pose_camera_view[0][2] << "	" << pose_camera_view[0][3] << "	" << std::endl;
						//std::cout << "	" << pose_camera_view[1][0] << "	" << pose_camera_view[1][1] << "	" << pose_camera_view[1][2] << "	" << pose_camera_view[1][3] << "	" << std::endl;
						//std::cout << "	" << pose_camera_view[2][0] << "	" << pose_camera_view[2][1] << "	" << pose_camera_view[2][2] << "	" << pose_camera_view[2][3] << "	" << std::endl;
						//std::cout << "	" << pose_camera_view[3][0] << "		" << pose_camera_view[3][1] << "		" << pose_camera_view[3][2] << "		" << pose_camera_view[3][3] << "		" << std::endl;


					if (USE_SIMPLE_LIGHTNING_MODEL)
					{
						Simple_shader.use();
						Simple_shader.setMatrix4fv("model", object_model);
						Simple_shader.setMatrix4fv("projection", projection);
						Simple_shader.setMatrix4fv("view", camera);
						Simple_shader.setVector3f("viewPos", Setup.camera_pose);
						Simple_shader.setVector3f("material.ambient", train_object.ambient);
						Simple_shader.setVector3f("material.diffuse", train_object.diffuse);
						Simple_shader.setVector3f("material.specular", train_object.specular);
						Simple_shader.setFloat("material.shininess", train_object.shininess);

						Simple_shader.setVector3f("light.position", light_position);
						Simple_shader.setVector3f("light.ambient", lightning.ambient);
						Simple_shader.setVector3f("light.diffuse", lightning.diffuse);
						Simple_shader.setVector3f("light.specular", lightning.light_color);
						TrainingObject.Draw(Simple_shader);

						Simple_shader.use();//called every time when draw a new object with the same shader 
						Simple_shader.setMatrix4fv("model", cube);
						Simple_shader.setMatrix4fv("projection", projection);
						Simple_shader.setMatrix4fv("view", camera);
						Simple_shader.setVector3f("viewPos", Setup.camera_pose);
						Simple_shader.setVector3f("material.ambient", reference_object.ambient);
						Simple_shader.setVector3f("material.diffuse", reference_object.diffuse);
						Simple_shader.setVector3f("material.specular", reference_object.specular);
						Simple_shader.setFloat("material.shininess", reference_object.shininess);
						Simple_shader.setVector3f("light.position", light_position);
						Simple_shader.setVector3f("light.ambient", lightning.ambient);
						Simple_shader.setVector3f("light.diffuse", lightning.diffuse);
						Simple_shader.setVector3f("light.specular", lightning.light_color);
						ReferenceObject.Draw(Simple_shader);
						//GLCall(glBindVertexArray(0));

 						lightning_shader.use();
 						lightning_shader.setMatrix4fv("model_light", lamp);
 						lightning_shader.setMatrix4fv("projection_light", projection);
						lightning_shader.setMatrix4fv("view_light", camera);
						lightning_shader.setVector3f("LightColor", lightning.light_color);

						Lightning.Draw("draw_arrays");

						glm::mat4 cube2 = glm::mat4(1.0f);
						cube2 = glm::translate(cube2, glm::vec3{ 4.0f,0.0f,0.0f });
						cube2 = glm::scale(cube2, glm::vec3(5.0f));
						Simple_shader.use();
						Simple_shader.setMatrix4fv("model", cube2);
						Simple_shader.setMatrix4fv("projection", projection);
						Simple_shader.setMatrix4fv("view", camera);
						Simple_shader.setVector3f("viewPos", Setup.camera_pose);
						Simple_shader.setVector3f("material.ambient", reference_object.ambient);
						Simple_shader.setVector3f("material.diffuse", reference_object.diffuse);
						Simple_shader.setVector3f("material.specular", reference_object.specular);
						Simple_shader.setFloat("material.shininess", reference_object.shininess);
						Simple_shader.setVector3f("light.position", light_position);
						Simple_shader.setVector3f("light.ambient", lightning.ambient);
						Simple_shader.setVector3f("light.diffuse", lightning.diffuse);
						Simple_shader.setVector3f("light.specular", lightning.light_color);

						//std::cout << "length of array[]: "<<sizeof(indicies_cube) << std::endl;
						//GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_cube));
						//GLCall(glBindVertexArray(VAO_cube));
						//GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));
						Cube.Draw("draw_elements");

						lightning_shader.use();
						lightning_shader.setMatrix4fv("projection_light", projection);
						lightning_shader.setMatrix4fv("view_light", camera);
						lightning_shader.setVector3f("LightColor", lightning.light_color);
						//GLCall(glBindVertexArray(VAO_Light));

						lamp = glm::mat4(1.0f);
						lamp = glm::translate(lamp, light_position);
						lamp = glm::scale(lamp, glm::vec3{ 10.0f,10.0f,10.0f });
						lightning_shader.setMatrix4fv("model_light", lamp);
						Lightning.Draw("draw_arrays");
						//GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));


					}//<--use simple light source

					std::string number = std::to_string(Y+P*10);
					std::string picture = "E:/data/image.jpg";
					if (ground_truth)
					{
						 picture = "E:/data/image_gt.jpg";
					}
					picture.insert(13, number);
					//screenshot_freeimage(picture.c_str(), SCR_WIDTH, SCR_HEIGHT);

					GLCall(glfwSwapBuffers(window[0]));
					GLCall(glfwPollEvents());
				}//<---end loop for roll


			}//<---Y loop
		
		}//<--P loop
		jsonfile.close();

		if (ground_truth)
		{
			glfwTerminate();//destroy glcontext
			exit(EXIT_SUCCESS);

		}
		
		ground_truth = true;
		std::cout << "ground_truth: "<< ground_truth << std::endl;
	}//<--while loop
	exit(EXIT_SUCCESS);
	return 0;


	}//<--end of main

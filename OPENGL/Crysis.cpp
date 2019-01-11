//OpenGL extension
#include <GL/glew.h>

//OpenGL
#include <GLFW/glfw3.h>

//OpenGL Math
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/random.hpp>


//OpenGL mesh loader
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//image output
//#include <IL/devil_internal_exports.h>
//#include <IL/ilut.h>

#include <FreeImage.h>

//self defined headers
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "stb_image.h"
#include "Mesh.h"


//C++ basics
#include <iostream>



//Triggers and Keys
#define LOAD_MODEL "mesh/nanosuit/chess/queen.obj"
#define LOAD_CUBE_REFERENCE "mesh/nanosuit/chess/test/untitled.obj"
#define USE_BACKGROUND_IMAGE true
#define ROTATE_CAMERA true
#define ENABLE_RANDOM_LIGHT_SOURCE_POSITION true
#define USE_SIMPLE_LIGHTNING_MODEL false
bool STATIC_CAMERA_VIEW = true;
bool ENABLE_USER_INPUT_TO_CONTROL_CAMERA = !STATIC_CAMERA_VIEW;
bool ROTATE_LIGHT = false;

//parameters
//Screen Parameters:
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;
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


struct light 
{
	glm::vec3 light_color = {1.0f,1.0f,1.0f};
	glm::vec3 light_direction = { -0.2,-1.0f,-0.3f };
	glm::vec3 light_position = {5.0f,0.0f,2.0f};

	glm::vec3 ambient = { 0.2f,0.2f,0.2f };
	glm::vec3 diffuse = {0.1f,0.1f,0.1f};
	glm::vec3 specular = light_color;

	float constantoffset = 1.0f;
	float linearfactor = 0.09f;
	float quadraticfactor = 0.032f;

	float cutoff = glm::cos(glm::radians(12.5f));
	float outercutoff = glm::cos(glm::radians(15.0f));
} dirLight, pointLight, spotLight,lightning;

glm::vec3 light_positions[] =
{	glm::ballRand(10.0),
	glm::ballRand(15.0),
	glm::ballRand(20.0),
	glm::ballRand(50.0)
};


struct Object
{
	//default white plastic
	glm::vec3 color{1.0f,1.0f,1.0f};
	glm::vec3 position{0.0f,0.0f,0.0f};
	glm::vec3 ambient{1.0f,1.0f,1.0f};
	glm::vec3 diffuse{0.55f,0.55f,0.55f};
	glm::vec3 specular{0.7f,0.7f,0.7f};
	float shininess = 15.0f;
}train_object,reference_object;


/*float cube_vertex[] = {
			   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

			   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
				0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

			   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

				0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
				0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
				0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
				0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
				0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

			   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
				0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
				0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
				0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

			   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
				0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};*/

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
struct CameraOrientation
{
	glm::vec3 camera_pose = glm::vec3{ 0.0f,10.0f,20.0f };
	glm::vec3 camera_front = glm::vec3{ 0.0f,0.0f,0.0f }-camera_pose;//the target camera look at - camera position
	glm::vec3 camera_up = glm::vec3{ 0.0f,0.1f,0.0f };

};

CameraOrientation Setup;

//rotate camera the function should be used in two for loop, which loop through the Yaw and Pitch angle
CameraOrientation rotateCamera(int P, int Y,float distance)
{
	//std::cout << "Camera rotation enabled!" << std::endl;

	CameraOrientation setup;
	//std::cout << "Y= " << Y << std::endl;
	float x_direction = distance * glm::cos(glm::radians((float)P))*cos(glm::radians((float)Y));
	float y_direction = distance * glm::sin(glm::radians((float)P));
	float z_direction = -distance * glm::cos(glm::radians((float)P))*sin(glm::radians((float)Y));

	setup.camera_pose = glm::vec3(x_direction, y_direction, z_direction);
	glm::vec3 camera_pose_xz = glm::vec3(setup.camera_pose.x, 0.0f, setup.camera_pose.z);
	setup.camera_front = -setup.camera_pose;
	setup.camera_up;

	if ((0 < P&&P <= 90) || (180 <= P && P <= 270) || P == 360)
	{
		setup.camera_up = glm::normalize(glm::cross(glm::cross(setup.camera_pose, camera_pose_xz), setup.camera_front));
		//std::cout << "0<P<=90" << std::endl;
	}

	if ((90 < P&&P < 180) || (270 < P&&P < 360)||P==0)
	{
		setup.camera_up = glm::normalize(glm::cross(glm::cross(setup.camera_pose, camera_pose_xz), setup.camera_pose));
		//std::cout << "90<P<=180" << std::endl;
	}



	return setup;
}


glm::mat4 rotateLight(glm::mat4 light_model, int P, int Y, float distance)
{
	float x_direction = distance * glm::cos(glm::radians((float)P))*cos(glm::radians((float)Y));
	float y_direction = distance * glm::sin(glm::radians((float)P));
	float z_direction = -distance * glm::cos(glm::radians((float)P))*sin(glm::radians((float)Y));
	
	light_position = glm::vec3{ x_direction,y_direction,z_direction };
	
	light_model = glm::translate(light_model, glm::vec3{x_direction,y_direction,z_direction});
	return light_model;
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


//void takeScreenshot(const char* screenshotFile)
//{
//	ILuint imageID = ilGenImage();
//	ilBindImage(imageID);
//	ilutGLScreen();
//	ilEnable(IL_FILE_OVERWRITE);
//	ilSaveImage(screenshotFile);
//	ilDeleteImage(imageID);
//	//printf("Screenshot saved to: %s\n", screenshotFile);
//}

void screenshot_freeimage(const char* screenshotFile, int width, int height ) {

	// Make the BYTE array, factor of 3 because it's RBG.
	BYTE* pixels = new BYTE[3 * width * height];

	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	// Convert to FreeImage format & save to file
	FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
	FreeImage_Save(FIF_BMP, image, screenshotFile, 0);

	// Free resources
	FreeImage_Unload(image);
	delete[] pixels;
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
		 1.0f,	1.0f,	0.0f, 1.0f, 1.0f, // top right
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
	0,1,3,
	1,2,3
};


//TODO:
//1. set pictures as the background of the window
//2. implement the split window to show both the rendered data and the ground truth data
//3. optimize all of the VAO and VBOs


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

// 	//setting up the second window
// 	if (!window[1])
// 	{
// 		glfwTerminate();
// 		exit(EXIT_FAILURE);
// 	}
// 	else
// 	{
// 		std::cout << "Window creation succeed!" << std::endl;
// 	}
// 
// 	glfwMakeContextCurrent(window[1]);
// 	//glfwSetWindowSizeCallback(window[1], window_size_callback);
// 	std::cout << "use ESC to exit the Window" << std::endl;
// 	glfwSetKeyCallback(window[1], key_callback);
// 	glfwSetWindowPos(window[1], 1524, 500);
// 	if (glewInit() != GLEW_OK)
// 	{
// 		std::cout << "initialize glew failed" << std::endl;
// 	}
// 	else
// 	{
// 		std::cout << "glew initialization succeed!" << std::endl;
// 	}
// 
// 
// 	glfwMakeContextCurrent(window[0]);


		unsigned int VAO_Light, VBO_Light;
		std::cout << "create Light buffers and layout!" << std::endl;
		GLCall(glGenVertexArrays(1, &VAO_Light));
		GLCall(glGenBuffers(1, &VBO_Light));
		GLCall(glBindVertexArray(VAO_Light));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_Light));
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLight), verticesLight, GL_STATIC_DRAW));
		//VertexBuffer light(verticesLight,sizeof(verticesLight)*sizeof(float));
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glBindVertexArray(0));

		unsigned int VAO_cube, VBO_cube,EBO_cube;
		GLCall(glGenVertexArrays(1,&VAO_cube));
		GLCall(glGenBuffers(1,&VBO_cube));
		GLCall(glGenBuffers(1, &EBO_cube));

		GLCall(glBindVertexArray(VAO_cube));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER,VBO_cube));
		GLCall(glBufferData(GL_ARRAY_BUFFER,sizeof(cube_vertex),cube_vertex,GL_STATIC_DRAW));
		
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO_cube));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies_cube), indicies_cube, GL_STATIC_DRAW));

		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))));
		GLCall(glEnableVertexAttribArray(1));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER,0));
		GLCall(glBindVertexArray(0));


		std::cout << "create Background buffers and layout!" << std::endl;
		unsigned int VAO_Background, VBO_Background, EBO_Background;
		GLCall(glGenVertexArrays(1, &VAO_Background));
		GLCall(glGenBuffers(1, &VBO_Background));
		GLCall(glGenBuffers(1, &EBO_Background));

		GLCall(glBindVertexArray(VAO_Background));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Background));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(back_indicies), back_indicies, GL_STATIC_DRAW));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_Background));
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(background), background, GL_STATIC_DRAW));
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
		GLCall(glEnableVertexAttribArray(1));
		unsigned int BK1;
		GLCall(glGenTextures(1, &BK1));
		GLCall(glBindTexture(GL_TEXTURE_2D, BK1));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		//background image setting
		if (USE_BACKGROUND_IMAGE)
		{
			int background_width, background_height, nrChannels;
			stbi_set_flip_vertically_on_load(true);
			unsigned char *data = stbi_load("Crynet_nanosuit.jpg", &background_width, &background_height, &nrChannels, 0);//Crynet_nanosuit.jpg

			if (data)
			{
				GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, background_width, background_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
				GLCall(glGenerateMipmap(GL_TEXTURE_2D));

			}
			else
			{
				std::cout << "failed to load background image" << std::endl;
			}
			stbi_image_free(data);
		}


			Shader background_shader("background_vertex.shader", "background_fragment.shader");
			background_shader.use();
			background_shader.setInt("texture1", 0);




		//Model nanosuits(LOAD_MODEL);//untitled.obj
		Model TrainingObject(LOAD_MODEL);
		Model ReferenceObject(LOAD_CUBE_REFERENCE);
		if (ENABLE_USER_INPUT_TO_CONTROL_CAMERA)
		{
			glfwSetInputMode(window[0], GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			std::cout << "user Input Enabled.." << std::endl;
			std::cout << "now you can move the camera view through keys: W,A,S,D " << std::endl;
			std::cout << "mouse to zoom.." << std::endl;
		}

		//old shader for nanosuits
		//Shader shader_program("VertexShader.shader", "FragmentShader.shader");

		Shader Simple_shader("Simple_vertex.shader","Simple_Fragment.shader");

		Shader lightning_shader("Lightning_vertex.shader", "Lightning_fragment.shader");

		Shader multiple_lightning_shader("multipleLightSource_vertex.shader","multipleLightSource_fragment.shader");



		lamp = glm::translate(lamp, light_position);
		lamp = glm::scale(lamp, glm::vec3(10.0f,10.0f,10.0f));
		//back_position = glm::translate(back_position, back_ground_position);

		
		GLCall(glEnable(GL_DEPTH_TEST));
		std::cout << "rendering..." << std::endl;
		while (!glfwWindowShouldClose(window[0]))  //start the game
		{
			//GLCall(glViewport(0,0,1024,768));

			glm::mat4 chess_piece = glm::mat4(1.0f);
			glm::mat4 cube = glm::mat4(1.0f);
			glm::mat4 camera = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);
			chess_piece  = glm::translate(chess_piece, train_object.position);
			chess_piece  = glm::scale(chess_piece, glm::vec3(14.0f, 14.0f, 14.0f));//for nanosuits default 0.4

			cube = glm::translate(cube, glm::vec3(3.0f,0.0f,0.0f));
			cube = glm::scale(cube, glm::vec3(5.0f, 5.0f, 5.0f));//for nanosuits default 0.4




			for (int P = 0; P < 361; P++)
			{
				//GLCall(glClearColor(0.03f, 0.05f, 0.05f, 1.0f));
				projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

				/*std::cout << "first loop:" <<P<< std::endl;
				std::cout <<" "<< back_position[0][0] <<" "<< back_position[0][1] <<" "<< back_position[0][2] <<" "<< back_position[0][3] <<" "<< std::endl;
				std::cout <<" "<<back_position[1][0] <<" "<< back_position[1][1] << " " << back_position[1][2] <<" "<< back_position[1][3] << " " << std::endl;
				std::cout << " " << back_position[2][0] << " " << back_position[2][1] << " " << back_position[2][2] << " " << back_position[2][3] << " " << std::endl;
				std::cout << " " << back_position[3][0] << " " << back_position[3][1] << " " << back_position[3][2] << " " << back_position[3][3] << " " << std::endl;
*/
				for (int Y = 0; Y < 361; Y++)
				{
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
						float distance = 20.0f;

						if(ROTATE_CAMERA)
							Setup=rotateCamera(P, Y, distance);

					}
					camera = glm::lookAt(Setup.camera_pose, Setup.camera_pose + Setup.camera_front, Setup.camera_up);


					GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
					GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

					//use background image
					if (USE_BACKGROUND_IMAGE)
					{
						GLCall(glActiveTexture(GL_TEXTURE0));
						GLCall(glBindTexture(GL_TEXTURE_2D,BK1));
						GLCall(glBindVertexArray(VAO_Background));
						background_shader.use();
						/*background_shader.setInt("texture1", 0);
						background_shader.setMatrix4fv("model_back", back_position);
						background_shader.setMatrix4fv("view", camera);*/

						GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

						GLCall(glClear(GL_DEPTH_BUFFER_BIT));

					}

					if (ROTATE_LIGHT)
					{
						lamp=rotateLight(lamp, P, Y ,5.0f);
					}

					//rotate_object(model, 2, 0.1f);
					/*std::cout<<"second loop:"<<std::endl;
					std::cout << " " << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << " " << std::endl;
					std::cout << " " << model[1][0] << " " << model[1][1] << " " << model[1][2] << " " << model[1][3] << " " << std::endl;
					std::cout << " " << model[2][0] << " " << model[2][1] << " " << model[2][2] << " " << model[2][3] << " " << std::endl;
					std::cout << " " << model[3][0] << " " << model[3][1] << " " << model[3][2] << " " << model[3][3] << " " << std::endl;*/
					//shader_program.use();
					//shader_program.setMatrix4fv("model", model);
					//shader_program.setMatrix4fv("projection", projection);
					//shader_program.setMatrix4fv("view", camera);
					//shader_program.setVector3f("lightPos", light_position);
					//shader_program.setVector3f("viewPos", Setup.camera_pose);
					//shader_program.setVector3f("LightColor",lightning.light_color);
					//nanosuits.Draw(shader_program);

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

						}

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
						multiple_lightning_shader.setMatrix4fv("model", chess_piece);
						TrainingObject.Draw(multiple_lightning_shader);//main object for training

						cube = glm::mat4(1.0f);
						cube = glm::translate(cube, glm::ballRand(5.0f));
						cube = glm::scale(cube, glm::vec3(5.0f));
						multiple_lightning_shader.use();
						multiple_lightning_shader.setMatrix4fv("model", cube);
						Object obstacles;
						obstacles.ambient = glm::ballRand(1.0f);
						obstacles.diffuse = glm::ballRand(1.0f);
						multiple_lightning_shader.setVector3f("material.ambient", obstacles.ambient);
						multiple_lightning_shader.setVector3f("material.diffuse", obstacles.diffuse);
						ReferenceObject.Draw(multiple_lightning_shader);//obstacles


						//draw reference cube, in order to check whether imported vertex data correctly generated by blender
						glm::mat4 cube2 = glm::mat4(1.0f);
						cube2 = glm::translate(cube2, glm::vec3{ 4.0f,0.0f,0.0f });
						cube2 = glm::scale(cube2, glm::vec3(5.0f));
						multiple_lightning_shader.use();
						multiple_lightning_shader.setMatrix4fv("model", cube2);
						multiple_lightning_shader.setVector3f("material.ambient", reference_object.ambient);
						multiple_lightning_shader.setVector3f("material.diffuse", reference_object.diffuse);

						//std::cout << "length of array[]: "<<sizeof(indicies_cube) << std::endl;
						GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_cube));
						GLCall(glBindVertexArray(VAO_cube));
						GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));
						//GLCall(glDrawArrays(GL_TRIANGLES, 0, 36);

						lightning_shader.use();
						lightning_shader.setMatrix4fv("projection_light", projection);
						lightning_shader.setMatrix4fv("view_light", camera);
						lightning_shader.setVector3f("LightColor", lightning.light_color);
						GLCall(glBindVertexArray(VAO_Light));
						for (int i = 0; i < sizeof(light_positions) / sizeof(glm::vec3); i++)
						{
							lamp = glm::mat4(1.0f);
							lamp = glm::translate(lamp, light_positions[i]);
							lamp = glm::scale(lamp, glm::vec3{ 10.0f,10.0f,10.0f });
							lightning_shader.setMatrix4fv("model_light", lamp);
							GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
						}

					}



					if (USE_SIMPLE_LIGHTNING_MODEL)
					{
						Simple_shader.use();
						Simple_shader.setMatrix4fv("model", chess_piece);
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

						GLCall(glBindVertexArray(VAO_Light));
						GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
						GLCall(glBindVertexArray(0));

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
						GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_cube));
						GLCall(glBindVertexArray(VAO_cube));
						GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));

						lightning_shader.use();
						lightning_shader.setMatrix4fv("projection_light", projection);
						lightning_shader.setMatrix4fv("view_light", camera);
						lightning_shader.setVector3f("LightColor", lightning.light_color);
						GLCall(glBindVertexArray(VAO_Light));

						lamp = glm::mat4(1.0f);
						lamp = glm::translate(lamp, light_position);
						lamp = glm::scale(lamp, glm::vec3{ 10.0f,10.0f,10.0f });
						lightning_shader.setMatrix4fv("model_light", lamp);
						GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));


					}

// 					iluInit();
// 					ilInit();

					std::string number = std::to_string(Y+P*10);
					std::string picture = "E:/data/image.png";
					picture.insert(13, number);
					//takeScreenshot(picture.c_str());
					screenshot_freeimage(picture.c_str(), SCR_WIDTH, SCR_HEIGHT);

					GLCall(glfwSwapBuffers(window[0]));
					GLCall(glfwPollEvents());

				}//<---Y loop
			
			}//<--P loop
			glfwTerminate();//destroy glcontext

		}//<--while loop
		exit(EXIT_SUCCESS);
		return 0;


	}//<--end of main

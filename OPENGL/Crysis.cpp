

//OpenGL extension
#include <GL/glew.h>

//OpenGL
#include <GLFW/glfw3.h>

//OpenGL Math
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

//OpenGL mesh loader
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//self defined headers
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "stb_image.h"
#include "Mesh.h"


//C++ basics
#include <iostream>


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime(0.0f), lastFrame(0.0f);

glm::vec3 camera_pose = glm::vec3(0.0f, 3.0f, 25.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse(true);
double lastX(SCR_WIDTH / 2), lastY(SCR_HEIGHT / 2);
float yaw(-90.0f), pitch(0.0f), fov(45.0f);

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
	camera_front = glm::normalize(front);

}


void wasd_keyinput(GLFWwindow* window)
{
	float camera_speed = 2.0f*deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera_pose += camera_speed * camera_front;
		//std::cout << "w pressed" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera_pose -= camera_speed * camera_front;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera_pose -= camera_speed * glm::normalize(glm::cross(camera_front, camera_up));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera_pose += camera_speed * glm::normalize(glm::cross(camera_front, camera_up));
	}
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

}




int main()
{
	//0.create window====================================================================
	GLFWwindow* window;

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//Why it doesn't work with 2,3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//what does this influence
											//if core_profile is chosen, the vertex array object need to be manually created
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TEST", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "initialize glew failed" << std::endl;
	}

	int nrAttributes;
	unsigned int vao, vertex_shader, fragment_shader, program;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	//std::cout << "maximum number of vertex attributes supported: " << nrAttributes << std::endl;

	Model nanosuits("mesh/nanosuit/untitled.obj");



	//5. Rotation and Translation
		/*glm::mat4 projs;
		projs = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT,0.1f,100.0f);*/
		//trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
	//create camera coordinate:
	//float radious = 10.0f, camX,camZ;

//create mouse cursor input

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//4.create shader
	Shader shader_program("VertexShader.shader", "FragmentShader.shader");

	shader_program.use();
	//shader_program.setMatrix4fv("view", view);

	//glfwSwapInterval(1);
	GLCall(glEnable(GL_DEPTH_TEST));
	//5. while loop	
	while (!glfwWindowShouldClose(window))
	{



		//std::cout << "System Time: " << (float)glfwGetTime() << std::endl;
		//std::cout << "Size of CubePosition[]: " << sizeof(CubePosition)<< std::endl;

		//wasd_keyinput(window);
		//glfwSetCursorPosCallback(window, mouse_callback);
		//glfwSetScrollCallback(window, scroll_callback);


		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glm::mat4 model, camera, projection;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		for(int P = 0; P<360.0f;P++)
		{
			//GLCall(glClearColor(0.03f, 0.05f, 0.05f, 1.0f));
			camera = glm::lookAt(camera_pose, camera_pose + camera_front, camera_up);
			projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

			model = glm::rotate(model, glm::radians(1.0f), glm::vec3(1.0f,0.0f,0.0f));
			
			std::cout << "first loop:" <<P<< std::endl;
			//std::cout <<" "<< model[0][0] <<" "<< model[0][1] <<" "<< model[0][2] <<" "<< model[0][3] <<" "<< std::endl;
			//std::cout <<" "<<model[1][0] <<" "<< model[1][1] << " " << model[1][2] <<" "<< model[1][3] << " " << std::endl;
			//std::cout << " " << model[2][0] << " " << model[2][1] << " " << model[2][2] << " " << model[2][3] << " " << std::endl;
			//std::cout << " " << model[3][0] << " " << model[3][1] << " " << model[3][2] << " " << model[3][3] << " " << std::endl;
		
			for (int Y = 0;Y< 360;Y++)
			{

				GLCall(glClearColor(0.7f, 0.7f, 0.7f, 1.0f));
				GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
				model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				/*std::cout<<"second loop:"<<std::endl;
				std::cout << " " << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << " " << std::endl;
				std::cout << " " << model[1][0] << " " << model[1][1] << " " << model[1][2] << " " << model[1][3] << " " << std::endl;
				std::cout << " " << model[2][0] << " " << model[2][1] << " " << model[2][2] << " " << model[2][3] << " " << std::endl;
				std::cout << " " << model[3][0] << " " << model[3][1] << " " << model[3][2] << " " << model[3][3] << " " << std::endl;*/
				shader_program.use();
				shader_program.setMatrix4fv("model", model);
				shader_program.setMatrix4fv("projection", projection);
				shader_program.setMatrix4fv("view", camera);

				nanosuits.Draw(shader_program);
				GLCall(glfwSwapBuffers(window));
				GLCall(glfwPollEvents());

			}
		}
		
		//GLCall(glDrawElements(GL_TRIANGLES, 6,GL_UNSIGNED_INT, 0));

	}
	glfwTerminate();//destroy glcontext
	exit(EXIT_SUCCESS);
	return 0;




}
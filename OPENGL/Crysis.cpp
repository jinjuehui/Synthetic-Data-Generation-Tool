

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


const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

float deltaTime(0.0f), lastFrame(0.0f);//now the variables are only used for keyboard input callback functions



bool firstMouse(true);
double lastX(SCR_WIDTH / 2), lastY(SCR_HEIGHT / 2);
float yaw(-90.0f), pitch(0.0f), fov(45.0f);



void rotate_object(glm::mat4 &model, int axis, float velocity)
{
	//model = glm::rotate(model, glm::radians(velocity), glm::vec3(0.0f, 0.0f, 0.0f));

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
//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//	if (firstMouse)
//	{
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos;
//	lastX = xpos;
//	lastY = ypos;
//
//	float sensitivity = 0.05;
//	xoffset *= sensitivity;
//	yoffset *= sensitivity;
//
//	yaw += xoffset;
//	pitch += yoffset;
//
//	if (pitch > 89.0f)
//	{
//		pitch = 89.0f;
//	}
//	if (pitch < -89.0f)
//	{
//		pitch = -89.0f;
//	}
//
//	glm::vec3 front;
//	front.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
//	front.y = sin(glm::radians(pitch));
//	front.z = sin(glm::radians(yaw))*cos(glm::radians(pitch));
//	camera_front = glm::normalize(front);
//
//}

//move the camera forward, backward, sideways
//void wasd_keyinput(GLFWwindow* window)
//{
//	float camera_speed =0.8f*deltaTime;
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//	{
//		camera_pose += camera_speed * camera_front;
//	}
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//	{
//		camera_pose -= camera_speed * camera_front;
//	}
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//	{
//		camera_pose -= camera_speed * glm::normalize(glm::cross(camera_front, camera_up));
//	}
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//	{
//		camera_pose += camera_speed * glm::normalize(glm::cross(camera_front, camera_up));
//	}
//}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

}

float verticesLight[] = {
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f
};


float background[] = {

	-1.0f, -1.0f, 0.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, 0.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, 0.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, 0.0f,  1.0f,  1.0f
};

unsigned int indicies[]
{
	0,1,2,
	2,3,1
};


//TODO:
//1. set pictures as the background of the window
//2. implement the split window to show both the rendered data and the ground truth data
//3. optimize all of the VAO and VBOs

int main()
{
	//0.create window====================================================================
	GLFWwindow* window;

	//initialize glfw
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//Why it doesn't work with 2,3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//if core_profile is chosen, the vertex array object need to be manually created
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Nanosuits", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "initialize glew failed" << std::endl;
	}

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);



	unsigned int VAO_Light,VBO_Light;
	GLCall(glGenVertexArrays(1,&VAO_Light));
	GLCall(glGenBuffers(1,&VBO_Light));
	GLCall(glBindVertexArray(VAO_Light));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER,VBO_Light));
	GLCall(glBufferData(GL_ARRAY_BUFFER,sizeof(verticesLight),verticesLight,GL_STATIC_DRAW));
	//VertexBuffer light(verticesLight,sizeof(verticesLight)*sizeof(float));
	GLCall(glVertexAttribPointer(0,3, GL_FLOAT,GL_FLAT,3*sizeof(float),(void*)0));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glBindVertexArray(0));

	unsigned int VAO_Background, VBO_Background,EBO_Background;
	GLCall(glGenVertexArrays(1, &VAO_Background));
	GLCall(glGenBuffers(1, &VBO_Background));
	GLCall(glGenBuffers(1, &EBO_Background));
	GLCall(glBindVertexArray(VAO_Background));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Background));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_Background));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(background),background,GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float))));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
	
	unsigned int BK1;
	GLCall(glGenTextures(1,&BK1));
	GLCall(glBindTexture(GL_TEXTURE_2D, BK1));
	GLCall(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR));
	
	int background_width, background_height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("Crynet_nanosuit.jpg",&background_width,&background_height,&nrChannels,0);

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

	Shader background_shader("background_vertex.shader","background_fragment.shader");
	background_shader.setInt("texture1", 0);



	Model nanosuits("mesh/nanosuit/untitled.obj");

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Shader shader_program("VertexShader.shader", "FragmentShader.shader");
	Shader lightning_shader("Lightning_vertex.shader","Lightning_fragment.shader");

	GLCall(glEnable(GL_DEPTH_TEST));
	
	glm::mat4 lamp;
	glm::vec3 light_position(2.0f, 0.0f, 2.0f);
	
	lamp = glm::translate(lamp, light_position);



	while (!glfwWindowShouldClose(window))  //start the game
	{

		glm::mat4 model, camera, projection;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));


		for(int P = 0; P<360.0f;P++)
		{
		


			//GLCall(glClearColor(0.03f, 0.05f, 0.05f, 1.0f));
			projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

			glfwSetScrollCallback(window, scroll_callback); 

				/*wasd_keyinput(window);
				glfwSetCursorPosCallback(window, mouse_callback);
				glfwSetScrollCallback(window, scroll_callback);*/
			//std::cout << "first loop:" <<P<< std::endl;
			//std::cout <<" "<< model[0][0] <<" "<< model[0][1] <<" "<< model[0][2] <<" "<< model[0][3] <<" "<< std::endl;
			//std::cout <<" "<<model[1][0] <<" "<< model[1][1] << " " << model[1][2] <<" "<< model[1][3] << " " << std::endl;
			//std::cout << " " << model[2][0] << " " << model[2][1] << " " << model[2][2] << " " << model[2][3] << " " << std::endl;
			//std::cout << " " << model[3][0] << " " << model[3][1] << " " << model[3][2] << " " << model[3][3] << " " << std::endl;
		
			for (int Y = 0;Y< 360;Y++)
			{
				
				GLCall(glActiveTexture(GL_TEXTURE0));
				GLCall(glBindTexture(GL_TEXTURE_2D,BK1));
				GLCall(background_shader.use());
				GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_Background));
				GLCall(glBindVertexArray(VAO_Background));
				GLCall(glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0));

				
				
				float distance(20.0f);
				glm::vec3 camera_pose = glm::vec3(distance*glm::cos(glm::radians((float)P))*cos(glm::radians((float)Y)), distance*glm::sin(glm::radians((float)P)), distance*glm::cos(glm::radians((float)P))*sin(glm::radians((float)Y)));
				glm::vec3 camera_pose_xz = glm::vec3(camera_pose.x, 0.0f, camera_pose.z);
				glm::vec3 camera_front = -camera_pose;
				glm::vec3 camera_up =- glm::cross( glm::cross(camera_pose,camera_pose_xz),camera_pose);
				glm::mat4 camera_model = glm::translate(camera_model, camera_pose);

				camera = glm::lookAt(camera_pose, camera_pose + camera_front, camera_up);

				float currentFrame = glfwGetTime();
				deltaTime = currentFrame - lastFrame;
				lastFrame = currentFrame;


				GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
				GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
				//rotate_object(model, 2, 0.1f);
				/*std::cout<<"second loop:"<<std::endl;
				std::cout << " " << model[0][0] << " " << model[0][1] << " " << model[0][2] << " " << model[0][3] << " " << std::endl;
				std::cout << " " << model[1][0] << " " << model[1][1] << " " << model[1][2] << " " << model[1][3] << " " << std::endl;
				std::cout << " " << model[2][0] << " " << model[2][1] << " " << model[2][2] << " " << model[2][3] << " " << std::endl;
				std::cout << " " << model[3][0] << " " << model[3][1] << " " << model[3][2] << " " << model[3][3] << " " << std::endl;*/
				shader_program.use();
				shader_program.setMatrix4fv("model", model);
				shader_program.setMatrix4fv("projection", projection);
				shader_program.setMatrix4fv("view", camera);
				shader_program.setVector3f("lightPos", light_position);
				shader_program.setVector3f("viewPos",camera_pose);
				nanosuits.Draw(shader_program);
				GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
				GLCall(glBindVertexArray(0));

				GLCall(glUseProgram(0));
				lightning_shader.use();
				lightning_shader.setMatrix4fv("model_light", lamp);
				lightning_shader.setMatrix4fv("projection_light", projection);
				lightning_shader.setMatrix4fv("view_light", camera);
			
				GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO_Light));
				GLCall(glBindVertexArray(VAO_Light));
				GLCall(glDrawArrays(GL_TRIANGLES,0,36));
				GLCall(glfwSwapBuffers(window));
				GLCall(glfwPollEvents());

			}
		}

	}
	glfwTerminate();//destroy glcontext
	exit(EXIT_SUCCESS);
	return 0;


}
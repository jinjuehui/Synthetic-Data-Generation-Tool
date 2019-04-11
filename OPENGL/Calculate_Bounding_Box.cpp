//OpenGL extension
#include <GL/glew.h>

//OpenGL
#include <GLFW/glfw3.h>
#include <glm.hpp>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/random.hpp>
#include <algorithm>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "stb_image.h"
#include "Mesh.h"



//camera setup with default parameters
struct CameraOrientation
{
	glm::vec3 camera_pose = glm::vec3{ 0.0f,0.10f,20.0f };//{ 0.0f, 10.0f, 20.0f };
	glm::vec3 camera_front = glm::vec3{ 0.0f,0.0f,0.0f }-camera_pose;//the target camera look at - camera position
	glm::vec3 camera_up = glm::vec3{ 0.0f,1.0f,0.0f };

}CameraSetup;

struct Object  //object rendering params related to lightning and materials
{
	//default white plastic
	glm::vec3 color{ 1.0f,1.0f,1.0f };
	glm::vec3 position{ 0.0f,0.0f,0.0f };
	glm::vec3 ambient{ 1.0f,1.0f,1.0f };
	glm::vec3 diffuse{ 0.55f,0.55f,0.55f };
	glm::vec3 specular{ 0.7f,0.7f,0.7f };
	float shininess = 15.0f;
}train_object;

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
}lightning;

glm::vec3 light_position(10.0f, 10.0f, 2.0f);


//rotate camera the function should be used in two for loop, which loop through the Yaw and Pitch angle
CameraOrientation rotateCamera(int P, int Y, float distance)
{
	//std::cout << "Camera rotation enabled!" << std::endl;

	CameraOrientation setup;
	//std::cout << "Y= " << Y << std::endl;
	float x_direction = distance * glm::cos(glm::radians((float)P)) * cos(glm::radians((float)Y));
	float y_direction = distance * glm::sin(glm::radians((float)P));
	float z_direction = distance * glm::cos(glm::radians((float)P)) * sin(glm::radians((float)Y));

	setup.camera_pose = glm::vec3(x_direction, y_direction, z_direction);
	std::cout << "camera_pose: " <<setup.camera_pose[0]<< " " <<setup.camera_pose[1] << " " << setup.camera_pose[2] << " " << std::endl;
	glm::vec3 camera_pose_xz = glm::vec3(setup.camera_pose.x, 0.0f, setup.camera_pose.z);
	setup.camera_front =glm::normalize(-setup.camera_pose);
	std::cout << "camera_front: " << setup.camera_front[0] << " " << setup.camera_front[1] << " " << setup.camera_front[2] << " " << std::endl;
	setup.camera_up;

	if (P == 0)
		setup.camera_up = glm::vec3(0.0f,1.0f,0.0f);

	if ((0 < P&&P <= 90) || (180 <= P && P <= 270) || P == 360)
	{
		setup.camera_up = glm::normalize(glm::cross(glm::cross(setup.camera_pose, camera_pose_xz), glm::normalize(setup.camera_front)));
		std::cout << "camera up: " <<setup.camera_up[0]<< " " << setup.camera_up[1]<< " " <<setup.camera_up[1] << std::endl;
	}

	if ((90 < P&&P < 180) || (270 < P&&P < 360))
	{
		setup.camera_up = glm::normalize(glm::cross(glm::cross(setup.camera_pose, camera_pose_xz), glm::normalize(setup.camera_pose)));
		std::cout << "camera up: " << setup.camera_up[0] << " " << setup.camera_up[1] << " " << setup.camera_up[1] << std::endl;
	}

	return setup;
}

GLFWwindow* initialize_window(int width, int height, const char* name)
{
	GLFWwindow* window;
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	window = glfwCreateWindow(width,height,name,NULL,NULL);

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "Window creation succeed!" << std::endl;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << "initialize glew failed" << std::endl;
	else
		std::cout << "glew initialization succeed!" << std::endl;

	GLCall(glViewport(0, 0, width, height));
	return window;
}

int main()
{

	const unsigned int SCR_WIDTH = 1000;
	const unsigned int SCR_HEIGHT = 1000;
	GLFWwindow* window = initialize_window(SCR_WIDTH, SCR_HEIGHT, "Bounding Box");

	Shader Simple_shader("Simple_vertex.shader", "Simple_Fragment.shader");

	glm::mat4 model(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model,glm::vec3(1.0f));
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);


	Model TrainingObject("mesh/obj_05.stl");
	std::cout << "finish loading model" << std::endl;

	std::vector<glm::vec3> pointset_on_image;
	glm::mat4 camera = glm::mat4(1.0f);

	int P=0, Y=0, R=0;

	GLCall(glEnable(GL_DEPTH_TEST));

	unsigned int BB_indicies[] =
	{
		0,1,2,
		3,0,2
	};
	
	Shader Basic_shader("Basic_vertex.shader", "Basic_Fragment.shader");
	while (!glfwWindowShouldClose(window))
	{


		for (int P = 0; P <361; P++)
		{	
			std::cout << "P: "<< P << std::endl;
			for (int Y = 0; Y < 361; Y += 5)
			{
				//std::cout << "P: " << Y << std::endl;

				float distance = 0.2f;
				CameraSetup = rotateCamera(P, Y, distance);
				//std::cout << "camera position" << CameraSetup.camera_pose[0] << " " << CameraSetup.camera_pose[1] << " " << CameraSetup.camera_pose[2] << std::endl;
				camera = glm::lookAt(CameraSetup.camera_pose, glm::normalize(CameraSetup.camera_pose) + glm::normalize(CameraSetup.camera_front), CameraSetup.camera_up);
				glm::mat4 camera_t = glm::transpose(camera);
				//std::cout << "projection: " << projection[0][0] << "	" << projection[0][1] << "	" << projection[0][2] << "	" <<projection[0][3] << std::endl;
				//std::cout << "projection: " << projection[1][0] << "	" << projection[1][1] << "	" << projection[1][2] << "	" <<projection[1][3] <<std::endl;
				//std::cout << "projection: " << projection[2][0] << "	" << projection[2][1] << "	" << projection[2][2] << "	" <<projection[2][3] <<std::endl;
				//std::cout << "projection: " << projection[3][0] << "	" << projection[3][1] << "	" << projection[3][2] << "	" <<projection[3][3] << std::endl;


				for (int R = 0; R < 361; R += 60)
				{
					GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
					GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
					//std::cout << "R: " << Y << std::endl;
					std::vector<float> U;
					std::vector<float> V;
					std::vector<float> W;

					camera = glm::rotate(camera, glm::radians((float)R), CameraSetup.camera_front);
					
					Simple_shader.use();
					Simple_shader.setMatrix4fv("model", model);
					Simple_shader.setMatrix4fv("projection", projection);
					Simple_shader.setMatrix4fv("view", camera);
					Simple_shader.setVector3f("viewPos", CameraSetup.camera_pose);
					Simple_shader.setVector3f("material.ambient", train_object.ambient);
					Simple_shader.setVector3f("material.diffuse", train_object.diffuse);
					Simple_shader.setVector3f("material.specular", train_object.specular);
					Simple_shader.setFloat("material.shininess", train_object.shininess);

					Simple_shader.setVector3f("light.position", light_position);
					Simple_shader.setVector3f("light.ambient", lightning.ambient);
					Simple_shader.setVector3f("light.diffuse", lightning.diffuse);
					Simple_shader.setVector3f("light.specular", lightning.light_color);
					GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
					TrainingObject.Draw(Simple_shader);
					GLCall(glClear(GL_DEPTH_BUFFER_BIT));


					//drawing bounding box
					int i = 0;
					while (i<TrainingObject.meshes[0].Vertecies.size())
					{
						glm::vec4 vertex_position_on_image =  projection * camera * model * glm::vec4(TrainingObject.meshes[0].Vertecies[i].Position, 1.0f);
						U.push_back(vertex_position_on_image[0]/vertex_position_on_image[3]);
						//std::cout << "vertex position on image: " << vertex_position_on_image[0]/ vertex_position_on_image[3]
						//			<< " " << vertex_position_on_image[1]/ vertex_position_on_image[3] << " " 
						//			<< vertex_position_on_image[2]/ vertex_position_on_image[3] << " " << vertex_position_on_image[3]/ vertex_position_on_image[3] << std::endl;
					
						V.push_back(vertex_position_on_image[1]/vertex_position_on_image[3]);
						//W.push_back(vertex_position_on_image[2]);
						i++;
					}
					float bb_X_max = *std::max_element(U.begin(), U.end());
					float bb_X_min = *std::min_element(U.begin(), U.end());
					//std::cout << "delta x: " << bb_X_min<<" "<<bb_X_max-bb_X_min << std::endl;
					float bb_Y_max = *std::max_element(V.begin(), V.end());
					float bb_Y_min = *std::min_element(V.begin(), V.end());
					//std::cout << "delta y: " << bb_Y_min << " "<<bb_Y_max-bb_Y_min << std::endl;
					//float bb_Z_max = *std::max_element(W.begin(), W.end());
					//std::cout << "Z max: " << bb_Z_max << std::endl;


					//define a functino to do all the transformation


					Basic_shader.use();
					//Basic_shader.setMatrix4fv("projection",otoprojection);


					std::map<std::string, int> AttribPointer_BB;
					AttribPointer_BB["layout_0"] = 0;
					AttribPointer_BB["size_of_vertex_0"] = 3;
					AttribPointer_BB["stride_0"] = 3 * sizeof(float);
					AttribPointer_BB["offset_0"] = 0;
					float bb_factor = 1;
					float bias = 0;

					float BoundingBox[] = {
						 bb_factor*bb_X_max,	bb_factor*bb_Y_max,	bias,  // top right
						 bb_factor*bb_X_max,	bb_factor*bb_Y_min,	bias,  // bottom right
						 bb_factor*bb_X_min,	bb_factor*bb_Y_min,	bias,  // bottom left
						 bb_factor*bb_X_min,	bb_factor*bb_Y_max,	bias
					};
					

					VertexBuffer BB(BoundingBox,
						BB_indicies,
						sizeof(BoundingBox) / sizeof(BoundingBox[0]),
						sizeof(float),
						sizeof(BB_indicies) / sizeof(BB_indicies[0]),
						sizeof(int),
						AttribPointer_BB,
						"bb");
					
					GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
					BB.Draw("draw_elements");

					GLCall(glfwSwapBuffers(window));
					GLCall(glfwPollEvents());
				}
			}
		}



	}
	glfwTerminate();
	exit(EXIT_SUCCESS);
	return 0;

}


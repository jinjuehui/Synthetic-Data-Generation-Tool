#include "utils.h"

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
CameraOrientation CameraSetup;
object_setting_for_fragment_shader train_object; 
light lightning;

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;
int delta_P(5), delta_Y(5), delta_R(10);

int main()
{

	GLFWwindow* window = initialize_window(SCR_WIDTH, SCR_HEIGHT, "Bounding Box");
	Shader Simple_shader("Simple_vertex.shader", "Simple_Fragment.shader");
	Shader Basic_shader("Basic_vertex.shader", "Basic_Fragment.shader");

	glm::mat4 model(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model,glm::vec3(1.0f));
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);


	Model TrainingObject("mesh/obj_05_simplified.stl");
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

	bounding_box bb;
	
	while (!glfwWindowShouldClose(window))
	{


		for (int P = 0; P <361; P+=delta_P)
		{	
			std::cout << "P: "<< P << std::endl;
			for (int Y = 0; Y < 361; Y += delta_Y)
			{
				std::cout << "Y: " << Y << std::endl;

				float distance = 0.2f;
				CameraSetup = rotateCamera(P, Y, distance);
				//std::cout << "camera position" << CameraSetup.camera_pose[0] << " " << CameraSetup.camera_pose[1] << " " << CameraSetup.camera_pose[2] << std::endl;
				camera = glm::lookAt(CameraSetup.camera_pose, glm::normalize(CameraSetup.camera_pose) + glm::normalize(CameraSetup.camera_front), CameraSetup.camera_up);
				glm::mat4 camera_t = glm::transpose(camera);
				//std::cout << "projection: " << projection[0][0] << "	" << projection[0][1] << "	" << projection[0][2] << "	" <<projection[0][3] << std::endl;
				//std::cout << "projection: " << projection[1][0] << "	" << projection[1][1] << "	" << projection[1][2] << "	" <<projection[1][3] <<std::endl;
				//std::cout << "projection: " << projection[2][0] << "	" << projection[2][1] << "	" << projection[2][2] << "	" <<projection[2][3] <<std::endl;
				//std::cout << "projection: " << projection[3][0] << "	" << projection[3][1] << "	" << projection[3][2] << "	" <<projection[3][3] << std::endl;


				for (int R = 0; R < 361; R += delta_R)
				{
					GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
					GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
					//std::cout << "R: " << Y << std::endl;
					std::vector<float> U;
					std::vector<float> V;

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

					Simple_shader.setVector3f("light.position", lightning.light_position);
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
						i++;
					}
					float bb_X_max = *std::max_element(U.begin(), U.end());
					float bb_X_min = *std::min_element(U.begin(), U.end());
					//std::cout << "delta x: " << bb_X_min<<" "<<bb_X_max-bb_X_min << std::endl;
					float bb_Y_max = *std::max_element(V.begin(), V.end());
					float bb_Y_min = *std::min_element(V.begin(), V.end());





					Basic_shader.use();


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
					
					bb = caculate_bounbox(bb_X_min,bb_X_max,bb_Y_min,bb_Y_max,SCR_WIDTH,SCR_HEIGHT);
					std::cout <<"bounding box: "<< bb.x << " " << bb.y << " " << bb.w << " " << bb.h << std::endl;
					
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


#include "utils.h"

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/random.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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
int delta_P(5), delta_Y(5), delta_R(10);//<---if the step here changed, then the yaml file need to be generated one more time 

int main()
{

	GLFWwindow* window = initialize_window(SCR_WIDTH, SCR_HEIGHT, "Bounding Box");
	Shader Simple_shader("Simple_vertex.shader", "Simple_Fragment.shader");
	Shader Basic_shader("Basic_vertex.shader", "Basic_Fragment.shader");
	Shader boundingbox_8p_shader("boundingbox_8p_vertex.shader", "boundingbox_8p_fragment.shader");

	glm::mat4 model(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model,glm::vec3(1.0f));
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);


	Model TrainingObject("mesh/obj_05_simplified.stl");
	std::cout << "finish loading model" << std::endl;

	std::vector<glm::vec3> pointset_on_image;
	glm::mat4 camera = glm::mat4(1.0f);	

	BoundingBox boundingbox(TrainingObject,SCR_WIDTH,SCR_HEIGHT);
	
	std::cout << "3d bouding box size: " << std::endl;
	std::cout << "x max:" << boundingbox.bb_v_3d.x_max << "x min: "
		<< boundingbox.bb_v_3d.x_min << " y max: "
		<< boundingbox.bb_v_3d.y_max << " y_min: "
		<< boundingbox.bb_v_3d.y_min << " z_max: "
		<< boundingbox.bb_v_3d.z_max << " z_min: "
		<< boundingbox.bb_v_3d.z_min << std::endl;

	float bounding_box_vertex_8point[] =
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

	float array[] = { 1,2,3,4,5,6,7,8,9,10 };
	//std::cout << "size of 8 point" << sizeof(bounding_box_vertex_8point)/sizeof(*bounding_box_vertex_8point)/3 << std::endl;
	std::vector<glm::vec3> bb_glm_vec3;

	for (int i = 0; i < 8; i++)
	{
		bb_glm_vec3.push_back(glm::vec3(bounding_box_vertex_8point[3*i], bounding_box_vertex_8point[3*i+1], bounding_box_vertex_8point[3*i+2]));
	}


	unsigned int bounding_box_indecies[] =
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


	unsigned int BB_indicies[] =
	{
		0,1,2,
		3,0,2
	};

	std::map<std::string, int> AttribPointer_BB_3d;
	AttribPointer_BB_3d["layout_0"] = 0;
	AttribPointer_BB_3d["size_of_vertex_0"] = 3;
	AttribPointer_BB_3d["stride_0"] = 3 * sizeof(float);
	AttribPointer_BB_3d["offset_0"] = 0;

	VertexBuffer BB_3d(bounding_box_vertex_8point,
		bounding_box_indecies,
		sizeof(bounding_box_vertex_8point) / sizeof(bounding_box_vertex_8point[0]),
		sizeof(float),
		sizeof(bounding_box_indecies) / sizeof(bounding_box_indecies[0]),
		sizeof(int),
		AttribPointer_BB_3d,
		"bb");


	int P=0, Y=0, R=0;

	GLCall(glEnable(GL_DEPTH_TEST));

	std::ofstream jsonfile;
	std::string json_path = "E:/label/label.json";
	jsonfile.open(json_path);
	
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

					//generating bounding box
					//for 2d bounding box
					//boundingbox.generate_bounding_box_labels_2d(TrainingObject, SCR_WIDTH, SCR_HEIGHT,P,Y,R, projection, camera, model,jsonfile,json_path);
					//for 3d 8 point project on to the 2d image
					boundingbox.generate_bounding_box_labels_3d(bb_glm_vec3, SCR_WIDTH, SCR_HEIGHT,P,Y,R,projection,camera,model,jsonfile,json_path);
					//2d Bounding Box
					float BoundingBox[] = {
						boundingbox.bb_v.x_max,	boundingbox.bb_v.y_max,	0.f,  // top right
						boundingbox.bb_v.x_max,	boundingbox.bb_v.y_min,	0.f,  // bottom right
						boundingbox.bb_v.x_min,	boundingbox.bb_v.y_min,	0.f,  // bottom left
						boundingbox.bb_v.x_min,	boundingbox.bb_v.y_max,	0.f
					};

					//3d Bounding Box
					boundingbox_8p_shader.use();
					boundingbox_8p_shader.setMatrix4fv("model", model);
					boundingbox_8p_shader.setMatrix4fv("view",camera);
					boundingbox_8p_shader.setMatrix4fv("projection",projection);

					GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
					BB_3d.Draw("draw_elements");


					std::map<std::string, int> AttribPointer_BB;
					AttribPointer_BB["layout_0"] = 0;
					AttribPointer_BB["size_of_vertex_0"] = 3;
					AttribPointer_BB["stride_0"] = 3 * sizeof(float);
					AttribPointer_BB["offset_0"] = 0;

					VertexBuffer BB(BoundingBox,
						BB_indicies,
						sizeof(BoundingBox) / sizeof(BoundingBox[0]),
						sizeof(float),
						sizeof(BB_indicies) / sizeof(BB_indicies[0]),
						sizeof(int),
						AttribPointer_BB,
						"bb");
					
					Basic_shader.use();
					BB.Draw("draw_elements");


					GLCall(glfwSwapBuffers(window));
					GLCall(glfwPollEvents());
				}
			}
		}

		jsonfile.close();

	}
		glfwTerminate();
		exit(EXIT_SUCCESS);
		return 0;

}


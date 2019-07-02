#include "utils.h"

//OpenGL mesh loader
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//write image output
//#include <FreeImage.h>

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
#define LOAD_MODEL "mesh/obj_05.stl"//"mesh/obj_05_re.stl"//"mesh/obj_05.stl"																								#change model type
#define ROTATE_CAMERA false
#define ENABLE_RANDOM_LIGHT_SOURCE_POSITION true
#define USE_SIMPLE_LIGHTNING_MODEL false
bool USE_BACKGROUND_IMAGE = true;
bool STATIC_CAMERA_VIEW = true; //set to true,camera won't moved by keybords input
bool ENABLE_USER_INPUT_TO_CONTROL_CAMERA = !STATIC_CAMERA_VIEW;
bool ROTATE_LIGHT = false;

//parameters
//Screen Parameters:
std::string const path = LOAD_MODEL;
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = SCR_WIDTH;

double lastX(SCR_WIDTH / 2), lastY(SCR_HEIGHT / 2);
glm::mat4 lamp, back_position;
glm::vec3 light_position(1.0f, 0.0f, 2.0f);

light dirLight, pointLight, spotLight, lightning;
object_setting_for_fragment_shader train_object;

//camera setup with default parameters
CameraOrientation Setup;

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


int main()
{
	//0.create window====================================================================
	GLFWwindow* window;
	window = initialize_window(SCR_WIDTH, SCR_HEIGHT, "Rendering...");

	//glfwMakeContextCurrent(window[0]);
	//glfwSetWindowSizeCallback(window[0], window_size_callback);
	std::cout << "use ESC to exit the Window" << std::endl;
	glfwSetWindowPos(window, 500, 500);

	//===============move into vertex classes to parse layout automatically====================
	std::map<std::string, int> AttribPointer_cube, AttribPointer_Background;
	AttribPointer_cube["layout_0"] = 0;
	AttribPointer_cube["size_of_vertex_0"] = 3;
	AttribPointer_cube["stride_0"] = 6 * sizeof(float);
	AttribPointer_cube["offset_0"] = 0;
	AttribPointer_cube["layout_1"] = 1;
	AttribPointer_cube["size_of_vertex_1"] = 3;
	AttribPointer_cube["stride_1"] = 6 * sizeof(float);
	AttribPointer_cube["offset_1"] = 3 * sizeof(float);
	//===============move into vertex classes to parse layout automatically=====================

	Model TrainingObject(LOAD_MODEL);
	Model ReferenceObject(LOAD_CUBE_REFERENCE);  //Obstacles

	// initializing shaer
	Shader lightning_shader("Lightning_vertex.shader", "Lightning_fragment.shader");
	Shader multiple_lightning_shader("multipleLightSource_vertex.shader", "multipleLightSource_fragment.shader");
	Shader Basic_shader("Basic_vertex.shader", "Basic_Fragment.shader"); //draw 2d bb
	Shader Boundingbox_8p_shader("boundingbox_8p_vertex.shader", "boundingbox_8p_fragment.shader"); //draw 3d bb
	Shader Segmentation("semantic_vertex.shader", "semantic_fragment.shader");
	Shader Simplelightning_shader("Simple_vertex.shader", "Simple_Fragment.shader");

	lamp = glm::translate(lamp, light_position);
	lamp = glm::scale(lamp, glm::vec3(10.0f, 10.0f, 10.0f));

	GLCall(glEnable(GL_DEPTH_TEST));
	std::cout << "rendering..." << std::endl;
	std::default_random_engine random_number_generator;

	while (!glfwWindowShouldClose(window))  //start the game
	{
		//GLCall(glViewport(0,0,1024,768));
		glm::mat4 camera = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		random_number_generator.seed(10);
		float light_strength = 5.f;

		for (int i = 60000; i < 80000; i++) // 80000 data, i=60000, i<800000
		{
			glm::mat4 object_model = glm::mat4(1.0f);  //trainning object matrix 
			object_model = glm::scale(object_model, glm::vec3(1.0f, 1.0f, 1.0f));
			glm::mat4 cube = glm::mat4(1.0f);
			cube = glm::translate(cube, glm::vec3(3.0f, 0.0f, 0.0f));
			cube = glm::scale(cube, glm::vec3(0.3f, 0.3f, 0.3f));

			//create light and cube vertex setting in OpenGL
			VertexBuffer Lightning(verticesLight, 108, sizeof(float), 0, 3, 3 * sizeof(float), 0);
			std::vector<glm::vec3> light_positions;
			int light_num = int(random_float(random_number_generator, 1, 5));
			std::cout << light_num << std::endl;

			for (int n = 0; n < light_num; n++)
			{
				light_positions.push_back(random_vec3(random_number_generator, -1.0, 1.0, -1.0, 1.0));
			}

			std::cout << "light position check: " << random_float(random_number_generator, -1.0f, 1.0f) << std::endl;
			projection = glm::perspective(glm::radians(60.f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
			camera = glm::lookAt(Setup.camera_pose, Setup.camera_pose + Setup.camera_front, Setup.camera_up);

			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));//added for object (bounding box use line)

			pointLight.ambient = random_v3_norm(random_number_generator, 0.08, 0.08, 0.08, 0.01);																			//randomize lightning color
			pointLight.diffuse = glm::vec3{ 0.8f,0.8f,0.8f };
			pointLight.specular = glm::vec3{ 1.0f,1.0,1.0f };

			multiple_lightning_shader.use();
			multiple_lightning_shader.setMatrix4fv("view", camera);
			multiple_lightning_shader.setVector3f("viewPos", Setup.camera_pose);
			multiple_lightning_shader.setMatrix4fv("projection", projection);
			multiple_lightning_shader.setMatrix4fv("model", cube);

			dirLight.ambient = glm::vec3{ 0.15f,0.15f,0.15f };
			dirLight.diffuse = glm::vec3{ 0.4f,0.4f,0.4f };
			dirLight.specular = glm::vec3{ 0.5f,0.5f,0.5f };

			multiple_lightning_shader.setVector3f("directionlight.direction", dirLight.light_direction);
			multiple_lightning_shader.setVector3f("directionlight.ambient", dirLight.ambient);
			multiple_lightning_shader.setVector3f("directionlight.diffuse", dirLight.diffuse);
			multiple_lightning_shader.setVector3f("directionlight.specular", dirLight.specular);

			if ((i + 1) % 2000 == 0)
				light_strength += 2.5;
			for (int n = 0; n < light_positions.size(); n++)
			{

				if (ENABLE_RANDOM_LIGHT_SOURCE_POSITION)
				{
					light_positions[n] = random_vec3(random_number_generator, -light_strength, light_strength, -light_strength, light_strength);						// -15, 15, -15, 15									//randomize light position
				}
				std::string number = std::to_string(n);
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


				multiple_lightning_shader.setVector3f(uniform_position, light_positions[n]);
				multiple_lightning_shader.setVector3f(uniform_ambient, pointLight.ambient);
				multiple_lightning_shader.setVector3f(uniform_diffuse, pointLight.diffuse);
				multiple_lightning_shader.setVector3f(uniform_specular, pointLight.specular);
				multiple_lightning_shader.setFloat(uniform_constantoffset, pointLight.constantoffset);
				multiple_lightning_shader.setFloat(uniform_linearfactor, pointLight.linearfactor);
				multiple_lightning_shader.setFloat(uniform_quadraticfactor, pointLight.quadraticfactor);

			}//<--for different light position

			spotLight.ambient = glm::vec3{ 0.0f,0.0f,0.0f };
			spotLight.diffuse = glm::vec3{ 1.0f,1.0f,1.0f };
			spotLight.specular = glm::vec3{ 1.0f,1.0f,1.0f };

			multiple_lightning_shader.setVector3f("spotlight.position", Setup.camera_pose);
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


			//glm::vec3 ObjectPosition = random_vec3(random_number_generator, -0.1, 0.1, 0.0, 0.0);
			//glm::vec3 ObjectPosition = set_random_with_distribution(random_number_generator, 0, 0.09, 0.05, 0.1);                //object position 0.03, 0.02
			glm::vec3 ObjectPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			object_model = glm::translate(object_model, ObjectPosition);
			//object_model = glm::translate(object_model, glm::vec3(0.1, 0.0, 0.25));

			//std::vector<float> angle = rotate_object_3axis_randomly(object_model, random_number_generator);
			glm::mat4 rotation_matrix = object_model = glm::rotate(object_model, float(-3.14/4), glm::vec3(1.0f, 1.0f, 0.0f));
			multiple_lightning_shader.setMatrix4fv("model", object_model);
			TrainingObject.Draw(multiple_lightning_shader);

			lightning_shader.use();
			lightning_shader.setMatrix4fv("projection_light", projection);
			lightning_shader.setMatrix4fv("view_light", camera);
			lightning_shader.setVector3f("LightColor", lightning.light_color);
			for (int n = 0; n < light_positions.size(); n++)
			{
				lamp = glm::mat4(1.0f);
				lamp = glm::translate(lamp, light_positions[n]);
				lamp = glm::scale(lamp, glm::vec3{ 10.0f,10.0f,10.0f });
				lightning_shader.setMatrix4fv("model_light", lamp);
				//Lightning.Draw("draw_arrays");

			}

			//screenshot_freeimage(picture.c_str(), SCR_WIDTH, SCR_HEIGHT);


			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());

			std::cin.get();

		}
		glfwTerminate();//destroy glcontext
		exit(EXIT_SUCCESS);

	}//<--while loop
	exit(EXIT_SUCCESS);
	return 0;


}//<--end of main

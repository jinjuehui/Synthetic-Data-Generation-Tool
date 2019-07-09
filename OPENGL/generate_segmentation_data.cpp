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
const unsigned int SCR_WIDTH = 224;
const unsigned int SCR_HEIGHT = SCR_WIDTH;
//System Time:
float deltaTime(0.0f), lastFrame(0.0f);//now the variables are only used for keyboard input callback functions		
//User Input Mouse and cursor
bool firstMouse(true);
double lastX(SCR_WIDTH / 2), lastY(SCR_HEIGHT / 2);
float yaw(-90.0f), pitch(0.0f), fov(45.0f);
//Drawing object	
glm::mat4 lamp, back_position;
glm::vec3 back_ground_position(1.0f, 1.0f, 1.0f);
glm::vec3 light_position(1.0f, 0.0f, 2.0f);

using json = nlohmann::json;

light dirLight, pointLight, spotLight, lightning;
object_setting_for_fragment_shader train_object, reference_object;


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
	Setup.camera_front = glm::normalize(front);

}

//move the camera forward, backward, sideways
void wasd_keyinput(GLFWwindow* window)
{
	float camera_speed = 5.0f*deltaTime;
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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Setup.camera_pose += camera_speed * glm::normalize(Setup.camera_up);
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
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

void conver_quaternion_to_array(glm::quat q, float quaternion[4])
{
	for (int i = 0; i < 4; i++)
	{
		quaternion[i] = q[i];
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

//* bench marking
//* photo realistic rendering
//* output data in PascalVOC structure
//* add instance segmentation
//* check rotation information
//* Image Loading problem, some image cause glTexImage2D exception break(walk arounded with deleting image from folder, probably related with aspect ration of the picture)
//* implement the split window to show both the rendered data and the ground truth data, multi processing might needed here

//Done(need be checked):
//* fix no rendering happens when P=0  (walk arounded with P set to 1)
//* Generating bounding box
//* generating 3d bounding box
//* Add in plane rotation
//* set pictures as the background of the window
//* optimize all of the VAO and VBOs
//* Object Position randomization
//* add semantic segmentation

//void window_size_callback(GLFWwindow* window, int width, int height)
//{
//	glfwGetWindowSize(window, &width, &height);
//}



int main()
{
	//0.create window====================================================================
	GLFWwindow* window[2];
	window[0] = initialize_window(SCR_WIDTH, SCR_HEIGHT, "Rendering...");

	//glfwMakeContextCurrent(window[0]);
	//glfwSetWindowSizeCallback(window[0], window_size_callback);
	std::cout << "use ESC to exit the Window" << std::endl;
	glfwSetKeyCallback(window[0], key_callback);
	glfwSetWindowPos(window[0], 500, 500);

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
	AttribPointer_Background["layout_0"] = 0;
	AttribPointer_Background["size_of_vertex_0"] = 3;
	AttribPointer_Background["stride_0"] = 5 * sizeof(float);
	AttribPointer_Background["offset_0"] = 0;
	AttribPointer_Background["layout_1"] = 1;
	AttribPointer_Background["size_of_vertex_1"] = 2;
	AttribPointer_Background["stride_1"] = 5 * sizeof(float);
	AttribPointer_Background["offset_1"] = 3 * sizeof(float);
	//===============move into vertex classes to parse layout automatically=====================



	//read file list int the folder
	std::cout << "creating image list..." << std::endl;
	std::map<std::string, int> Filelist = read_images_in_folder("E:\\autoencoder_6d_pose_estimation\\backgrounimage\\VOCdevkit\\VOC2012\\JPEGImages");//SegmentationClass
	std::map<std::string, int>::iterator it = Filelist.begin();
	std::advance(it, 0);  //2000  //3000(80000 data)

	std::cout << "image list created!" << std::endl;
	Model TrainingObject(LOAD_MODEL);
	//std::cout << "number of model meshes: " << TrainingObject.meshes[0].Vertecies.size() << std::endl;
	Model ReferenceObject(LOAD_CUBE_REFERENCE);  //Obstacles

	if (ENABLE_USER_INPUT_TO_CONTROL_CAMERA)
	{
		glfwSetInputMode(window[0], GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		std::cout << "user Input Enabled.." << std::endl;
		std::cout << "now you can move the camera view through keys: W,A,S,D " << std::endl;
		std::cout << "mouse to zoom.." << std::endl;
	}


	// initializing shaer
	Shader lightning_shader("Lightning_vertex.shader", "Lightning_fragment.shader");
	Shader multiple_lightning_shader("multipleLightSource_vertex.shader", "multipleLightSource_fragment.shader");
	Shader Basic_shader("Basic_vertex.shader", "Basic_Fragment.shader"); //draw 2d bb
	Shader Boundingbox_8p_shader("boundingbox_8p_vertex.shader", "boundingbox_8p_fragment.shader"); //draw 3d bb
	Shader Segmentation("semantic_vertex.shader", "semantic_fragment.shader");
	// setup bounding box object
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
	lamp = glm::scale(lamp, glm::vec3(10.0f, 10.0f, 10.0f));
	//back_position = glm::translate(back_position, back_ground_position);


	GLCall(glEnable(GL_DEPTH_TEST));
	std::cout << "rendering..." << std::endl;
	bool ground_truth = false;

	int delta_P(5), delta_Y(5), delta_R(10);

	std::default_random_engine random_number_generator;

	while (!glfwWindowShouldClose(window[0]))  //start the game
	{
		//GLCall(glViewport(0,0,1024,768));


		glm::mat4 cube = glm::mat4(1.0f);
		glm::mat4 camera = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);


		cube = glm::translate(cube, glm::vec3(3.0f, 0.0f, 0.0f));
		cube = glm::scale(cube, glm::vec3(0.3f, 0.3f, 0.3f));

		if (ground_truth)
		{
			USE_BACKGROUND_IMAGE = false;
		}

		//important to set random seed on this position, if this is done in the for loop, the randomization will behave locally
		random_number_generator.seed(10);   //2000 pic, seed3; 10000 pic seed1; 10000 seed2; 40000, seed4; 60000, seed5; 80000, seed6
		float light_strength = 1.f;

		for (int i = 0; i < 3; i++) // 80000 data, i=60000, i<800000
		{
			std::cout << "random_number: " << random_float(random_number_generator, 1, 5) << std::endl;
			glm::mat4 object_model = glm::mat4(1.0f);  //trainning object matrix 
			object_model = glm::scale(object_model, glm::vec3(1.0f, 1.0f, 1.0f));
			//initialize data name for generated picture
			std::string number = to_format(i);
			std::string picture = "E:/data/pose_estimation/tr/.jpg";//"E:/data/noobstacles_gt/tr/.jpg";
			std::string picture_multiobject = "E:/data/multi_object/image_tr.jpg";
			std::string picture_sm_seg = "E:/data/semantic_segmentation/image_tr.jpg";

			//initialize json:
			std::string json_path = "E:/data/pose_estimation/label/.json";//"E:/data/noobstacles_gt/label/.json";
			json_path.insert(30, number);
			json labels;
			std::ofstream jsonfile;
			if (!ground_truth)
			{
				std::cout << "generate label" << std::endl;
				jsonfile.open(json_path);
				labels["name"] = number;
				labels["object_id"] = path.substr(path.find_last_of('/') + 1);
			}


			//create light and cube vertex setting in OpenGL
			VertexBuffer Lightning(verticesLight, 108, sizeof(float), 0, 3, 3 * sizeof(float), 0);
			VertexBuffer Cube(cube_vertex,
				indicies_cube,
				sizeof(cube_vertex) / sizeof(cube_vertex[0]),
				sizeof(float),
				sizeof(indicies_cube) / sizeof(indicies_cube[0]),
				sizeof(int),
				AttribPointer_cube);
			std::cout << "create Background buffers and layout!" << std::endl;

			std::cout << "iterations: " << i << std::endl;
			// initialze light position vector, changing value here won't change the rendering result
			std::vector<glm::vec3> light_positions;
			int light_num = int(random_float(random_number_generator, 1, 5));
			std::cout << light_num << std::endl;

			for (int n = 0; n < light_num; n++)
			{
				light_positions.push_back(random_vec3(random_number_generator, -1.0, 1.0, -1.0, 1.0));
			}

			std::cout << "light position check: " << random_float(random_number_generator, -1.0f, 1.0f) << std::endl;
			//GLCall(glClearColor(0.03f, 0.05f, 0.05f, 1.0f));
			projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);

			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			camera = glm::lookAt(Setup.camera_pose, Setup.camera_pose + Setup.camera_front, Setup.camera_up);
			//create inplane rotation

			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));//added for object (bounding box use line)
			//use background image

			pointLight.ambient = random_v3_norm(random_number_generator, 0.08, 0.08, 0.08, 0.05);																			//randomize lightning color
			pointLight.diffuse = glm::vec3{ 0.8f,0.8f,0.8f };
			pointLight.specular = glm::vec3{ 1.0f,1.0,1.0f };

			if (USE_BACKGROUND_IMAGE)
			{

				VertexBuffer Background(background,
					back_indicies,
					sizeof(background) / sizeof(background[0]),
					sizeof(float),
					sizeof(back_indicies) / sizeof(back_indicies[0]),
					sizeof(int),
					AttribPointer_Background,
					"generate texture");

				if (it == Filelist.end())
					it = Filelist.begin();
				//background image setting
				int background_width, background_height, nrChannels;
				stbi_set_flip_vertically_on_load(true);
				unsigned char *data = stbi_load(it->first.c_str(), &background_width, &background_height, &nrChannels, 0);//Crynet_nanosuit.jpg
				//std::cout << "background image size: " << (float)background_width / background_height << std::endl;
				std::cout << it->first.c_str() << std::endl;


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
				it++;
				Shader background_shader("background_vertex.shader", "background_fragment.shader");
				background_shader.use();
				background_shader.setInt("texture1", 0);
				background_shader.setVector3f("light", pointLight.ambient);
				Background.Bind("bind_Texure");
				Background.Draw("draw_elements");
				GLCall(glClear(GL_DEPTH_BUFFER_BIT));//otherweise, it would be foreground

				Background.UnBind();
				stbi_image_free(data);

			}

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

			//std::cout << "number of the light sources: " << (int)((sizeof(light_positions) / sizeof(glm::vec3))) << std::endl;

			//pointLight.ambient = glm::vec3{ 0.05f,0.05f,0.05f };
			//pointLight.diffuse = glm::vec3{ 0.8f,0.8f,0.8f };
			//pointLight.specular = glm::vec3{ 1.0f,1.0,1.0f };


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
			glm::vec3 ObjectPosition = set_random_with_distribution(random_number_generator, 0, 0.09, 0.01);                //object position 0.03, 0.02
			//glm::vec3 ObjectPosition = glm::vec3(0.08f, 0.0f, 0.0f);
			std::cout << "position: " << " "<<ObjectPosition[0] <<" "<< ObjectPosition[1] <<" "<< ObjectPosition[2] <<std::endl;
			object_model = glm::translate(object_model, ObjectPosition);

			std::vector<float> rotation_angle= rotate_object_3axis_randomly(object_model, random_number_generator);
			//glm::mat4 rotation_matrix = object_model = glm::rotate(object_model, float(3.14/2.f), glm::vec3(0.0f, 1.0f, 0.0f));
			multiple_lightning_shader.setMatrix4fv("model", object_model);
			std::vector<float> projected_point = projection_single_point_on_creen(glm::vec3(0.0f, 0.0f, 0.0f), object_model, camera, projection);
			float pose_array[4][4], quaternion[4];
			if (!ground_truth)
			{
				glm::mat4 camera_transpose = glm::transpose(camera);
				labels["center_point"] = projected_point;  //careful its not tranlation, but [pic_centerx, pic_centery, distance]
				glm::mat4 pose = camera * object_model;
				pose = glm::transpose(pose);
				convert_array(pose, pose_array);
				conver_quaternion_to_array(glm::quat_cast(pose), quaternion);
				labels["Orientation"] = pose_array;
				labels["Quaternion"] = quaternion;
				//jsonfile << labels;
			}
			std::cout << "center point: " << projected_point[0] << " " << projected_point[1] << " " << projected_point[2] << std::endl;
			std::cout << "quaternion: " << quaternion[0] << " " << quaternion[1] << " " << quaternion[2] << " " << quaternion[3] << std::endl;
			std::cout << "object position matrix:" << std::endl;
			std::cout << "	" << pose_array[0][0] << "	" << pose_array[0][1] << "	" << pose_array[0][2] << "	" << pose_array[0][3] << "	" << std::endl;
			std::cout << "	" << pose_array[1][0] << "	" << pose_array[1][1] << "	" << pose_array[1][2] << "	" << pose_array[1][3] << "	" << std::endl;
			std::cout << "	" << pose_array[2][0] << "	" << pose_array[2][1] << "	" << pose_array[2][2] << "	" << pose_array[2][3] << "	" << std::endl;
			std::cout << "	" << pose_array[3][0] << "	" << pose_array[3][1] << "	" << pose_array[3][2] << "	" << pose_array[3][3] << "	" << std::endl;
			//////////////////////////setting shader for semantic segmentation////////////////////////////
			if (ground_truth)
			{
				Segmentation.use();
				glm::vec3 train_object_color(1.0f, 1.0f, 1.0f);
				Segmentation.setMatrix4fv("view", camera);
				Segmentation.setMatrix4fv("projection", projection);
				Segmentation.setMatrix4fv("model", object_model);
				Segmentation.setVector3f("fragcolor", train_object_color);
				TrainingObject.Draw(Segmentation);
			}
			else
			{
				std::cout << "draw training object" << std::endl;
				TrainingObject.Draw(multiple_lightning_shader);
			}


			//TrainingObject.Draw(multiple_lightning_shader);//main object for training
			//object_model = glm::rotate(object_model, -float(glm::radians(1.0)), glm::vec3(1.0, 0.0, 0.0));
			//inverse_object_3axis_rotation(object_model, rotation_matrix);
			//object_model = glm::translate(object_model, -ObjectPosition); //after drawing the object traslate it back to origin

			/*std::cout << "object position matrix:" << std::endl;
			std::cout << "	" << object_model[0][0] << "	" << object_model[0][1] << "	" << object_model[0][2] << "	" << object_model[0][3] << "	" << std::endl;
			std::cout << "	" << object_model[1][0] << "	" << object_model[1][1] << "	" << object_model[1][2] << "	" << object_model[1][3] << "	" << std::endl;
			std::cout << "	" << object_model[2][0] << "	" << object_model[2][1] << "	" << object_model[2][2] << "	" << object_model[2][3] << "	" << std::endl;
			std::cout << "	" << object_model[3][0] << "	" << object_model[3][1] << "	" << object_model[3][2] << "	" << object_model[3][3] << "	" << std::endl;

			std::cout <<"object: "<< object_model[0][0] << std::endl;*/

			//////////////////////using bounding box//////////////////////////////////////////////////////

			boundingbox.fill_bb_glm_vec3(bounding_box_vertex_8point);
			//boundingbox.generate_bounding_box_labels_3d(SCR_WIDTH, SCR_HEIGHT, P, Y, R, projection, camera, object_model, jsonfile, json_path);
			//std::cout << "labels generated" << std::endl;
			//3d Bounding Box
			Boundingbox_8p_shader.use();
			Boundingbox_8p_shader.setMatrix4fv("model", object_model);
			Boundingbox_8p_shader.setMatrix4fv("view", camera);
			Boundingbox_8p_shader.setMatrix4fv("projection", projection);
			GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
			//BB_3d.Draw("draw_elements");

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
			//BB_2d.Draw("draw_elements");
			GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

			////////////////////////////////////////////////////////////////////////////////////////


			cube = glm::mat4(1.0f);
			glm::vec3 cube_position = set_random_with_distribution(random_number_generator, 0.0, 0.2, 0.05);							//0.03, 0.02			//cube position
			object_setting_for_fragment_shader obstacles;
			float scale = random_float(random_number_generator, 0.2, 0.5);
			std::vector<float> cube_angle= rotate_object_3axis_randomly(cube, random_number_generator);
			cube = glm::translate(cube, cube_position);
			cube = glm::scale(cube, glm::vec3(scale));

			if (!ground_truth)
			{
				multiple_lightning_shader.use();
				multiple_lightning_shader.setMatrix4fv("model", cube);
				obstacles.ambient = set_random_with_distribution(random_number_generator, 0.5, 0.5, 0.2);
				obstacles.diffuse = set_random_with_distribution(random_number_generator, 0.5, 0.5, 0.2);

				multiple_lightning_shader.setVector3f("material.ambient", obstacles.ambient);
				multiple_lightning_shader.setVector3f("material.diffuse", obstacles.diffuse);
				ReferenceObject.Draw(multiple_lightning_shader);
			}
			else
			{
				glm::vec3 ambient = set_random_with_distribution(random_number_generator, 0.5, 0.5, 0.2);
				glm::vec3 diffuse = set_random_with_distribution(random_number_generator, 0.5, 0.5, 0.2);

				Segmentation.use();
				glm::vec3 train_object_color(0.0f, 0.0f, 0.0f);

				Segmentation.setMatrix4fv("view", camera);
				Segmentation.setMatrix4fv("projection", projection);
				Segmentation.setMatrix4fv("model", cube);
				Segmentation.setVector3f("fragcolor", train_object_color);
				ReferenceObject.Draw(Segmentation);//obstacles																	//draw obstacles

			}

			///////////////////////////////////////semantic segmentation///////////////////
			//Segmentation.use();
			//Segmentation.setMatrix4fv("view", camera);
			//Segmentation.setMatrix4fv("projection", projection);
			//Segmentation.setMatrix4fv("model", cube);
			//Segmentation.setVector3f("fragcolor", obstacles.diffuse);
			//inverse_object_3axis_rotation(cube, cube_rotation_matrix);
			//cube = glm::translate(cube, -cube_position);
			/////////////////////////////////////////////////////////////////////////////////

			//GLCall(glDrawArrays(GL_TRIANGLES, 0, 36);
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


			if (ground_truth)
			{
				picture = "E:/data/pose_estimation/gt_color/.jpg";//"E:/data/pose_estimation/gt/.jpg";//"E:/data/noobstacles_gt/gt/.jpg";//"E:/data/single_object2/gt/.jpg";
				picture_multiobject = "E:/data/multi_object/image_gt.jpg";
				picture_sm_seg = "E:/data/semantic_segmentation/image_gt.jpg";

			}
			picture.insert(picture.find_last_of('/')+1, number);
			picture_multiobject.insert(26, number);
			picture_sm_seg.insert(35, number);
			screenshot_freeimage(picture.c_str(), SCR_WIDTH, SCR_HEIGHT);


			GLCall(glfwSwapBuffers(window[0]));
			GLCall(glfwPollEvents());

			std::cin.get();
			if (!ground_truth)
				jsonfile.close();

		}



		if (ground_truth)
		{
			glfwTerminate();//destroy glcontext
			exit(EXIT_SUCCESS);

		}

		ground_truth = true;
		std::cout << "ground_truth: " << ground_truth << std::endl;
	}//<--while loop
	exit(EXIT_SUCCESS);
	return 0;


}//<--end of main

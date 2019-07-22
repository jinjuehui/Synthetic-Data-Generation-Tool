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
#define LOAD_MODEL "mesh/obj_05.stl"
#define ROTATE_CAMERA true
#define ENABLE_RANDOM_LIGHT_SOURCE_POSITION true
#define USE_SIMPLE_LIGHTNING_MODEL false
#define GENERATE_MASK true
bool USE_BACKGROUND_IMAGE = true;
bool STATIC_CAMERA_VIEW = true;
bool ENABLE_USER_INPUT_TO_CONTROL_CAMERA = !STATIC_CAMERA_VIEW;
bool ROTATE_LIGHT = false;

//parameters
//Screen Parameters:
std::string const path = LOAD_MODEL;
const unsigned int SCR_WIDTH = 224;
const unsigned int SCR_HEIGHT = 224;
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
glm::vec3 light_positions[] =
{ glm::ballRand(1.0),
	glm::ballRand(1.0),
	glm::ballRand(1.0),
	glm::ballRand(1.0)
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


void conver_quaternion_to_array(glm::quat q, float quaternion[4])
{
	for (int i = 0; i < 4; i++)
	{
		quaternion[i] = q[i];
	}
}


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
	window[0] = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, u8"Rendering...", NULL, NULL);
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
	glfwSetWindowPos(window[0], 500, 500);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "initialize glew failed" << std::endl;
	}
	else
	{
		std::cout << "glew initialization succeed!" << std::endl;
	}

	GLCall(glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT));


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
	//draw lights
	VertexBuffer Lightning(verticesLight, 108, sizeof(float), 0, 3, 3 * sizeof(float), 0);


	VertexBuffer Cube(cube_vertex,
		indicies_cube,
		sizeof(cube_vertex) / sizeof(cube_vertex[0]),
		sizeof(float),
		sizeof(indicies_cube) / sizeof(indicies_cube[0]),
		sizeof(int),
		AttribPointer_cube);

	VertexBuffer Background(background,
		back_indicies,
		sizeof(background) / sizeof(background[0]),
		sizeof(float),
		sizeof(back_indicies) / sizeof(back_indicies[0]),
		sizeof(int),
		AttribPointer_Background,
		"generate texture");

	//read file list int the folder
	std::map<std::string, int> Filelist = read_images_in_folder("E:\\autoencoder_6d_pose_estimation\\backgrounimage\\VOCdevkit\\VOC2012\\JPEGImages");//SegmentationClass
	std::map<std::string, int>::iterator it = Filelist.begin();
	std::advance(it, 1);

	Model TrainingObject(LOAD_MODEL);
	Model ReferenceObject(LOAD_CUBE_REFERENCE);

	Shader Simple_shader("Simple_vertex.shader", "Simple_Fragment.shader");
	Shader Simplelightning_shader("Simple_vertex.shader", "Simple_Fragment.shader");
	Shader lightning_shader("Lightning_vertex.shader", "Lightning_fragment.shader");
	Shader multiple_lightning_shader("multipleLightSource_vertex.shader", "multipleLightSource_fragment.shader");
	Shader Basic_shader("Basic_vertex.shader", "Basic_Fragment.shader");
	Shader Boundingbox_8p_shader("boundingbox_8p_vertex.shader", "boundingbox_8p_fragment.shader");
	Shader Segmentation("semantic_vertex.shader", "semantic_fragment.shader");
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

	lamp = glm::translate(lamp, light_position);
	lamp = glm::scale(lamp, glm::vec3(10.0f, 10.0f, 10.0f));
	//back_position = glm::translate(back_position, back_ground_position);


	GLCall(glEnable(GL_DEPTH_TEST));
	bool ground_truth = true;

	int delta_P(10), delta_Y(10), delta_R(10);
	bool generate_mask = false;
	while (!glfwWindowShouldClose(window[0]))  //start the game
	{
		//GLCall(glViewport(0,0,1024,768));

		glm::mat4 object_model = glm::mat4(1.0f);
		glm::mat4 cube = glm::mat4(1.0f);
		glm::mat4 camera = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		object_model = glm::scale(object_model, glm::vec3(1.0f, 1.0f, 1.0f));//for nanosuits default 0.4

		cube = glm::translate(cube, glm::vec3(3.0f, 0.0f, 0.0f));
		cube = glm::scale(cube, glm::vec3(0.3f, 0.3f, 0.3f));//for nanosuits default 0.4

		if (ground_truth)
		{
			USE_BACKGROUND_IMAGE = false;
		}
		camera = glm::lookAt(Setup.camera_pose, Setup.camera_pose + Setup.camera_front, Setup.camera_up);
		int number = 0;
		for (int R = 0; R < 361; R += delta_R)
		{
			//GLCall(glClearColor(0.03f, 0.05f, 0.05f, 1.0f));
			projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

			for (int Y = 0; Y < 361; Y += delta_Y)
			{
				for (int P = 0; P < 181; P += delta_P)
				{
					std::string number_str = to_format(number);
					std::cout << "R: " << R << "Y: " << Y << "P: " << P <<std::endl;
					object_model = object_model = glm::mat4(1.0f);
					object_model = glm::translate(object_model, glm::vec3(0.0f, 0.0f, 0.0));
					object_model = glm::rotate(object_model, glm::radians((float)Y), glm::vec3(1.0f, 0.0f, 0.0f));
					object_model = glm::rotate(object_model, glm::radians((float)R), glm::vec3(0.0f,1.0f,0.0f));
					object_model = glm::rotate(object_model, glm::radians((float)P), glm::vec3(0.0f, 0.0f, 1.0f));

					GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
					GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
					GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));//added for object (bounding box use line)
					//use background image
					if (USE_BACKGROUND_IMAGE)
					{
						
						if (it == Filelist.end())
							it = Filelist.begin();
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
					if (!generate_mask)
					{
						std::string json_path = "E:/data/pose_estimation/continue_rotation_label/.json";
						json_path.insert(json_path.find_last_of('/')+1, number_str);
						json labels;
						std::ofstream jsonfile;
						jsonfile.open(json_path);
						labels["name"] = number;
						labels["object_id"] = path.substr(path.find_last_of('/') + 4, path.find_last_of('/') + 6);

						float pose_array[4][4], quaternion[4];
						glm::quat quaternion_original;
						std::vector<float> projected_point = projection_single_point_on_creen(glm::vec3(0.0f, 0.0f, 0.0f), object_model, camera, projection);
						glm::mat4 camera_transpose = glm::transpose(camera);
						glm::mat4 pose = camera_transpose * object_model;
						for (int u = 0; u < 3; u++)
							for (int v = 0; v < 3; v++)
							{
								if (std::abs(pose[u][v]) < 1e-7)
									pose[u][v] = 0;
							}
						convert_array(pose, pose_array);
						quaternion_original = glm::quat_cast(pose);
						conver_quaternion_to_array(quaternion_original, quaternion);
						labels["center_point"] = projected_point;
						labels["Orientation"] = pose_array;
						labels["Quaternion"] = quaternion;
						jsonfile << labels;
						std::cout << "camera_transpose*object_model matrix:" << std::endl;
						std::cout << "	" << pose[0][0] << "	" << pose[0][1] << "	" << pose[0][2] << "	" << pose[0][3] << "	" << std::endl;
						std::cout << "	" << pose[1][0] << "	" << pose[1][1] << "	" << pose[1][2] << "	" << pose[1][3] << "	" << std::endl;
						std::cout << "	" << pose[2][0] << "	" << pose[2][1] << "	" << pose[2][2] << "	" << pose[2][3] << "	" << std::endl;
						std::cout << "	" << pose[3][0] << "	" << pose[3][1] << "	" << pose[3][2] << "	" << pose[3][3] << "	" << std::endl;

						std::cout << "quaternion:" << std::endl;
						std::cout << "	" << quaternion[0] << "	" << quaternion[1] << "	" << quaternion[2] << "	" << quaternion[3] << "	" << std::endl;

						std::cout << "centroid:" << std::endl;
						std::cout << "	" << projected_point[0] << "	" << projected_point[1] << "	" << projected_point[2] << std::endl;
						jsonfile.close();
					}
					

					if (!USE_SIMPLE_LIGHTNING_MODEL)
					{
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

						pointLight.ambient = glm::vec3{ 0.05f,0.05f,0.05f };
						pointLight.diffuse = glm::vec3{ 0.8f,0.8f,0.8f };
						pointLight.specular = glm::vec3{ 1.0f,1.0,1.0f };

				

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


						multiple_lightning_shader.use();
						multiple_lightning_shader.setMatrix4fv("model", object_model);

						//////////////////////////setting shader for semantic segmentation////////////////////////////
						if (generate_mask)
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
							light simple_light;
							simple_light.light_position = glm::vec3(0.1, 0.1, 0.9);
							simple_light.ambient = glm::vec3(0.2);
							simple_light.diffuse = glm::vec3(0.5);
							simple_light.specular = glm::vec3(0.5);
							/*simple_light.constantoffset = 1.f;
							simple_light.linearfactor = 0.5;
							simple_light.quadraticfactor = 0.5;
							simple_light.cutoff = glm::cos(glm::radians(20.5f));
							simple_light.outercutoff = glm::cos(glm::radians(35.0f));*/

							train_object.ambient = glm::vec3(0.9);
							train_object.diffuse = glm::vec3(0.9);
							train_object.specular = glm::vec3(0.7);
							Simplelightning_shader.use();
							Simplelightning_shader.setMatrix4fv("model", object_model);
							Simplelightning_shader.setMatrix4fv("view", camera);
							Simplelightning_shader.setMatrix4fv("projection", projection);
							Simplelightning_shader.setVector3f("viewPos", Setup.camera_pose);
							Simplelightning_shader.setVector3f("material.ambient", train_object.ambient);
							Simplelightning_shader.setVector3f("material.diffuse", train_object.diffuse);
							Simplelightning_shader.setVector3f("material.specular", train_object.specular);
							Simplelightning_shader.setFloat("material.shininess", train_object.shininess);
							Simplelightning_shader.setVector3f("light.position", simple_light.light_position);
							Simplelightning_shader.setFloat("light.cutoff", simple_light.cutoff);
							Simplelightning_shader.setFloat("light.outerutoff", simple_light.outercutoff);
							Simplelightning_shader.setVector3f("light.ambient", simple_light.ambient);
							Simplelightning_shader.setVector3f("light.diffuse", simple_light.diffuse);
							Simplelightning_shader.setVector3f("light.specular", simple_light.specular);
							GLCall(glClear(GL_COLOR_BUFFER_BIT))
							TrainingObject.Draw(Simplelightning_shader);//main object for training
						}
						

						//////////////////////using bounding box//////////////////////////////////////////////////////
						/*std::cout << "first loop:" << P << std::endl;
						std::cout << " " << object_model[0][0] << " " << object_model[0][1] << " " << object_model[0][2] << " " << object_model[0][3] << " " << std::endl;
						std::cout << " " << object_model[1][0] << " " << object_model[1][1] << " " << object_model[1][2] << " " << object_model[1][3] << " " << std::endl;
						std::cout << " " << object_model[2][0] << " " << object_model[2][1] << " " << object_model[2][2] << " " << object_model[2][3] << " " << std::endl;
						std::cout << " " << object_model[3][0] << " " << object_model[3][1] << " " << object_model[3][2] << " " << object_model[3][3] << " " << std::endl;*/

						boundingbox.fill_bb_glm_vec3(bounding_box_vertex_8point);
						//boundingbox.generate_bounding_box_labels_3d(SCR_WIDTH, SCR_HEIGHT, P, Y, R, projection, camera, object_model, jsonfile, json_path);
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

							///////////////////////////////////////semantic segmentation///////////////////
							//Segmentation.use();
							//Segmentation.setMatrix4fv("view", camera);
							//Segmentation.setMatrix4fv("projection", projection);
							//Segmentation.setMatrix4fv("model", cube);
							//Segmentation.setVector3f("fragcolor", obstacles.diffuse);

							ReferenceObject.Draw(Segmentation);//obstacles
							///////////////////////////////////////////////////////////////////////////////////
							glm::mat4 cube2 = glm::mat4(1.0f);
							cube2 = glm::translate(cube2, glm::vec3{ 4.0f,0.0f,0.0f });
							cube2 = glm::scale(cube2, glm::vec3(0.3f));
							multiple_lightning_shader.use();
							multiple_lightning_shader.setMatrix4fv("model", cube2);
							multiple_lightning_shader.setVector3f("material.ambient", reference_object.ambient);
							multiple_lightning_shader.setVector3f("material.diffuse", reference_object.diffuse);

							/////////////////////////////semantic segmentation/////////////////////////////////
							//Segmentation.use();
							//glm::vec3 obstacle_color(0.0f,0.5f,0.5f);
							//Segmentation.setMatrix4fv("view", camera);
							//Segmentation.setMatrix4fv("projection", projection);
							//Segmentation.setMatrix4fv("model", cube2);
							//Segmentation.setVector3f("fragcolor", obstacle_color);
							Cube.Draw("draw_elements");
							///////////////////////////////////////////////////////////////////////////////////

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

					if (generate_mask)
					{
						std::string picture = "E:/data/pose_estimation/continue_rotation_gt/.jpg";
						picture.insert(45, number_str);
						//std::cout << picture;
						screenshot_freeimage(picture.c_str(), SCR_WIDTH, SCR_HEIGHT);
					}
					else {
						std::string picture = "E:/data/pose_estimation/continue_rotation/.jpg";
						picture.insert(42, number_str);
						//std::cout << picture;
						screenshot_freeimage(picture.c_str(), SCR_WIDTH, SCR_HEIGHT);

					}
					
					number += 1;
					GLCall(glfwSwapBuffers(window[0]));
					GLCall(glfwPollEvents());

					//std::cin.get();
				}//<---end loop for roll
			}//<---Y loop
		}//<--P loop
		if (generate_mask)
		{
			glfwTerminate();//destroy glcontext
			exit(EXIT_SUCCESS);
		}
		generate_mask = true;

	}//<--while loop
	exit(EXIT_SUCCESS);
	return 0;
}//<--end of main

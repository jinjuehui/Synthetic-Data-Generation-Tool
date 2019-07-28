#include "utils.h"

//OpenGL mesh loader
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "stb_image.h"
#include "Camera.h"
#include <fstream>
#include <map>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;
#define LOAD_CUBE_REFERENCE "mesh/nanosuit/chess/test/untitled.obj"
#define LOAD_MODEL "mesh/obj_05.stl"//"mesh/obj_05_re.stl"//"mesh/obj_05.stl"																								#change model type
#define LOAD_CYLINDER "mesh/distractions/cylinder.stl"
#define LOAD_CONE "mesh/distractions/cone.stl"
#define LOAD_DONAS  "mesh/distractions/torus.stl"//"mesh/distractions/donas.stl"
#define LOAD_SPHERE "mesh/distractions/sphere.stl"
#define BACK_GROUND_IMAGE_PATH "D:\\autoencoder_6d_pose_estimation\\backgrounimage\\VOCdevkit\\VOC2012\\JPEGImages"
#define JSON_LABEL "D:/data/human_error/960x720/test_real_label_error/label/.json"
#define SAVE_IMAGE_PATH "D:/data/human_error/960x720/test_real_label_error/image/.jpg"  //D:/data/segmentation/training_data/.jpg
#define MASK_DATA_PATH "D:/data/human_error/960x720/test_real_label_error/mask_data/.jpg"
#define ROTATE_CAMERA false
#define ENABLE_RANDOM_LIGHT_SOURCE_POSITION true
#define USE_SIMPLE_LIGHTNING_MODEL false
bool USE_BACKGROUND_IMAGE = true;
bool STATIC_CAMERA_VIEW = true; //set to true,camera won't moved by keybords input
bool ENABLE_USER_INPUT_TO_CONTROL_CAMERA = !STATIC_CAMERA_VIEW;
bool ROTATE_LIGHT = false;
const unsigned int SCR_WIDTH = 960;
const unsigned int SCR_HEIGHT = 720;
using json = nlohmann::json;
std::string const path = LOAD_MODEL;
glm::mat4 back_position;
glm::vec3 back_ground_position(1.0f, 1.0f, 1.0f);
glm::vec3 light_position(1.0f, 0.0f, 2.0f);
light dirLight, pointLight, spotLight, lightning;
object_setting_for_fragment_shader train_object, cylinder, cone, donas;
CameraOrientation Setup;
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


glm::mat4 model_matrix_generator(glm::mat4& model, std::default_random_engine& randomizer, std::vector<float> scale_factor, std::vector<float> pose_params)
{
	model = glm::mat4(1.0f);
	glm::vec3 position = set_random_with_distribution(randomizer, pose_params[2], pose_params[3], pose_params[4], pose_params[5]);							//0.03, 0.02			//cube position
	//glm::vec3 position = random_vec3(randomizer, -xy, xy, -z, z);
	std::cout << "position: " << position[0] << std::endl;
	float scale = random_float(randomizer, scale_factor[0], scale_factor[1]);
	std::vector<float> cube_angle = rotate_object_3axis_randomly(model, randomizer, pose_params[0], pose_params[1]);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(scale));
	return model;
}

void setting_object_properties_in_shader(Shader& shader, object_setting_for_fragment_shader& model, glm::mat4& model_matrix)
{
	shader.use();
	shader.setMatrix4fv("model", model_matrix);
	shader.setVector3f("material.ambient", model.ambient);
	shader.setVector3f("material.diffuse", model.diffuse);
	shader.setVector3f("material.specular", model.specular);
	shader.setFloat("material.shininess", model.shininess);
}


object_setting_for_fragment_shader random_object_color(object_setting_for_fragment_shader& mesh_model,
	std::default_random_engine& randomizer,
	std::vector<float> ambient,
	std::vector<float> diffuse,
	std::vector<float> specular,
	std::vector<float> shininess)
{
	mesh_model.ambient = random_v3_norm(randomizer, ambient[0], ambient[1], ambient[2], ambient[3]);
	mesh_model.diffuse = random_v3_norm(randomizer, diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
	mesh_model.specular = random_v3_norm(randomizer, specular[0], specular[1], specular[2], specular[3]);
	mesh_model.shininess = random_float(randomizer, shininess[0], shininess[1]);
	return mesh_model;

}

void dump_iterating(std::default_random_engine& randomizer,
	int n,
	std::vector<float> ambient,
	std::vector<float> diffuse,
	std::vector<float> specular,
	std::vector<float> shininess)
{
	for (int i = 0; i < n; i++)
	{
		object_setting_for_fragment_shader dump;
		dump = random_object_color(dump, randomizer, ambient, diffuse, specular, shininess);
	}
}


std::map<std::string, int> read_images_in_folder(std::string path)
{
	std::map<std::string, int> files;
	//std::cout << "out side loop" << std::endl;
	for (const auto& entry : fs::directory_iterator(path))
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

float background[] = {
		 1.0f,	1.0f,	0.0f, 1.f, 1.0f, // top right
		 1.0f,  -1.0f,	0.0f, 1.0f, 0.0f, // bottom right
		-1.0f,	-1.0f,	0.0f, 0.0f, 0.0f, // bottom left
		-1.0f,	1.0f,	0.0f, 0.0f, 1.0f
};

unsigned int back_indicies[] =
{
	0,1,2,
	3,0,2
};


void generate_json_label(std::string json_path, int number, glm::mat4 object_model, glm::mat4 camera, glm::mat4 projection, BoundingBox::bounding_box bb)
{
	std::string number_str = to_format(number); //"E:/data/pose_estimation/continue_rotation_label/.json";
	json_path.insert(json_path.find_last_of('/') + 1, number_str);
	json labels;
	std::ofstream jsonfile;
	jsonfile.open(json_path);
	labels["name"] = number;
	labels["object_id"] = path.substr(path.find_last_of('/') + 4, path.find_last_of('/') + 6);

	std::vector<float> bb_2d;
	bb_2d.push_back(bb.x_min);
	bb_2d.push_back(bb.y_min);
	bb_2d.push_back(bb.x_max);
	bb_2d.push_back(bb.y_max);


	float pose_array[4][4], quaternion[4];
	glm::quat quaternion_original;
	std::vector<float> projected_point = projection_single_point_on_creen(glm::vec3(0.0f, 0.0f, 0.0f), object_model, camera, projection);
	std::cout << "object center position: " << projected_point[0] << " " << projected_point[1] << " " << projected_point[2] << std::endl;
	glm::mat4 camera_transpose = glm::transpose(camera);
	glm::mat4 pose = glm::transpose(camera * object_model);
	std::cout << "pose" << std::endl;
	std::cout << "	" << pose[0][0] << "	" << pose[0][1] << "	" << pose[0][2] << "	" << pose[0][3] << "	" << std::endl;
	std::cout << "	" << pose[1][0] << "	" << pose[1][1] << "	" << pose[1][2] << "	" << pose[1][3] << "	" << std::endl;
	std::cout << "	" << pose[2][0] << "	" << pose[2][1] << "	" << pose[2][2] << "	" << pose[2][3] << "	" << std::endl;
	std::cout << "	" << pose[3][0] << "	" << pose[3][1] << "	" << pose[3][2] << "	" << pose[3][3] << "	" << std::endl;

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
	labels["BoundingBox"] = bb_2d;
	jsonfile << labels;
	std::cout << "bounding_box, x_min: " << bb_2d[0] << "y_min: " << bb_2d[1] << "x_max: " << bb_2d[2] << "y_max: " << bb_2d[3] << std::endl;
	std::cout << "quaternion:" << std::endl;
	std::cout << "	" << quaternion[0] << "	" << quaternion[1] << "	" << quaternion[2] << "	" << quaternion[3] << "	" << std::endl;
	std::cout << "centroid:" << std::endl;
	std::cout << "	" << projected_point[0] << "	" << projected_point[1] << "	" << projected_point[2] << std::endl;
	std::cout << "bounding box" << bb_2d[0] << " " << bb_2d[1] << " " << bb_2d[2] << " " << bb_2d[3] << std::endl;
	jsonfile.close();
}


//Randomization factors
//1. lighting conditions
//Randomization factors
//1. lighting conditions, no spot light now
std::vector<float> light_number_range = { 3.0f, 5.0f };						//minimum>=2	maximum
std::vector<float> light_position_step = { 2000, 0.5 };						//step_number, step_size, x,y,z min=-step_size and max=step_size
std::vector<float> point_light_ambient_color = { 0.08f,0.08f,0.08f,0.4f };	// r mean, g mean, b mean, sigma  last change step, 0.1,0.2...
std::vector<float> point_light_diffuse_color = { 0.8f,0.8f,0.8f,0.01f };	//
std::vector<float> point_light_specular_color = { 1.0f,1.0f,1.0f,0.01f };
//std::vector<float> point_light_position = { 1.0f , 1.0f, 5.0f };			//start position, step size, end position (meter)
std::vector<float> direction_light_direction = { -0.2f,-1.0f,-0.3f, 0.5f }; //
std::vector<float> direction_light_ambient = { 0.15f,0.15f,0.15f,0.01f };
std::vector<float> direction_light_diffuse = { 0.4f,0.4f,0.4f,0.01f };
std::vector<float> direction_light_specular = { 0.5f,0.5,0.5f,0.01f };
//2. object material	
//std::vector<float> train_color = { 0.5f,0.5f, 0.5f, 0.01f };
std::vector<float> train_ambient = { 0.1f,0.1f, 0.1, 0.01f };
std::vector<float> train_diffuse = { 0.55f,0.55f, 0.55,0.01f };
std::vector<float> train_specular = { 0.2f,0.2f, 0.2f, 0.01f };
std::vector<float> train_shininess = { 0.1f, 16.0f };						//minimum, maximum
std::vector<float> distractor_color = { 0.5f,0.5f, 0.5f,0.8f };
std::vector<float> distractor_ambient = { 0.5f,0.5f, 0.5f,0.3f };
std::vector<float> distractor_diffuse = { 0.5f,0.5f, 0.5f,0.3f };
std::vector<float> distractor_specular = { 0.1f,0.1f, 0.1f,0.3f };
std::vector<float> distractor_shininess = { 0.1f, 16.0f };
//3.object position
std::vector<float> object_position_distribution = { 0, 0.2, 0.02, 0.07 };	//xy_mean, z_mean, xy_sigma, z_sigma
std::vector<float> obstacles_scale_factor = { 0.2, 0.5 };						//minimum maximum
std::vector<float> obstacles_scale_factor2 = { 60.f, 80.f };

std::vector<float> cube_position = { 0.f, 3.14f, 0.0, 0.4, 0.01, 0.1 };		//angle min,max, traslation xy_mean, z_mean, xy_sigma, z_sigma	
std::vector<float> cone_position = { 0.f, 7.f, 0.0, 0.4, 0.01, 0.1 };
std::vector<float> sphere_position = { 0.f, 3.65f, 0.0, 0.4, 0.01, 0.1 };
std::vector<float> donas_position = { 0.f, 2.48f, 0.0, 0.4, 0.01, 0.1 };


int main()
{
	GLFWwindow* window;
	window = initialize_window(SCR_WIDTH, SCR_HEIGHT, "Rendering...");
	glfwSetWindowPos(window, 2000, 100);

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

	std::cout << "creating image list..." << std::endl;

	std::map<std::string, int> Filelist = read_images_in_folder(BACK_GROUND_IMAGE_PATH);//SegmentationClass
	std::map<std::string, int>::iterator it = Filelist.begin();
	std::advance(it, 0);  //2000  //3000(80000 data)


	std::cout << "image list created!" << std::endl;
	Model TrainingObject(LOAD_MODEL);
	Model ReferenceObject(LOAD_CUBE_REFERENCE);
	Model CylinderObject(LOAD_CYLINDER);
	Model ConeObject(LOAD_CONE);
	Model SphereObject(LOAD_SPHERE);
	Model DonasObject(LOAD_DONAS);

	Shader multiple_lightning_shader("multipleLightSource_vertex.shader", "multipleLightSource_fragment.shader");
	Shader Basic_shader("Basic_vertex.shader", "Basic_Fragment.shader"); //draw 2d bb
	Shader Boundingbox_8p_shader("boundingbox_8p_vertex.shader", "boundingbox_8p_fragment.shader"); //draw 3d bb
	Shader Segmentation("semantic_vertex.shader", "semantic_fragment.shader");
	Shader Simplelightning_shader("Simple_vertex.shader", "Simple_Fragment.shader");
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

	std::map<std::string, int> AttribPointer_BB; //problem by extracting to calss: pass array result in uncorrect bing VAO
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

	GLCall(glEnable(GL_DEPTH_TEST));
	std::cout << "rendering..." << std::endl;
	bool ground_truth = false;
	std::default_random_engine random_number_generator;
	std::vector<float> camera_intrin = { 810.4968405, 0.0, 487.5509672, 0.0, 810.61326022, 354.6674888, 0.0, 0.0, 1.0 };
	Camera real_camera(camera_intrin[0], camera_intrin[4], camera_intrin[2], camera_intrin[5], SCR_WIDTH, SCR_HEIGHT, 0.1, 10000);
	real_camera.perspective_NDC[1][1] = -real_camera.perspective_NDC[1][1];
	glm::mat3 mat, mat1, mat2;
	mat1 = { {1,2,3}, {3,4,5},{1,2,3} };
	mat2 = { {5,6,7}, {7,8,9},{3,4,5} };
	mat = glm::transpose(glm::transpose(mat1) * glm::transpose(mat2));
	std::cout << "test mat multiply" << std::endl;
	std::cout << mat[0][0] << ", " << mat[0][1] << ", " << mat[0][2] << std::endl;
	std::cout << mat[1][0] << ", " << mat[1][1] << ", " << mat[1][2] << std::endl;
	std::cout << mat[2][0] << ", " << mat[2][1] << ", " << mat[2][2] << std::endl;
	while (!glfwWindowShouldClose(window))  //start the game
	{

		glm::mat4 camera = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		if (ground_truth)
		{
			USE_BACKGROUND_IMAGE = false;
		}

		float light_strength = 1.f;
		random_number_generator.seed(8);   // 5, //2000 pic, seed3; 10000 pic seed1; 10000 seed2; 40000, seed4; 60000, seed5; 80000, seed6
		for (int i = 30000; i < 31000; i++) // 80000 data, i=60000, i<800000
		{
			std::cout << "iterations: " << i << std::endl;
			std::cout << "random test: " << random_float(random_number_generator, 1.0f, 5.0f) << std::endl;

			glm::mat4 object_model = glm::mat4(1.0f);  //trainning object matrix 
			glm::mat4 cube = glm::mat4(1.0f);
			object_model = glm::scale(object_model, glm::vec3(1.0f, 1.0f, 1.0f));
			//cube = glm::translate(cube, glm::vec3(3.0f, 0.0f, 0.0f));
			//cube = glm::scale(cube, glm::vec3(0.3f, 0.3f, 0.3f));
			std::string number = to_format(i);
			std::string picture = SAVE_IMAGE_PATH;

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
			std::vector<glm::vec3> light_positions;
			int light_num = int(random_float(random_number_generator, light_number_range[0], light_number_range[1]));

			std::cout << "light number: " << light_num << std::endl;

			for (int n = 0; n < light_num; n++)
			{
				light_positions.push_back(random_vec3(random_number_generator, -1.0, 1.0, -1.0, 1.0));
			}



			std::cout << "light position check: " << light_positions[0].x << " " << light_positions[0].y << " " << light_positions[0].z << std::endl;
			//projection = glm::perspective(glm::radians(30.f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
			projection = glm::transpose(real_camera.perspective_NDC);

			camera = glm::lookAt(Setup.camera_pose, Setup.camera_pose + Setup.camera_front, Setup.camera_up);

			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			//GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));//added for object (bounding box use line)

			pointLight.ambient = random_v3_norm(random_number_generator, point_light_ambient_color[0], point_light_ambient_color[1], point_light_ambient_color[2], point_light_ambient_color[3]);																			//randomize lightning color
			pointLight.diffuse = random_v3_norm(random_number_generator, point_light_diffuse_color[0], point_light_diffuse_color[1], point_light_diffuse_color[2], point_light_diffuse_color[3]);
			pointLight.specular = random_v3_norm(random_number_generator, point_light_specular_color[0], point_light_specular_color[1], point_light_specular_color[2], point_light_specular_color[3]);


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
				unsigned char* data = stbi_load(it->first.c_str(), &background_width, &background_height, &nrChannels, 0);//Crynet_nanosuit.jpg
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

			dirLight.ambient = random_v3_norm(random_number_generator, direction_light_ambient[0], direction_light_ambient[1], direction_light_ambient[2], direction_light_ambient[3]);
			dirLight.diffuse = random_v3_norm(random_number_generator, direction_light_diffuse[0], direction_light_diffuse[1], direction_light_diffuse[2], direction_light_diffuse[3]);
			dirLight.specular = random_v3_norm(random_number_generator, direction_light_specular[0], direction_light_specular[1], direction_light_specular[2], direction_light_specular[3]);
			dirLight.light_direction = random_v3_norm(random_number_generator, direction_light_direction[0], direction_light_direction[1], direction_light_direction[2], direction_light_direction[3]);

			multiple_lightning_shader.setVector3f("directionlight.direction", dirLight.light_direction);
			multiple_lightning_shader.setVector3f("directionlight.ambient", dirLight.ambient);
			multiple_lightning_shader.setVector3f("directionlight.diffuse", dirLight.diffuse);
			multiple_lightning_shader.setVector3f("directionlight.specular", dirLight.specular);

			if ((i + 1) % int(light_position_step[0]) == 0)
				light_strength += light_position_step[1];
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

			train_object = random_object_color(train_object, random_number_generator, train_ambient, train_diffuse, train_specular, train_shininess);

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
			glm::vec3 ObjectPosition = set_random_with_distribution(random_number_generator, object_position_distribution[0], object_position_distribution[1], object_position_distribution[2], object_position_distribution[3]);                //object position 0.03, 0.02
			//glm::vec3 ObjectPosition = glm::vec3(0.1f, 0.050f, 0.1f);
			std::cout << "position: " << " " << ObjectPosition[0] << " " << ObjectPosition[1] << " " << ObjectPosition[2] << std::endl;
			object_model = glm::translate(object_model, ObjectPosition);
			//object_model = glm::translate(object_model, glm::vec3(0.1, 0.0, 0.25));
			std::vector<float> angle = rotate_object_3axis_randomly(object_model, random_number_generator);
			std::cout << "object model:" << std::endl;

			//glm::mat4 rotation_matrix = object_model = glm::rotate(object_model, float(0), glm::vec3(1.0f, 0.0f, 0.0f));
			multiple_lightning_shader.setMatrix4fv("model", object_model);
			std::vector<float> projected_point = projection_single_point_on_creen(glm::vec3(0.0f, 0.0f, 0.0f), object_model, camera, projection);
			float pose_array[4][4], quaternion[4];
			glm::quat quaternion_original;

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
				//DonasObject.Draw(multiple_lightning_shader);
			}


			//TrainingObject.Draw(multiple_lightning_shader);//main object for training
			//object_model = glm::rotate(object_model, -float(glm::radians(1.0)), glm::vec3(1.0, 0.0, 0.0));
			//inverse_object_3axis_rotation(object_model, angle);
			//object_model = glm::translate(object_model, -ObjectPosition); //after drawing the object traslate it back to origin

			std::cout << "NDC matrix:" << std::endl;
			std::cout << "	" << real_camera.perspective_NDC[0][0] << "	" << real_camera.perspective_NDC[0][1] << "	" << real_camera.perspective_NDC[0][2] << "	" << real_camera.perspective_NDC[0][3] << "	" << std::endl;
			std::cout << "	" << real_camera.perspective_NDC[1][0] << "	" << real_camera.perspective_NDC[1][1] << "	" << real_camera.perspective_NDC[1][2] << "	" << real_camera.perspective_NDC[1][3] << "	" << std::endl;
			std::cout << "	" << real_camera.perspective_NDC[2][0] << "	" << real_camera.perspective_NDC[2][1] << "	" << real_camera.perspective_NDC[2][2] << "	" << real_camera.perspective_NDC[2][3] << "	" << std::endl;
			std::cout << "	" << real_camera.perspective_NDC[3][0] << "	" << real_camera.perspective_NDC[3][1] << "	" << real_camera.perspective_NDC[3][2] << "	" << real_camera.perspective_NDC[3][3] << "	" << std::endl;

			std::cout << "perspective intrinsics:" << std::endl;
			std::cout << "	" << real_camera.perspective_matrix[0][0] << "	" << real_camera.perspective_matrix[0][1] << "	" << real_camera.perspective_matrix[0][2] << "	" << std::endl;
			std::cout << "	" << real_camera.perspective_matrix[1][0] << "	" << real_camera.perspective_matrix[1][1] << "	" << real_camera.perspective_matrix[1][2] << "	" << std::endl;
			std::cout << "	" << real_camera.perspective_matrix[2][0] << "	" << real_camera.perspective_matrix[2][1] << "	" << real_camera.perspective_matrix[2][2] << "	" << std::endl;

			std::cout << "ortho ortho:" << std::endl;
			std::cout << "	" << real_camera.glortho[0][0] << "	" << real_camera.glortho[0][1] << "	" << real_camera.glortho[0][2] << "	" << real_camera.glortho[0][3] << "	" << std::endl;
			std::cout << "	" << real_camera.glortho[1][0] << "	" << real_camera.glortho[1][1] << "	" << real_camera.glortho[1][2] << "	" << real_camera.glortho[1][3] << "	" << std::endl;
			std::cout << "	" << real_camera.glortho[2][0] << "	" << real_camera.glortho[2][1] << "	" << real_camera.glortho[2][2] << "	" << real_camera.glortho[2][3] << "	" << std::endl;
			std::cout << "	" << real_camera.glortho[3][0] << "	" << real_camera.glortho[3][1] << "	" << real_camera.glortho[3][2] << "	" << real_camera.glortho[3][3] << "	" << std::endl;

			std::cout << "old projection:" << std::endl;
			std::cout << "	" << projection[0][0] << "	" << projection[0][1] << "	" << projection[0][2] << "	" << projection[0][3] << "	" << std::endl;
			std::cout << "	" << projection[1][0] << "	" << projection[1][1] << "	" << projection[1][2] << "	" << projection[1][3] << "	" << std::endl;
			std::cout << "	" << projection[2][0] << "	" << projection[2][1] << "	" << projection[2][2] << "	" << real_camera.perspective_matrix[2][3] << "	" << std::endl;
			std::cout << "	" << real_camera.perspective_matrix[3][0] << "	" << real_camera.perspective_matrix[3][1] << "	" << real_camera.perspective_matrix[3][2] << "	" << real_camera.perspective_matrix[3][3] << "	" << std::endl;

			//std::cout <<"object: "<< object_model[0][0] << std::endl;

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

			boundingbox.generate_bounding_box_3d_2d(projection, camera, object_model, int(SCR_WIDTH), int(SCR_HEIGHT));

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

			object_setting_for_fragment_shader obstacles, cones, spheres, donass;

			cube = glm::mat4(1.0f);
			//glm::vec3 cube_position = set_random_with_distribution(random_number_generator, 0.0, 0.2, 0.05);							//0.03, 0.02			//cube position
			//float scale = random_float(random_number_generator, obstacles_scale_factor[0], obstacles_scale_factor[1]);
			//std::vector<float> cube_angle = rotate_object_3axis_randomly(cube, random_number_generator);
			//cube = glm::translate(cube, cube_position);
			//cube = glm::scale(cube, glm::vec3(scale));
			glm::mat4 cylinder = glm::mat4(1.0f);
			glm::mat4 cone = glm::mat4(1.0f);
			glm::mat4 sphere = glm::mat4(1.0f);
			glm::mat4 donas = glm::mat4(1.0f);


			cube = model_matrix_generator(cube, random_number_generator, obstacles_scale_factor, cube_position);		//angle min,max, traslation xy_mean, zmean, sigma
			cone = model_matrix_generator(cone, random_number_generator, obstacles_scale_factor2, cone_position);
			sphere = model_matrix_generator(sphere, random_number_generator, obstacles_scale_factor2, sphere_position);
			donas = model_matrix_generator(donas, random_number_generator, obstacles_scale_factor2, donas_position);

			if (!ground_truth)
			{
				obstacles = random_object_color(obstacles, random_number_generator, distractor_ambient, distractor_diffuse, distractor_specular, train_shininess);

				//multiple_lightning_shader.use();
				//multiple_lightning_shader.setMatrix4fv("model", cube);
				//multiple_lightning_shader.setVector3f("material.ambient", obstacles.ambient);
				//multiple_lightning_shader.setVector3f("material.diffuse", obstacles.diffuse);
				//multiple_lightning_shader.setVector3f("material.specular", obstacles.specular);
				//multiple_lightning_shader.setFloat("material.shininess", obstacles.shininess);
				setting_object_properties_in_shader(multiple_lightning_shader, obstacles, cube);
				ReferenceObject.Draw(multiple_lightning_shader);
				cones = random_object_color(cones, random_number_generator, distractor_ambient, distractor_diffuse, distractor_specular, train_shininess);
				//multiple_lightning_shader.use();
				//multiple_lightning_shader.setMatrix4fv("model", cone);
				//multiple_lightning_shader.setVector3f("material.ambient", cones.ambient);
				//multiple_lightning_shader.setVector3f("material.diffuse", cones.diffuse);
				//multiple_lightning_shader.setVector3f("material.specular", cones.specular);
				//multiple_lightning_shader.setFloat("material.shininess", cones.shininess);
				setting_object_properties_in_shader(multiple_lightning_shader, cones, cone);
				ConeObject.Draw(multiple_lightning_shader);

				donass = random_object_color(donass, random_number_generator, distractor_ambient, distractor_diffuse, distractor_specular, train_shininess);
				//multiple_lightning_shader.use();
				//multiple_lightning_shader.setMatrix4fv("model", donas);
				//multiple_lightning_shader.setVector3f("material.ambient", donass.ambient);
				//multiple_lightning_shader.setVector3f("material.diffuse", donass.diffuse);
				//multiple_lightning_shader.setVector3f("material.specular", donass.specular);
				//multiple_lightning_shader.setFloat("material.shininess", donass.shininess);
				setting_object_properties_in_shader(multiple_lightning_shader, donass, donas);
				DonasObject.Draw(multiple_lightning_shader);

				spheres = random_object_color(spheres, random_number_generator, distractor_ambient, distractor_diffuse, distractor_specular, train_shininess);
				//multiple_lightning_shader.use();
				//multiple_lightning_shader.setMatrix4fv("model", sphere);
				//multiple_lightning_shader.setVector3f("material.ambient", spheres.ambient);
				//multiple_lightning_shader.setVector3f("material.diffuse", spheres.diffuse);
				//multiple_lightning_shader.setVector3f("material.specular", spheres.specular);
				//multiple_lightning_shader.setFloat("material.shininess", spheres.shininess);
				setting_object_properties_in_shader(multiple_lightning_shader, spheres, sphere);
				SphereObject.Draw(multiple_lightning_shader);

			}
			else
			{
				dump_iterating(random_number_generator, 4, distractor_ambient, distractor_diffuse, distractor_specular, train_shininess);
				Segmentation.use();
				glm::vec3 train_object_color(0.0f, 0.0f, 0.0f);

				Segmentation.setMatrix4fv("view", camera);
				Segmentation.setMatrix4fv("projection", projection);
				Segmentation.setMatrix4fv("model", cube);
				Segmentation.setVector3f("fragcolor", train_object_color);
				ReferenceObject.Draw(Segmentation);//obstacles																	//draw obstacles
				Segmentation.use();
				Segmentation.setMatrix4fv("model", cone);
				ConeObject.Draw(Segmentation);//obstacle
				Segmentation.use();
				Segmentation.setMatrix4fv("model", donas);
				DonasObject.Draw(Segmentation);//obstacle
				Segmentation.use();
				Segmentation.setMatrix4fv("model", sphere);
				SphereObject.Draw(Segmentation);//obstacle
			}
			std::string json_path = JSON_LABEL;
			///////////////////////////////////////semantic segmentation///////////////////
			//Segmentation.use();
			//Segmentation.setMatrix4fv("view", camera);
			//Segmentation.setMatrix4fv("projection", projection);
			//Segmentation.setMatrix4fv("model", cube);
			//Segmentation.setVector3f("fragcolor", obstacles.diffuse);
			//inverse_object_3axis_rotation(cube, cube_angle);
			//cube = glm::translate(cube, -cube_position);
			/////////////////////////////////////////////////////////////////////////////////


			if (ground_truth)
			{
				picture = MASK_DATA_PATH;//"E:/data/pose_estimation/gt/.jpg";//"E:/data/noobstacles_gt/gt/.jpg";//"E:/data/single_object2/gt/.jpg";
			}
			picture.insert(picture.find_last_of('/') + 1, number);
			std::cout << picture << std::endl;
			//generate_json_label(json_path, i, object_model, camera, projection, boundingbox.bb);
			//screenshot_freeimage(picture.c_str(), SCR_WIDTH, SCR_HEIGHT);

			GLCall(glfwSwapBuffers(window));
			GLCall(glfwPollEvents());

			std::cin.get();

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
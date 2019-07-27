#include "utils.h"

std::string to_format(const int number) {
	std::stringstream ss;
	ss << std::setw(5) << std::setfill('0') << number;																	//change file name digit here
	return ss.str();
}

glm::vec3 set_random_position(int range)
{
	/*
		use std library to generate random numbers
		Input range: by given range the generated number will be located in range [0,range]
		return: a 3d-vector, wich convert the integer number to floats.
		set seed use set srand(i) i=1,2,3,4,5.. this will be needed out side the function
	*/

	int array[3];
	for (int j = 0; j < 3; j++)
	{
		array[j] = rand() % range;
	}
	return glm::vec3{ array[0] / 100.f,array[1] / 100.f,array[2] / 100.f };
}

glm::vec3 set_random_with_distribution(std::default_random_engine &generator, float xy, float z, float sigma)
{
	/*
		use <random> library to generate random numbers, in this case a better random generator with 
		certain distribution can be used
		Input generator: a random number generator, need tobe initialized outside this funciton, in order to set the random seed, generator.seed()
			  sigma: this variance of the distribution, default normal distribution is unbiased
		return: a 3d-vector, wich convert the integer number to floats.
		set seed use set srand(i) i=1,2,3,4,5.. this will be needed out side the function
	*/

	std::normal_distribution<double> distribution_xy(xy, sigma);
	std::normal_distribution<double> distribution_z(z, sigma);

	std::vector<float> position;
	position.push_back(distribution_xy(generator));
	position.push_back(distribution_xy(generator));
	position.push_back(distribution_z(generator));

	return glm::vec3(position[0], position[1], position[2]);
}

glm::vec3 set_random_with_distribution(std::default_random_engine &generator, float xy, float z, float sigma, float sigma2)
{
	/*
		use <random> library to generate random numbers, in this case a better random generator with
		certain distribution can be used
		Input generator: a random number generator, need tobe initialized outside this funciton, in order to set the random seed, generator.seed()
			  sigma: this variance of the distribution, default normal distribution is unbiased
		return: a 3d-vector, wich convert the integer number to floats.
		set seed use set srand(i) i=1,2,3,4,5.. this will be needed out side the function
	*/

	std::normal_distribution<double> distribution_xy(xy, sigma);
	std::normal_distribution<double> distribution_z(z, sigma2);

	std::vector<float> position;
	position.push_back(distribution_xy(generator));
	position.push_back(distribution_xy(generator));
	position.push_back(distribution_z(generator));

	return glm::vec3(position[0], position[1], position[2]);
}

glm::vec3 random_v3_norm(std::default_random_engine &generator, float a, float b, float c, float d)
{

	std::normal_distribution<float> distribution_a(a, d);
	std::normal_distribution<float> distribution_b(b, d);
	std::normal_distribution<float> distribution_c(c, d);

	std::vector<float> position;
	position.push_back(distribution_a(generator));
	position.push_back(distribution_b(generator));
	position.push_back(distribution_c(generator));

	//std::cout << "position: "<< position[0] <<" "<< position[1]<<" "<< position[2] << std::endl;

	return glm::vec3(position[0], position[1], position[2]);
}

glm::vec3 random_vec3(std::default_random_engine &generator, float xy_min, float xy_max, float z_min, float z_max)
{

	std::uniform_real_distribution<float> distribution_xy(xy_min, xy_max);
	std::uniform_real_distribution<float> distribution_z(z_min, z_max);
	std::vector<float> position;
	position.push_back(distribution_xy(generator));
	position.push_back(distribution_xy(generator));
	position.push_back(distribution_z(generator));

	//std::cout << "position: "<< position[0] <<" "<< position[1]<<" "<< position[2] << std::endl;

	return glm::vec3(position[0],position[1],position[2]);
}

float random_float(std::default_random_engine &generator, float v_min, float v_max)
{
	std::uniform_real_distribution<float> distribution(v_min, v_max);
	return distribution(generator);
}




//input: pitch and yaw angle, and the distance: TODO add roll angle in this function
//return: the camera setup, used to initialize the gllookAt() function to initialize the camera object
CameraOrientation rotateCamera(int P, int Y, float distance)
{
	//std::cout << "Camera rotation enabled!" << std::endl;

	CameraOrientation setup;
	//std::cout << "Y= " << Y << std::endl;
	float x_direction = distance * glm::cos(glm::radians((float)P)) * cos(glm::radians((float)Y));
	float y_direction = distance * glm::sin(glm::radians((float)P));
	float z_direction = distance * glm::cos(glm::radians((float)P)) * sin(glm::radians((float)Y));

	setup.camera_pose = glm::vec3(x_direction, y_direction, z_direction);
	//std::cout << "camera_pose: " << setup.camera_pose[0] << " " << setup.camera_pose[1] << " " << setup.camera_pose[2] << " " << std::endl;
	glm::vec3 camera_pose_xz = glm::vec3(setup.camera_pose.x, 0.0f, setup.camera_pose.z);
	setup.camera_front = glm::normalize(-setup.camera_pose);
	//std::cout << "camera_front: " << setup.camera_front[0] << " " << setup.camera_front[1] << " " << setup.camera_front[2] << " " << std::endl;
	setup.camera_up;

	if (P == 0)
		setup.camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

	if ((0 < P&&P <= 90) || (180 <= P && P <= 270) || P == 360)
	{
		setup.camera_up = glm::normalize(glm::cross(glm::cross(setup.camera_pose, camera_pose_xz), glm::normalize(setup.camera_front)));
		//std::cout << "camera up: " << setup.camera_up[0] << " " << setup.camera_up[1] << " " << setup.camera_up[1] << std::endl;
	}

	if ((90 < P&&P < 180) || (270 < P&&P < 360))
	{
		setup.camera_up = glm::normalize(glm::cross(glm::cross(setup.camera_pose, camera_pose_xz), glm::normalize(setup.camera_pose)));
		//std::cout << "camera up: " << setup.camera_up[0] << " " << setup.camera_up[1] << " " << setup.camera_up[1] << std::endl;
	}

	return setup;
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

std::vector<float> rotate_object_3axis_randomly(glm::mat4 &model, std::default_random_engine &generator)
{
	//model = glm::rotate(model, glm::radians(velocity), glm::vec3(0.0f, 0.0f, 0.0f));
	//std::cout << "Object Rotation Enabled!" << std::endl;
	std::uniform_real_distribution<float> distribution(0,6.28);
	std::vector<float> random_angle;
	random_angle.push_back(distribution(generator));
	random_angle.push_back(distribution(generator));
	random_angle.push_back(distribution(generator));

	model = glm::rotate(model, random_angle[0], glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, random_angle[1], glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, random_angle[2], glm::vec3(0.0f, 0.0f, 1.0f));
	/*
	std::cout << "	" << model[0][0] << "	" << model[0][1] << "	" << model[0][2] << "	" << model[0][3] << "	" << std::endl;
	std::cout << "	" << model[1][0] << "	" << model[1][1] << "	" << model[1][2] << "	" << model[1][3] << "	" << std::endl;
	std::cout << "	" << model[2][0] << "	" << model[2][1] << "	" << model[2][2] << "	" << model[2][3] << "	" << std::endl;
	std::cout << "	" << model[3][0] << "	" << model[3][1] << "	" << model[3][2] << "	" << model[3][3] << "	" << std::endl;
	*/

	return random_angle;
}

std::vector<float> rotate_object_3axis_randomly(glm::mat4 &model, std::default_random_engine &generator, float min,float max)
{
	//model = glm::rotate(model, glm::radians(velocity), glm::vec3(0.0f, 0.0f, 0.0f));
	//std::cout << "Object Rotation Enabled!" << std::endl;
	std::uniform_real_distribution<float> distribution(min, max);
	std::vector<float> random_angle;
	random_angle.push_back(distribution(generator));
	random_angle.push_back(distribution(generator));
	random_angle.push_back(distribution(generator));

	model = glm::rotate(model, random_angle[0], glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, random_angle[1], glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, random_angle[2], glm::vec3(0.0f, 0.0f, 1.0f));

	return random_angle;
}

void inverse_object_3axis_rotation(glm::mat4 &model, std::vector<float> random_angle)
{
	model = glm::rotate(model, random_angle[2], glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, random_angle[1], glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, random_angle[0], glm::vec3(1.0f, 0.0f, 0.0f));
}

void rotate_light(glm::mat4 &light_model, int P, int Y, float distance)
{
	float x_direction = distance * glm::cos(glm::radians((float)P))*cos(glm::radians((float)Y));
	float y_direction = distance * glm::sin(glm::radians((float)P));
	float z_direction = -distance * glm::cos(glm::radians((float)P))*sin(glm::radians((float)Y));

	light_model = glm::translate(light_model, glm::vec3{ x_direction,y_direction,z_direction });
}

//input: screen width, screen height, window name
//return: a window object used for ex. while loop
GLFWwindow* initialize_window(int width, int height, const char* name)
{
	GLFWwindow* window;
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	window = glfwCreateWindow(width, height, name, NULL, NULL);

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

void screenshot_freeimage(const char* screenshotFile, int width, int height) {

	// Make the BYTE array, factor of 3 because it's RBG.
	BYTE* pixels = new BYTE[3 * width * height];

	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

	// Convert to FreeImage format & save to file
	//FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
	FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
	FreeImage_Save(FIF_JPEG, image, screenshotFile, 0);

	// Free resources
	FreeImage_Unload(image);
	delete[] pixels;
}


void generate_yaml_label(std::ofstream &jsonfile, std::string json_path,json labels, std::string object_path, BoundingBox::bounding_box bb,int P,int Y,int R)
{
	int n = 100 * P + 10 * Y + R;
	std::string PYR=std::to_string(n);
	//std::cout << "PYR" << PYR << std::endl;
		
	labels["object_id"] = object_path.substr(0, object_path.find_last_of('/'));
	labels[PYR]["bb"]["x"] = bb.x;
	labels[PYR]["bb"]["y"] = bb.y;
	labels[PYR]["bb"]["w"] = bb.w;
	labels[PYR]["bb"]["h"] = bb.h;
	jsonfile << labels;


}

std::vector<float> projection_single_point_on_creen(glm::vec3 point, glm::mat4 model, glm::mat4 camera, glm::mat4 projection)
{
	std::vector<float> coordinate;
	glm::vec4 projected_point;
	projected_point = projection * camera* model * glm::vec4(point,1.0f);
	std::cout << projected_point[0]<<" " << projected_point[1]<<" "<<projected_point[2]<<" "<<projected_point[3]<<std::endl;
	glm::vec4 camera_coordinate = camera * model*glm::vec4(point, 1.0f);
	coordinate.push_back((projected_point[0] / projected_point[3]+1)/2);
	coordinate.push_back((1-projected_point[1] / projected_point[3])/2);
	//coordinate.push_back(projected_point[2] / projected_point[3]);
	coordinate.push_back(camera_coordinate[2] / camera_coordinate[3]);

	return coordinate;
}

//use to calculate the bounding box in pixels
//input: (x_min,y_min) lower left origin, (x_max,y_max) upper right corner, screen width, screen height
//return: a bounding box structure contains, origin, width and height of the bounding box in pixels
BoundingBox::BoundingBox(Model train_object)
{
	
	generate_bounding_box_3d(train_object);
	
}

BoundingBox::~BoundingBox()
{
	//delete BB_3d;
	//delete BB_2d;
}

//void BoundingBox::construct_BB_2d(float* bounding_box_vertex_4point)
//{
//	std::map<std::string, int> AttribPointer_BB;
//	AttribPointer_BB["layout_0"] = 0;
//	AttribPointer_BB["size_of_vertex_0"] = 3;
//	AttribPointer_BB["stride_0"] = 3 * sizeof(float);
//	AttribPointer_BB["offset_0"] = 0;
//	BB_2d = new VertexBuffer(bounding_box_vertex_4point,
//		bounding_box_vertex_4point_indecies,
//		sizeof(bounding_box_vertex_4point) / sizeof(bounding_box_vertex_4point),
//		sizeof(float),
//		sizeof(bounding_box_vertex_4point_indecies) / sizeof(bounding_box_vertex_4point_indecies[0]),
//		sizeof(int),
//		AttribPointer_BB,
//		"bb");
//
//}

//void BoundingBox::construct_BB_3d(float* bounding_box_vertex_8point)
//{
//
//	std::map<std::string, int> AttribPointer_BB;
//	AttribPointer_BB["layout_0"] = 0;
//	AttribPointer_BB["size_of_vertex_0"] = 3;
//	AttribPointer_BB["stride_0"] = 3 * sizeof(float);
//	AttribPointer_BB["offset_0"] = 0;
//	std::cout << "in construct bb 3d" << bounding_box_vertex_8point<< std::endl;
//	BB_3d = new VertexBuffer(bounding_box_vertex_8point,
//		bounding_box_vertex_8point_indecies,
//		sizeof(bounding_box_vertex_8point) / sizeof(bounding_box_vertex_8point),
//		sizeof(float),
//		sizeof(bounding_box_vertex_8point_indecies) / sizeof(bounding_box_vertex_8point_indecies[0]),
//		sizeof(int),
//		AttribPointer_BB,
//		"bb");
//
//	
//	std::cout << "construction finished" << std::endl;
//}

void BoundingBox::calculate_boundingbox(float x_min,
								float x_max,
								float y_min,
								float y_max,
								int screen_w,
								int screen_h)
{
	//calculate the coordinate with regard to lower left origin of the whole picture
	bb.x = int(0.5*screen_w*x_min+screen_w/2);
	bb.y = int(0.5*screen_h*y_min+screen_h/2);
	bb.w = int(screen_w*(x_max - x_min)/2);
	bb.h = int(screen_h*(y_max - y_min)/2);
	//transform to top left coordinate
	bb.x_min = (0.5*screen_w*x_min + screen_w / 2)/screen_w;
	bb.y_min = (screen_h/2 - 0.5*screen_h*y_max)/screen_h;
	bb.x_max = (0.5*screen_w*x_max + screen_w / 2)/screen_w;
	bb.y_max = (screen_h/2 - 0.5*screen_h*y_min)/screen_h;


}



//use to generate the bounding box labels 
//input: a model object, screen width and screen height, the projection matrix, the view(camera) matrix, and model matrix
//return: return a bouding box vertex structure to visualize on the screen.
void BoundingBox::generate_bounding_box_labels_2d(Model train_object, 
									int screen_w, 
									int screen_h,
									int P,
									int Y,
									int R,
									glm::mat4 projection,
									glm::mat4 camera,
									glm::mat4 model,
									std::ofstream &jsonfile,
									std::string json_path)
{
	std::ofstream &jf=jsonfile;
	std::vector<float> U;
	std::vector<float> V;

	int i = 0;
	while (i < train_object.meshes[0].Vertecies.size())
	{
		glm::vec4 vertex_position_on_image = projection * camera * model * glm::vec4(train_object.meshes[0].Vertecies[i].Position, 1.0f);
		//std::cout << "vertex position on image: " << vertex_position_on_image[0]/ vertex_position_on_image[3]
		//			<< " " << vertex_position_on_image[1]/ vertex_position_on_image[3] << " " 
		//			<< vertex_position_on_image[2]/ vertex_position_on_image[3] << " " << vertex_position_on_image[3]/ vertex_position_on_image[3] << std::endl;

		U.push_back(vertex_position_on_image[0] / vertex_position_on_image[3]);
		V.push_back(vertex_position_on_image[1] / vertex_position_on_image[3]);

		i++;
	}
	bb_v.x_max = *std::max_element(U.begin(), U.end());//top right
	bb_v.x_min = *std::min_element(U.begin(), U.end());//buttom right
	bb_v.y_max = *std::max_element(V.begin(), V.end());//buttom left
	bb_v.y_min = *std::min_element(V.begin(), V.end());
	//std::cout << "delta x: " << bb_X_min<<" "<<bb_X_max-bb_X_min << std::endl;

	calculate_boundingbox(bb_v.x_min, bb_v.x_max, bb_v.y_min, bb_v.y_max, screen_w, screen_h);

	json bb_labels;
	generate_yaml_label(jf,json_path,bb_labels,"mesh/obj_05.stl",bb,P,Y,R);
	//std::cout <<"bounding box: "<< bb.x << " " << bb.y << " " << bb.w << " " << bb.h << std::endl;


}

void BoundingBox::generate_bounding_box_2d(Model train_object, glm::mat4 projection, glm::mat4 camera, glm::mat4 model, int screen_w, int screen_h)//run only once, once object is created
{
	std::vector<float> U;
	std::vector<float> V;

	int i = 0;
	while (i < train_object.meshes[0].Vertecies.size())
	{
		glm::vec4 vertex_position_on_image = projection * camera * model * glm::vec4(train_object.meshes[0].Vertecies[i].Position, 1.0f);
		//std::cout << "vertex position on image: " << vertex_position_on_image[0]/ vertex_position_on_image[3]
		//			<< " " << vertex_position_on_image[1]/ vertex_position_on_image[3] << " " 
		//			<< vertex_position_on_image[2]/ vertex_position_on_image[3] << " " << vertex_position_on_image[3]/ vertex_position_on_image[3] << std::endl;

		U.push_back(vertex_position_on_image[0] / vertex_position_on_image[3]);
		V.push_back(vertex_position_on_image[1] / vertex_position_on_image[3]);

		i++;
	}
	bb_v.x_max = *std::max_element(U.begin(), U.end());//top right
	bb_v.x_min = *std::min_element(U.begin(), U.end());//buttom right
	bb_v.y_max = *std::max_element(V.begin(), V.end());//buttom left
	bb_v.y_min = *std::min_element(V.begin(), V.end());
	//std::cout << "delta x: " << bb_X_min<<" "<<bb_X_max-bb_X_min << std::endl;

	calculate_boundingbox(bb_v.x_min, bb_v.x_max, bb_v.y_min, bb_v.y_max, screen_w, screen_h);
}

void BoundingBox::generate_bounding_box_3d(Model train_object)//run only once, once object is created
{
	std::vector<float> U;
	std::vector<float> V;
	std::vector<float> W;
	int i = 0;
	while (i < train_object.meshes[0].Vertecies.size())
	{
		//if (std::abs(train_object.meshes[0].Vertecies[i].Position[0] <= 1))//check for outlier
		U.push_back(train_object.meshes[0].Vertecies[i].Position[0]);
		//if (std::abs(train_object.meshes[0].Vertecies[i].Position[1] <= 1))
		V.push_back(train_object.meshes[0].Vertecies[i].Position[1]);
		//if (std::abs(train_object.meshes[0].Vertecies[i].Position[2] <= 1))
		W.push_back(train_object.meshes[0].Vertecies[i].Position[2]);
		i++;
	}

	bb_v_3d.x_max = *std::max_element(U.begin(), U.end());
	bb_v_3d.x_min = *std::min_element(U.begin(), U.end());
	bb_v_3d.y_max = *std::max_element(V.begin(), V.end());
	bb_v_3d.y_min = *std::min_element(V.begin(), V.end());
	bb_v_3d.z_max = *std::max_element(W.begin(), W.end());
	bb_v_3d.z_min = *std::min_element(W.begin(), W.end());
	//std::cout << " bounding box 3d: "<< bb_v_3d.x_max << std::endl;

}

void BoundingBox::generate_bounding_box_3d_2d(glm::mat4 projection, glm::mat4 camera, glm::mat4 model, int screen_w, int screen_h)//run only once, once object is created
{
	std::vector<float> U;
	std::vector<float> V;
	int i = 0;
	//std::cout << "bounding box" << bounding_box_3d_vertex[5]<<" "<< bounding_box_3d_vertex[1]<<" "<< bounding_box_3d_vertex[2] << std::endl;
	//std::cout << "size of array: " <<sizeof(bounding_box_3d_vertex) << std::endl;
	while (i < bb_glm_vec3.size())
	{
		//do transformation
		glm::vec4 vertex_position_on_image = projection * camera*model*glm::vec4(bb_glm_vec3[i], 1.0f);
		U.push_back(vertex_position_on_image[0] / vertex_position_on_image[3]);
		V.push_back(vertex_position_on_image[1] / vertex_position_on_image[3]);
		i++;
	}

	bb_v.x_max = *std::max_element(U.begin(), U.end());
	bb_v.x_min = *std::min_element(U.begin(), U.end());
	bb_v.y_max = *std::max_element(V.begin(), V.end());
	bb_v.y_min = *std::min_element(V.begin(), V.end());
	calculate_boundingbox(bb_v.x_min, bb_v.x_max, bb_v.y_min, bb_v.y_max, screen_w, screen_h);
}

void BoundingBox::generate_bounding_box_labels_3d(
	int screen_w,
	int screen_h,
	int P,
	int Y,
	int R,
	glm::mat4 projection,
	glm::mat4 camera,
	glm::mat4 model,
	std::ofstream &jsonfile,
	std::string json_path)
{
	std::ofstream &jf = jsonfile;
	std::vector<float> U;
	std::vector<float> V;
	int i = 0;
	//std::cout << "bounding box" << bounding_box_3d_vertex[5]<<" "<< bounding_box_3d_vertex[1]<<" "<< bounding_box_3d_vertex[2] << std::endl;
	//std::cout << "size of array: " <<sizeof(bounding_box_3d_vertex) << std::endl;
	while (i<bb_glm_vec3.size())
	{
		//do transformation
		glm::vec4 vertex_position_on_image = projection * camera*model*glm::vec4(bb_glm_vec3[i],1.0f);
		U.push_back(vertex_position_on_image[0] / vertex_position_on_image[3]);
		V.push_back(vertex_position_on_image[1] / vertex_position_on_image[3]);
		i++;
	}

	bb_v.x_max = *std::max_element(U.begin(), U.end());
	bb_v.x_min = *std::min_element(U.begin(), U.end());
	bb_v.y_max = *std::max_element(V.begin(), V.end());
	bb_v.y_min = *std::min_element(V.begin(), V.end());
	calculate_boundingbox(bb_v.x_min, bb_v.x_max, bb_v.y_min, bb_v.y_max, screen_w, screen_h);
	//std::cout << "delta x: " << bb_v.x_min<<" "<<bb_v.x_max-bb_v.x_min << std::endl;

	json bb_labels;
	generate_yaml_label(jf, json_path, bb_labels, "mesh/obj_05.stl", bb, P, Y, R);
}


void BoundingBox::fill_bb_glm_vec3(float* bounding_box_vertex_8point)
{
	for (int i = 0; i < 8; i++)
	{
		bb_glm_vec3.push_back(glm::vec3(bounding_box_vertex_8point[3 * i], bounding_box_vertex_8point[3 * i + 1], bounding_box_vertex_8point[3 * i + 2]));
	}
}

//void BoundingBox::draw_3d(Shader shader,glm::mat4 model,glm::mat4 camera,glm::mat4 projection)
//{
//
//	shader.use();
//	shader.setMatrix4fv("model", model);
//	shader.setMatrix4fv("view", camera);
//	shader.setMatrix4fv("projection", projection);
//	GLCall(glPolygonMode(GL_FRONT_AND_BACK,GL_LINE));
//	//BB_3d->Draw("draw_elements");
//}
//
//void BoundingBox::draw_2d(Shader shader, glm::mat4 model, glm::mat4 camera, glm::mat4 projection)
//{
//	//std::cout << "bounding box 2d vertex: " << bounding_box_vertex_4point[1] << std::endl;
//	shader.use();
//	shader.setMatrix4fv("model", model);
//	shader.setMatrix4fv("view", camera);
//	shader.setMatrix4fv("projection", projection);
//	GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
//	//BB_2d->Draw("draw_elements");
//}

inline void calculateRotation(Quaternion& q, glm::mat4 a) {

	float trace = a[0][0] + a[1][1] + a[2][2]; //removed + 1.0f; 
	if (trace > 0) {// I changed M_EPSILON to 0
		float s = 0.5f / sqrtf(trace + 1.0f);
		q.w = 0.25f / s;
		q.x = (a[2][1] - a[1][2]) * s;
		q.y = (a[0][2] - a[2][0]) * s;
		q.z = (a[1][0] - a[0][1]) * s;
	}
	else {
		if (a[0][0] > a[1][1] && a[0][0] > a[2][2]) {
			float s = 2.0f * sqrtf(1.0f + a[0][0] - a[1][1] - a[2][2]);
			q.w = (a[2][1] - a[1][2]) / s;
			q.x = 0.25f * s;
			q.y = (a[0][1] + a[1][0]) / s;
			q.z = (a[0][2] + a[2][0]) / s;
		}
		else if (a[1][1] > a[2][2]) {
			float s = 2.0f * sqrtf(1.0f + a[1][1] - a[0][0] - a[2][2]);
			q.w = (a[0][2] - a[2][0]) / s;
			q.x = (a[0][1] + a[1][0]) / s;
			q.y = 0.25f * s;
			q.z = (a[1][2] + a[2][1]) / s;
		}
		else {
			float s = 2.0f * sqrtf(1.0f + a[2][2] - a[0][0] - a[1][1]);
			q.w = (a[1][0] - a[0][1]) / s;
			q.x = (a[0][2] + a[2][0]) / s;
			q.y = (a[1][2] + a[2][1]) / s;
			q.z = 0.25f * s;
		}
	}
}

void Arrow(GLdouble x1, GLdouble y1, GLdouble z1, GLdouble x2, GLdouble y2, GLdouble z2, GLdouble D)
{
	double x = x2 - x1;
	double y = y2 - y1;
	double z = z2 - z1;
	double L = sqrt(x*x + y * y + z * z);

	GLUquadricObj *quadObj;

	glPushMatrix();

	glTranslated(x1, y1, z1);

	if ((x != 0.) || (y != 0.)) {
		glRotated(atan2(y, x) / RADPERDEG, 0., 0., 1.);
		glRotated(atan2(sqrt(x*x + y * y), z) / RADPERDEG, 0., 1., 0.);
	}
	else if (z < 0) {
		glRotated(180, 1., 0., 0.);
	}

	glTranslatef(0, 0, L - 4 * D);

	quadObj = gluNewQuadric();
	gluQuadricDrawStyle(quadObj, GLU_FILL);
	gluQuadricNormals(quadObj, GLU_SMOOTH);
	gluCylinder(quadObj, 2 * D, 0.0, 4 * D, 32, 1);
	gluDeleteQuadric(quadObj);

	quadObj = gluNewQuadric();
	gluQuadricDrawStyle(quadObj, GLU_FILL);
	gluQuadricNormals(quadObj, GLU_SMOOTH);
	gluDisk(quadObj, 0.0, 2 * D, 32, 1);
	gluDeleteQuadric(quadObj);

	glTranslatef(0, 0, -L + 4 * D);

	quadObj = gluNewQuadric();
	gluQuadricDrawStyle(quadObj, GLU_FILL);
	gluQuadricNormals(quadObj, GLU_SMOOTH);
	gluCylinder(quadObj, D, D, L - 4 * D, 32, 1);
	gluDeleteQuadric(quadObj);

	quadObj = gluNewQuadric();
	gluQuadricDrawStyle(quadObj, GLU_FILL);
	gluQuadricNormals(quadObj, GLU_SMOOTH);
	gluDisk(quadObj, 0.0, D, 32, 1);
	gluDeleteQuadric(quadObj);

	glPopMatrix();

}

void drawAxes(GLdouble length)
{
	glPushMatrix();
	glTranslatef(-length, 0, 0);
	Arrow(0, 0, 0, 2 * length, 0, 0, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -length, 0);
	Arrow(0, 0, 0, 0, 2 * length, 0, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -length);
	Arrow(0, 0, 0, 0, 0, 2 * length, 0.2);
	glPopMatrix();
}

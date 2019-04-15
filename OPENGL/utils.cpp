#include "utils.h"

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
	bb.w = int(screen_w*(x_max - x_min));
	bb.h = int(screen_h*(y_max - y_min));

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

	calculate_boundingbox(bb_v.x_max, bb_v.x_min, bb_v.y_max, bb_v.y_min, screen_w, screen_h);

	json bb_labels;
	generate_yaml_label(jf,json_path,bb_labels,"mesh/obj_05.stl",bb,P,Y,R);
	//std::cout <<"bounding box: "<< bb.x << " " << bb.y << " " << bb.w << " " << bb.h << std::endl;


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
	calculate_boundingbox(bb_v.x_max, bb_v.x_min, bb_v.y_max, bb_v.y_min, screen_w, screen_h);
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
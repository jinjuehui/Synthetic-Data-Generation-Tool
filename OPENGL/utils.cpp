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



//use to calculate the bounding box in pixels
//input: (x_min,y_min) lower left origin, (x_max,y_max) upper right corner, screen width, screen height
//return: a bounding box structure contains, origin, width and height of the bounding box in pixels
bounding_box caculate_boundingbox(float x_min,
								float x_max,
								float y_min,
								float y_max,
								int screen_w,
								int screen_h)
{
	//calculate the coordinate with regard to lower left origin of the whole picture
	bounding_box bb;
	bb.x = int(0.5*screen_w*x_min+screen_w/2);
	bb.y = int(0.5*screen_h*y_min+screen_h/2);
	bb.w = int(screen_w*(x_max - x_min));
	bb.h = int(screen_h*(y_max - y_min));
	return bb;

}


//use to generate the bounding box labels 
//input: a model object, screen width and screen height, the projection matrix, the view(camera) matrix, and model matrix
//return: return a bouding box vertex structure to visualize on the screen.
bounding_box_vertex generate_bounding_box_labels(Model train_object, 
									int screen_w, 
									int screen_h, 
									glm::mat4 projection,
									glm::mat4 camera,
									glm::mat4 model)
{

	std::vector<float> U;
	std::vector<float> V;

	int i = 0;
	while (i < train_object.meshes[0].Vertecies.size())
	{
		glm::vec4 vertex_position_on_image = projection * camera * model * glm::vec4(train_object.meshes[0].Vertecies[i].Position, 1.0f);
		U.push_back(vertex_position_on_image[0] / vertex_position_on_image[3]);
		//std::cout << "vertex position on image: " << vertex_position_on_image[0]/ vertex_position_on_image[3]
		//			<< " " << vertex_position_on_image[1]/ vertex_position_on_image[3] << " " 
		//			<< vertex_position_on_image[2]/ vertex_position_on_image[3] << " " << vertex_position_on_image[3]/ vertex_position_on_image[3] << std::endl;

		V.push_back(vertex_position_on_image[1] / vertex_position_on_image[3]);
		i++;
	}
	float bb_X_max = *std::max_element(U.begin(), U.end());
	float bb_X_min = *std::min_element(U.begin(), U.end());
	//std::cout << "delta x: " << bb_X_min<<" "<<bb_X_max-bb_X_min << std::endl;
	float bb_Y_max = *std::max_element(V.begin(), V.end());
	float bb_Y_min = *std::min_element(V.begin(), V.end());


	bounding_box_vertex BoundingBox;

	BoundingBox.x_max = bb_X_max;	// top right
	BoundingBox.x_min = bb_X_min;  // bottom right
	BoundingBox.y_max = bb_Y_max;  // bottom left
	BoundingBox.y_min = bb_Y_min;


	bounding_box bb = caculate_boundingbox(bb_X_min, bb_X_max, bb_Y_min, bb_Y_max, screen_w, screen_h);
	//std::cout <<"bounding box: "<< bb.x << " " << bb.y << " " << bb.w << " " << bb.h << std::endl;

	return BoundingBox;

}
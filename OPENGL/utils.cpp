#include "utils.h"








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


bounding_box caculate_bounbox(float x_min,
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
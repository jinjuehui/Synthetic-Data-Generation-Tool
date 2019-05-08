#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <gtc/random.hpp>
#include <string>
#include "utils.h"

glm::vec3 light_position[] =
{
	glm::ballRand(1.f),
	glm::ballRand(1.f),
	glm::ballRand(1.f)
};

//glm::vec3 set_random_position(int range)
//{
//	/*
//		use std library to generate random numvers
//		Input range: by given range the generated number will be located in range [0,range]
//		return: a 3d-vector, wich convert the integer number to floats.
//
//		set seed use set srand(i) i=1,2,3,4,5.. this will be needed out side the function
//	*/
//	int array[3];
//	for (int j = 0; j < 3; j++)
//	{
//		array[j] = rand() % range;
//	}
//	return glm::vec3{ array[0]/100.f,array[1]/100.f,array[2]/100.f };
//}

//int main()
//{
//	int i = 0;
//	while(i<=3)
//	{
//		srand(1);
//		for (int i = 0; i < 2; i++)
//		{
//			for (int j = 0; j < 2; j++)
//			{
//				glm::vec3 position = set_random_position(100);
//				std::cout << "position: " << position.x << std::endl;
//				std::cout << "position: " << position.y << std::endl;
//				std::cout << "position: " << position.z << std::endl;
//				std::cin.get();
//			}
//			std::cout << "=========== first for loop end=============" << std::endl;
//
//		}
//		std::cout << "===========for loop end=============" << std::endl;
//		i++;
// 
//	}
//}

double generate_random_number(std::default_random_engine &generator, float bias, float sigma)
{
	std::normal_distribution<double> distribution(bias, sigma);
	return distribution(generator);
}

int main()
{

	std::default_random_engine generator;
	//std::normal_distribution<double> distribution(5.0, 2.0);

	while(true)
	{
		generator.seed(1);
		for (int i = 0; i < 4; ++i) {
			double number = random_float(generator, 0.0,0.5);
			glm::vec3 v = random_vec3(generator, 1.0, 2.0, 1.0, 2.0);
			std::cout << "generated random number: " << number << std::endl;
			std::cout << "random_vec3 check: " << v.x << std::endl;

			std::cin.get();
		}
		std::cout << "==========outer for loop =================== " << std::endl;

	}

	return 0;
}
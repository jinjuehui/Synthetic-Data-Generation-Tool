#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <gtc/random.hpp>


glm::vec3 light_position[] =
{
	glm::ballRand(1.f),
	glm::ballRand(1.f),
	glm::ballRand(1.f)
};

glm::vec3 set_random_position(int range)
{
	int array[3];
	for (int j = 0; j < 3; j++)
	{
		array[j] = rand() % range;
	}
	return glm::vec3{ array[0]/100.f,array[1]/100.f,array[2]/100.f };
}

int main()
{
	int i = 0;
	while(i<=3)
	{
		srand(1);
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				glm::vec3 position = set_random_position(100);
				std::cout << "position: " << position.x << std::endl;
				std::cout << "position: " << position.y << std::endl;
				std::cout << "position: " << position.z << std::endl;
				std::cin.get();
			}
			std::cout << "=========== first for loop end=============" << std::endl;

		}
		std::cout << "===========for loop end=============" << std::endl;
		i++;
 
	}
}
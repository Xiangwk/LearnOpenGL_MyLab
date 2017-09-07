#include <glad\glad.h>

#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

class VertDataLoader
{
public:
	std::vector<Vertex> data;
	std::fstream fin;

	VertDataLoader() = default;

	void loadData(const std::string &filename);
};

void VertDataLoader::loadData(const std::string &filename)
{
	fin.open(filename);
	std::string vertdata;
	while (std::getline(fin, vertdata))
	{
		std::stringstream dataStream(vertdata);
		std::string X, Y, Z, NX, NY, NZ, S, T;
		dataStream >> X >> Y >> Z >> NX >> NY >> NZ >> S >> T;
		//load position
		GLfloat x = std::stof(X), y = std::stof(Y), z = std::stof(Z);
		//load normal
		GLfloat nx = std::stof(NX), ny = std::stof(NY), nz = std::stof(NZ);
		//load texcoord
		GLfloat s = std::stof(S), t = std::stof(T);
		glm::vec3 pos(x, y, z);
		glm::vec3 norm(nx, ny, nz);
		glm::vec2 tex(s, t);
		data.push_back({ pos, norm, tex });
	}
}
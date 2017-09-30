#include <glad\glad.h>

#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <Mesh.h>

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
	if (!data.empty()) data.clear();
	fin.open(filename);
	bool hasNormal, hasTexCoord;
	fin >> hasNormal >> hasTexCoord;
	fin.get();

	glm::vec3 pos, norm;
	glm::vec2 tex;

	std::string vertdata;
	while (std::getline(fin, vertdata))
	{
		std::stringstream dataStream(vertdata);
		std::string X, Y, Z, NX, NY, NZ, S, T;
		dataStream >> X >> Y >> Z;
		//load position
		GLfloat x = std::stof(X), y = std::stof(Y), z = std::stof(Z);
		pos = glm::vec3(x, y, z);
		//load normal
		if (hasNormal)
		{
			dataStream >> NX >> NY >> NZ;
			GLfloat nx = std::stof(NX), ny = std::stof(NY), nz = std::stof(NZ);
			norm = glm::vec3(nx, ny, nz);
		}
		//load texcoord
		if (hasTexCoord)
		{
			dataStream >> S >> T;
			GLfloat s = std::stof(S), t = std::stof(T);
			tex = glm::vec2(s, t);
		}
		data.push_back({ pos, norm, tex });
	}
	fin.close();
}
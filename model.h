#include "geometry.h"
#include "ppm.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>


// LIGHT DATA STRUCTURE
enum light_type  { Point, Spotlight };
enum material_type {Transparent, Glossy, Mirror};

struct light
{
	vec3<float> position;
	light_type type = Point;
	float brightness = 0.5;

	light(vec3<float> pos, light_type t)
	{
		position = pos;
		type = t;
	}

	light() {}
};

// MATERIAL FOR MODEL
struct material
{
	colour col;
	material_type type = Glossy;
	float shininess;
	float reflection = 0; // No reflection by default (0) (1 is mirrored reflection)
	float refraction_coefficient = -1;

	material() {}
	material(colour c, float s, float r)
	{
		col = c;
		shininess = s;
		reflection = r;
	}
};

struct model
{
	std::vector<vec3<float>> vertices; // List of vertices
	std::vector<vec2<float>> texture_coords; // List of texture coordinates

	std::vector<int> triangles; // List of vertices / texture coords making a triangle face
	std::vector<int> tri_tex_coords; // texture Coordinates for triangle

	material mat;
};



model load3dModel(std::string filename)
{
	model m;

	std::ifstream file;
	std::string line;
	std::string temp;

	file.open(filename);
	if (file.is_open())
	{
		vec3<float> v;
		vec2<float> uv;
		int t;

		while (file >> temp)
		{
			if (temp == "v")
			{
				file >> v.x;
				file >> v.y;
				file >> v.z;
				m.vertices.push_back(v);
			}
			else if (temp == "vt")
			{
				file >> uv.x;
				file >> uv.y;
				m.texture_coords.push_back(uv);
			}
			else if (temp == "f")
			{
				file >> temp;
				t = temp.find("/");
				if (t == std::string::npos)
				{
					m.triangles.push_back(std::stoi(temp) - 1);
					m.tri_tex_coords.push_back(-1);
				}
				else
				{
					m.triangles.push_back(std::stoi(temp.substr(0, t)) - 1);
					m.tri_tex_coords.push_back(std::stoi(temp.substr(0, t)));
				}
				file >> temp;
				t = temp.find("/");
				if (t == std::string::npos)
				{
					m.triangles.push_back(std::stoi(temp) - 1);
					m.tri_tex_coords.push_back(-1);
				}
				else
				{
					m.triangles.push_back(std::stoi(temp.substr(0, t)) - 1);
					m.tri_tex_coords.push_back(std::stoi(temp.substr(0, t)));
				}
				file >> temp;
				t = temp.find("/");
				if (t == std::string::npos)
				{
					m.triangles.push_back(std::stoi(temp) - 1);
					m.tri_tex_coords.push_back(-1);
				}
				else
				{
					m.triangles.push_back(std::stoi(temp.substr(0, t)) - 1);
					m.tri_tex_coords.push_back(std::stoi(temp.substr(0, t)));
				}
			}
		}
	}
	else
		std::cout << "Cannot open file: " << filename << std::endl;


	std::cout << "Model loaded: " << filename << std::endl;
	return m;
}
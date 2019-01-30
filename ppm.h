// PPM functionality
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

struct colour
{
	int r;
	int g;
	int b;

	colour(int a, int a1, int a2)
	{
		r = a;
		g = a1;
		b = a2;
	}
	colour(){}

	void operator =(const colour c)
	{
		r = c.r;
		g = c.g;
		b = c.b;
	}

	colour operator +(const colour c)
	{
		return colour(r + c.r, g + c.g, b + c.b);
	}

	void operator +=(const colour c)
	{
		r = r + c.r;
		g = g + c.g;
		b = b + c.b;
	}


	colour operator *(const float f)
	{
		return colour(r * f, g * f, b * f);
	}
};

struct ppm
{
	int height;
	int width;
	std::vector<colour> rgb;

	ppm(int w, int h)
	{
		width = w;
		height = h;
		rgb = std::vector<colour>(w * h); // reserve rgb space
		colour c = { 0, 0, 0 };
		for (int i = 0; i < w * h; i++)
			rgb.at(i) = c;
	}
	ppm()
	{
		width = 0;
		height = 0;
	}

	void setPixel(int x, int y, colour c)
	{
		if (x > width-1 | y > height-1) // Start with bottom row as x/y = 0 so width/height out of bounds
			return;

		// Index rgb from opposite direction as origin in bottom left
		int index = width * (height - 1 - y) + x;
		rgb.at(index) = c;
	}
	colour getPixel(int x, int y)
	{
		int index = height * (width - y) - 1 + x;
		return rgb.at(index);
	}
	colour getTexel(float x, float y)
	{
		x = x * (width - 1);
		y = y * (height - 1);

		int index = width * (height - 1 - (int)y) + (int)x;
		return rgb.at(index);
	}
};



ppm readFile(std::string filename)
{
	std::cout << "Loading PPM File" << std::endl;
	ppm f;
	colour c;

	std::ifstream file;
	std::string line;

	file.open(filename);
	if (file.is_open())
	{
		// Get file type
		std::getline(file, line);

		// Check type but continue regardless with warning
		if (line != "P3")
			std::cout << "File type may be incompatible" << std::endl;

		// Ignore comment
		std::getline(file, line);

		// Get dimensions
		file >> f.width;
		file >> f.height;

		std::cout << "Dimensions: " << f.width << " x " << f.height << std::endl;

		// Ignore max value
		file >> line;
		

		// Loop through colours
		while (file >> c.r >> c.g >> c.b)
		{
			//std::cout << "RGB: " << c.r << " " << c.g << " " << c.b << std::endl;
			f.rgb.push_back(c);
		}
	}
	else
		std::cout << "Cannot open file: " << filename << std::endl;

	std::cout << "Finished Loading PPM File" << std::endl;
	return f;
}

void writeFile(ppm p, std::string filename)
{

	std::ofstream file(filename);

	file << "P3\n";
	file << "#.\n";
	file << p.width << " " << p.height << "\n";
	file << "255\n";

	// Write Colours
	for (int i = 0; i < p.rgb.size(); i++)
	{
		file << p.rgb.at(i).r << " " << p.rgb.at(i).g << " " << p.rgb.at(i).b << "\n";
	}
	
	std::cout << "File written to: " << filename << std::endl;
}
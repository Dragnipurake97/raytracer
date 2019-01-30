#include "math.h"
#include <vector>

// 2D Vector
template <class t> struct vec2
{
public:
	t x;
	t y;

	vec2<t>() {}

	vec2<t>(t a, t b)
	{
		x = a;
		y = b;
	}

	// Overload Operators

	vec2<t> operator =(const t& a) // v = {x, y}
	{
		x = a[0];
		y = a[1];

		return this;
	}

	vec2<t> operator =(const vec2<t>& v) // v = v
	{
		x = v.x;
		y = v.y;

		return *this;
	}

	vec2<t> operator +(const vec2<t>& v)
	{
		return vec2<t>(x + v.x, y + v.y);
	}

	vec2<t> operator -(const vec2<t>& v)
	{
		return vec2<t>(x - v.x, y - v.y);
	}

	vec2<t> operator /(const vec2<t>& v)
	{
		return vec2<t>(x / v.x, y / v.y);
	}

	vec2<t> operator *(const vec2<t>& v)
	{
		return vec2<t>(x * v.x, y * v.y);
	}
	vec2<t> operator *(const float s) // Scalar multiplication
	{
		return vec2<t>(x * s, y * s);
	}
	vec2<t> operator *(const int s) // Scalar multiplication
	{
		return vec2<t>(x * s, y * s);
	}

	t dot(vec2<t> v)
	{
		return x * v.x + y * v.y;
	}
};



// 3D Vector
template <class t> struct vec3
{
public:
	t x;
	t y;
	t z;

	vec3<t>() {}

	vec3<t>(t a, t b, t c)
	{
		x = a;
		y = b;
		z = c;
	}

	// Overload Operators

	vec3<t> operator =(const t& a) // v = {x, y, z}
	{
		x = a[0];
		y = a[1];
		z = a[2];

		return this;
	}

	vec3<t> operator =(const vec3<t>& v) // v = v
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	bool operator ==(const vec3<t>& v) // v = v
	{
		if (x == v.x && y == v.y && z == v.z)
			return true;
		else
			return false;
	}


	vec3<t> operator +(const vec3<t>& v)
	{
		return vec3<t>(x + v.x, y + v.y, z + v.z);
	}

	vec3<t> operator -(const vec3<t>& v)
	{
		return vec3<t>(x - v.x, y - v.y, z - v.z);
	}

	vec3<t> operator *(const vec3<t>& v)
	{
		return vec3<t>(x * v.x, y * v.y, z * v.z);
	}
	vec3<t> operator *(const float s) // Scalar multiplication
	{
		return vec3<t>(x * s, y * s, z * s);
	}

	// Vector Functions
	vec3<t> normalise()
	{
		float magnitude = sqrt(x * x + y * y + z * z);
		return vec3<float>(x / magnitude, y / magnitude, z / magnitude);
	}

	t dot(vec3<t> v)
	{
		return x * v.x + y * v.y + v.z * z;
	}

	vec3<t> cross(vec3<t> v)
	{
		return vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};


// Ray
struct ray
{
	vec3<float> origin;
	vec3<float> direction;
};

// Matrices

template <class t> struct mat3x3
{
public:
	t elements[9];
	int row = 3;
	int column = 3;

	mat3x3<t>() {}

	mat3x3<t>(t e[9])
	{
		for (int i = 0; i < 9; i++)
		{
			elements.push_back(e[i]);
		}
	}
	mat3x3<t>(const vec3<t>& v1, const vec3<t>& v2, const vec3<t>& v3)
	{
		elements[0] = v1.x; elements[1] = v1.y; elements[2] = v1.z;
		elements[3] = v2.x; elements[4] = v2.y; elements[5] = v2.z;
		elements[6] = v3.x; elements[7] = v3.y; elements[8] = v3.z;
	}

	vec3<t> operator *(const vec3<t>& m)
	{
		vec3<t> v;
		v.x = elements[0] * m.x + elements[1] * m.y + elements[2] * m.z;
		v.y = elements[3] * m.x + elements[4] * m.y + elements[5] * m.z;
		v.z = elements[6] * m.x + elements[7] * m.y + elements[8] * m.z;
		return v;
	}
};


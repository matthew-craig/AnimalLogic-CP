#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>

#include "tiny_obj_loader.h"

using namespace boost::geometry;

struct simple_point
{
	simple_point(double x = 0, double y = 0, double z = 0)
		: x(x), y(y), z(z)
	{}
	simple_point operator+(const simple_point& p)
	{
		return simple_point(x + p.x, y + p.y, z + p.z);
	}
	simple_point operator-(const simple_point& p)
	{
		return simple_point(x - p.x, y - p.y, z - p.z);
	}
	simple_point operator*(const simple_point& p)
	{
		return simple_point(x * p.x, y * p.y, z * p.z);
	}
	double x, y, z;
};

struct discrete_point
{
	discrete_point(long a = 0, long b = 0, long c = 0)
		: a(a), b(b), c(c)
	{}
	long a, b, c;
};

class Mesh
{
public:
	Mesh(std::string inputfile);

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
};
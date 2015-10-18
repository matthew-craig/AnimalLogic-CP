#pragma once
//#include <boost/numeric/ublas/vector.hpp>
//#include <boost/numeric/ublas/io.hpp>

#include "Mesh.h"

#include <boost/unordered_map.hpp>
#include <boost/math/special_functions/round.hpp>

//using namespace boost::numeric::ublas;
//using namespace boost::geometry::concept;

/*

vector<double> v(3);
for (unsigned i = 0; i < v.size(); ++i)
	v(i) = i;
std::cout << v << std::endl;
*/
/*
struct point_hash : std::unary_function<simple_point, std::size_t>
{
	std::size_t operator()(simple_point const& p) const
	{
		int xh = (int)floor((double)p.x / cell_size);
		int yh = (int)floor((double)p.y / cell_size);
		int zh = (int)floor((double)p.z / cell_size);

		std::size_t seed = 0;
		boost::hash_combine(seed, )
	}
};
*/

//typedef vector<double> vecd;
//typedef unsigned int data_t;
// <key,payload,hash fn, equality fn>
//typedef boost::unordered_multimap<simple_point, polygon_indices> polygon_hashmap;

class ClosestPointQuery
{
public:

	ClosestPointQuery(const Mesh& m, double init_cell_size, unsigned int init_max_buckets);
	simple_point operator()(const simple_point& queryPoint, float maxDist) const;

	static inline discrete_point cell(simple_point const& p)
	{
		return discrete_point(boost::math::iround(p.x / cell_size),
			boost::math::iround(p.y / cell_size),
			boost::math::iround(p.z / cell_size));
	}

	/*
	inline unsigned int hash_point(const simple_point& p) const
	{
		int xh = (int)floor((double)p.x / cell_size);
		int yh = (int)floor((double)p.y / cell_size);
		int zh = (int)floor((double)p.z / cell_size);
		return (xh * 73856093 ^ yh * 19349663 ^ zh * 83492791) % max_buckets;
	}
	*/
	struct point_equal_to : std::binary_function<simple_point, simple_point, bool>
	{
		bool operator()(simple_point const& p1, simple_point const& p2) const
		{
			//return  p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
			// Just return true as we don't want the underlying container to try and resolve hash conflicts we want points to map to the same buckets/cells
			return true;
		}
	};

	struct point_hash : std::unary_function<simple_point, std::size_t>
	{
		std::size_t operator()(simple_point const& p) const
		{
			std::size_t seed = 0;
			//int xh = boost::math::iround(p.x / cell_size);
			//int yh = boost::math::iround(p.y / cell_size);
			//int zh = boost::math::iround(p.z / cell_size);
			discrete_point cidx = cell(p);
			//std::cout << "xh " << xh << " yh " << yh << " zh " << zh << "\n";
			boost::hash_combine(seed, cidx.a * 73856093);
			boost::hash_combine(seed, cidx.b * 19349663);
			boost::hash_combine(seed, cidx.c * 83492791);
			//boost::hash_combine(seed, xh * 73856093);
			//boost::hash_combine(seed, yh * 19349663);
			//boost::hash_combine(seed, zh * 83492791);
			//std::cout << "key " << seed << "\n";
			return seed;
		}
	};

	// Bounding Box operations
	void calculate_bounding_box();
	int get_intersection(double dist1, double dist2, simple_point p1, simple_point p2, simple_point & Hit, std::string debug) const;
	int inside_box(simple_point Hit, simple_point b1, simple_point b2, const int Axis) const;
	int intersect_line_bb(simple_point b1, simple_point b2, simple_point l1, simple_point l2, simple_point & Hit) const;
	simple_point get_bb_intersection(const simple_point& p) const;

	simple_point bb_min;
	simple_point bb_max;
	simple_point bb_center;

	// Spatial Hashing Operations
	void populate_buckets();
	std::vector<simple_point>* get_points(const simple_point& p) const;

	static double cell_size;
	static unsigned int max_buckets;
	const Mesh *mesh;
	typedef boost::unordered_multimap<simple_point, discrete_point, point_hash, point_equal_to> polygon_hashmap;
	polygon_hashmap hashmap;
};
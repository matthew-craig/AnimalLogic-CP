#pragma once
#include "Mesh.h"

#include <boost/unordered_map.hpp>
#include <boost/math/special_functions/round.hpp>

class ClosestPointQuery
{
public:

	ClosestPointQuery(const Mesh& m, double init_cell_size, unsigned int init_max_buckets);

	static inline discrete_point cell(simple_point const& p)
	{
		return discrete_point(boost::math::iround(p.x / cell_size),
			boost::math::iround(p.y / cell_size),
			boost::math::iround(p.z / cell_size));
	}

	struct point_equal_to : std::binary_function<simple_point, simple_point, bool>
	{
		bool operator()(simple_point const& p1, simple_point const& p2) const
		{
			// Just return true as we don't want the underlying container to try and resolve hash conflicts we want points to map to the same buckets/cells
			return true;
		}
	};

	struct point_hash : std::unary_function<simple_point, std::size_t>
	{
		std::size_t operator()(simple_point const& p) const
		{
			std::size_t seed = 0;
			discrete_point cidx = cell(p);
			//std::cout << "xh " << xh << " yh " << yh << " zh " << zh << "\n";
			boost::hash_combine(seed, cidx.a * 73856093);
			boost::hash_combine(seed, cidx.b * 19349663);
			boost::hash_combine(seed, cidx.c * 83492791);
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
	simple_point operator()(const simple_point& queryPoint, const double maxDist = cell_size, int brute_force = 0) const;
	void populate_buckets();
	std::vector<simple_point>* get_points(const simple_point& p, const double max_dist = cell_size) const;
	std::vector<simple_point>* get_points_brute_force(const simple_point& p, const double max_dist = cell_size) const;
	double get_distance(const simple_point& p1, const simple_point& p2) const;

	static double cell_size;
	static unsigned int max_buckets;
	const Mesh *mesh;
	typedef boost::unordered_multimap<simple_point, discrete_point, point_hash, point_equal_to> polygon_hashmap;
	polygon_hashmap hashmap;
};
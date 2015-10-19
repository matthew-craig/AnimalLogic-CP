// ClosestPointOnMesh.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include <boost/timer/timer.hpp>

#include "Mesh.h"
#include "SpatialHash_ClosestPoint.h"

int main()
{
	const double cell_size = 1.0;
	const unsigned int max_buckets = 5000;
	const double max_dist = 1.0; // NB: max_dist searches an expanding grid so increases are O(n3) within the search space that will correspond to cell size

	const Mesh ss_mesh = Mesh("simplesphere.obj");

	if (ss_mesh.shapes.size() == 0)
		std::cout << "WARNING: No shapes loaded from mesh\n";

	ClosestPointQuery ss_pq = ClosestPointQuery(ss_mesh,cell_size,max_buckets);
	ss_pq.populate_buckets();
	ss_pq.calculate_bounding_box();

	// Test which point we hit the sphere from from directly above should be point 381 for simplesphere.obj
	simple_point query_test_p1 = simple_point(-0.988001, 10.0, -1.397472);
	simple_point closest_point = simple_point();

	{
		boost::timer::auto_cpu_timer t(std::cerr, 2);
		closest_point = ss_pq(query_test_p1, max_dist);
	}

	std::cout << "closest point to query point is x " << closest_point.x << " y " << closest_point.y << " z " << closest_point.z << "\n";

	// Test a known point in the mesh should definitely return results
	simple_point query_known_p1 = simple_point(-1.2539, 5.50048, -0.579121);
	closest_point = ss_pq(query_known_p1, max_dist);
	std::cout << "closest point to query point is x " << closest_point.x << " y " << closest_point.y << " z " << closest_point.z << "\n";

	simple_point query_p1 = simple_point(0.0, 10.0, 0.0);
	closest_point = ss_pq(query_p1, max_dist);
	std::cout << "closest point to query point is x " << closest_point.x << " y " << closest_point.y << " z " << closest_point.z << "\n";

	// setup for a larger mesh
	const Mesh ls_mesh = Mesh("largesphere.obj");

	if (ls_mesh.shapes.size() == 0)
		std::cout << "WARNING: No shapes loaded from mesh\n";

	ClosestPointQuery ls_pq = ClosestPointQuery(ls_mesh, cell_size, max_buckets);
	ls_pq.populate_buckets();
	ls_pq.calculate_bounding_box();

	query_test_p1 = simple_point(-0.988001, 10.0, -1.397472);

	{
		boost::timer::auto_cpu_timer t(std::cerr, 2);
		closest_point = ls_pq(query_test_p1, max_dist);
	}

	std::cout << "closest point to query point is x " << closest_point.x << " y " << closest_point.y << " z " << closest_point.z << "\n";

	{
		boost::timer::auto_cpu_timer t(std::cerr, 2);
		closest_point = ls_pq(query_test_p1, max_dist, 1);
	}

	std::cout << "closest point to query point is x " << closest_point.x << " y " << closest_point.y << " z " << closest_point.z << "\n";

	std::cin.get();
	return 0;
}


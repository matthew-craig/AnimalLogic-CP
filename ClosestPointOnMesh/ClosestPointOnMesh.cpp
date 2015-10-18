// ClosestPointOnMesh.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include "Mesh.h"
#include "SpatialHash_ClosestPoint.h"

int main()
{
	const Mesh mesh = Mesh("simplesphere.obj");
	const double cell_size = 1.0;
	const unsigned int max_buckets = 5000;
	const double max_dist = 1.0; // NB: max_dist searches an expanding grid so increases are O(n3) within the search space that will correspond to cell size

	ClosestPointQuery pq = ClosestPointQuery(mesh,cell_size,max_buckets);
	pq.populate_buckets();
	pq.calculate_bounding_box();

	simple_point query_test_p1 = simple_point(-0.988001, 10.0, -1.397472);
	simple_point closest_point = pq(query_test_p1, max_dist);
	std::cout << "closest point to query point is x " << closest_point.x << " y " << closest_point.y << " z " << closest_point.z << "\n";

	simple_point query_p1 = simple_point(0.0, 10.0, 0.0);
	//simple_point query_p1 = simple_point(-1.2539, 5.50048, -0.579121);
	closest_point = pq(query_p1, max_dist);
	std::cout << "closest point to query point is x " << closest_point.x << " y " << closest_point.y << " z " << closest_point.z << "\n";

	std::cin.get();
	return 0;
}


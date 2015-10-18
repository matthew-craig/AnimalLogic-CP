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
	ClosestPointQuery pq = ClosestPointQuery(mesh,cell_size,max_buckets);
	pq.populate_buckets();
	pq.calculate_bounding_box();
	simple_point query_p1 = simple_point(0.0, 10.0, 0.0);
	//simple_point query_p1 = simple_point(-1.2539, 5.50048, -0.579121);
	simple_point closest_point_p1 = pq(query_p1, 10.0);
	std::cin.get();
	return 0;
}


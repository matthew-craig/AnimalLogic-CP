#include "SpatialHash_ClosestPoint.h"

double ClosestPointQuery::cell_size = 0;
unsigned int ClosestPointQuery::max_buckets = 0;

ClosestPointQuery::ClosestPointQuery(const Mesh& m, double init_cell_size, unsigned int init_max_buckets)
{
	cell_size = init_cell_size;
	max_buckets = init_max_buckets;	
	bb_min.x = bb_min.y = bb_min.z = 0;
	bb_max.x = bb_max.y = bb_max.z = 0;
	bb_center.x = bb_center.y = bb_center.z = 0;
	mesh = &m;
}

void ClosestPointQuery::populate_buckets()
{
	hashmap.clear();
	unsigned long pts = 0;
	for (size_t v = 0; v < mesh->shapes[0].mesh.positions.size() / 3; v++)
	{
		simple_point p = simple_point(mesh->shapes[0].mesh.positions[3 * v + 0],
			mesh->shapes[0].mesh.positions[3 * v + 1],
			mesh->shapes[0].mesh.positions[3 * v + 2]);

		// Tbd store indice lookups
		/*
		polygon_indices p = polygon_indices(mesh->shapes[0].mesh.indices[3 * f + 0],
			mesh->shapes[0].mesh.positions[3 * v + 1],
			mesh->shapes[0].mesh.positions[3 * v + 2]);
		*/
		discrete_point idx = discrete_point();

		hashmap.emplace(p, idx);
		pts++;
	}
	std::cout << "Populated " << pts << " points to hashmap" << "\n";
}

void ClosestPointQuery::calculate_bounding_box()
{
	for (size_t v = 0; v < mesh->shapes[0].mesh.positions.size() / 3; v++)
	{
		simple_point p = simple_point(mesh->shapes[0].mesh.positions[3 * v + 0],
			mesh->shapes[0].mesh.positions[3 * v + 1],
			mesh->shapes[0].mesh.positions[3 * v + 2]);

		if (p.x < bb_min.x) bb_min.x = p.x;
		if (p.y < bb_min.y) bb_min.y = p.y;
		if (p.z < bb_min.z) bb_min.z = p.z;

		if (p.x > bb_max.x) bb_max.x = p.x;
		if (p.y > bb_max.y) bb_max.y = p.y;
		if (p.z > bb_max.z) bb_max.z = p.z;
	}

	bb_center.x = bb_min.x + (bb_max.x - bb_min.x) / 2.0;
	bb_center.y = bb_min.y + (bb_max.y - bb_min.y) / 2.0;
	bb_center.z = bb_min.z + (bb_max.z - bb_min.z) / 2.0;

	std::cout << "Obj bb min x " << bb_min.x << " y " << bb_min.y << " z " << bb_min.z << "\n";
	std::cout << "Obj bb max x " << bb_max.x << " y " << bb_max.y << " z " << bb_max.z << "\n";
	std::cout << "Obj bb center x " << bb_center.x << " y " << bb_center.y << " z " << bb_center.z << "\n";
}

int inline ClosestPointQuery::get_intersection(double dist1, double dist2, simple_point p1, simple_point p2, simple_point &Hit, std::string debug) const
{
	if ((dist1 * dist2) >= 0.0f) return 0;
	if (dist1 == dist2) return 0;
	double dist_factor = (-dist1 / (dist2 - dist1));
	simple_point dist_vec = simple_point(dist_factor, dist_factor, dist_factor);
	Hit = p1 + (p2 - p1) * dist_vec;
	/*
	std::cout << "hit " << debug << "\n";
	std::cout << "p1 x " << p1.x << " y " << p1.y << " z " << p1.z << "\n";
	std::cout << "p2 x " << p2.x << " y " << p2.y << " z " << p2.z << "\n";
	std::cout << "dist1 " << dist1 << " dist2 " << dist2 << "\n";
	std::cout << "dist factor " << (-dist1 / (dist2 - dist1)) << "\n";
	simple_point diff = (p2 - p1);
	std::cout << "diff x " << diff.x << " y " << diff.y << " z " << diff.z << "\n";
	simple_point diff_mult = (p2 - p1) * (-dist1 / (dist2 - dist1));
	std::cout << "diff_mult x " << diff_mult.x << " y " << diff_mult.y << " z " << diff_mult.z << "\n";
	*/
	return 1;
}

int inline ClosestPointQuery::inside_box(simple_point Hit, simple_point b1, simple_point b2, const int Axis) const
{
	if (Axis == 1 && Hit.z > b1.z && Hit.z < b2.z && Hit.y > b1.y && Hit.y < b2.y) return 1;
	if (Axis == 2 && Hit.z > b1.z && Hit.z < b2.z && Hit.x > b1.x && Hit.x < b2.x) return 1;
	if (Axis == 3 && Hit.x > b1.x && Hit.x < b2.x && Hit.y > b1.y && Hit.y < b2.y) return 1;
	return 0;
}

int ClosestPointQuery::intersect_line_bb(simple_point b1, simple_point b2, simple_point l1, simple_point l2, simple_point &Hit) const
{
	if (l1.x > b1.x && l1.x < b2.x &&
		l1.y > b1.y && l1.y < b2.y &&
		l1.z > b1.z && l1.z < b2.z)
	{
		Hit = l1;
		return true;
	}
	if ((get_intersection(l1.x - b1.x, l2.x - b1.x, l1, l2, Hit, "xmin") && inside_box(Hit, b1, b2, 1))
		|| (get_intersection(l1.y - b1.y, l2.y - b1.y, l1, l2, Hit, "ymin") && inside_box(Hit, b1, b2, 2))
		|| (get_intersection(l1.z - b1.z, l2.z - b1.z, l1, l2, Hit, "zmin") && inside_box(Hit, b1, b2, 3))
		|| (get_intersection(l1.x - b2.x, l2.x - b2.x, l1, l2, Hit, "xmax") && inside_box(Hit, b1, b2, 1))
		|| (get_intersection(l1.y - b2.y, l2.y - b2.y, l1, l2, Hit, "ymax") && inside_box(Hit, b1, b2, 2))
		|| (get_intersection(l1.z - b2.z, l2.z - b2.z, l1, l2, Hit, "zmax") && inside_box(Hit, b1, b2, 3)))
		return true;

	return false;
}

simple_point ClosestPointQuery::get_bb_intersection(const simple_point& p) const
{
	simple_point hit_point = simple_point();
	int hit = intersect_line_bb(bb_min, bb_max, p, bb_center, hit_point);
	return hit_point;
}

std::vector<simple_point>* ClosestPointQuery::get_points(const simple_point& p) const
{
	std::vector<simple_point>* points = new std::vector<simple_point>;
	/*
	for (auto it = hashmap.find(p); it != hashmap.end(); it++)
	{
		points->push_back(it->first);
	}
	*/
	discrete_point cidx = cell(p);
	std::cout << "query cell x " << cidx.a << " y " << cidx.b << " z " << cidx.c << "\n";

	std::cout << "hashmap size " << hashmap.size() << "\n";
	std::cout << "hashmap buckets " << hashmap.bucket_count() << "\n";
	std::cout << "hashmap bucket " << hashmap.bucket(p) << "\n";
	std::cout << "hashmap count " << hashmap.count(p) << "\n\n";
	auto range = hashmap.equal_range(p);
	for (auto it = range.first; it != range.second; ++it)
	{
		points->push_back(it->first);
	}
	
	// Show hash distributions useful for debugging cell size changes
	for (size_t v = 0; v < mesh->shapes[0].mesh.positions.size() / 3; v++)
	{
		simple_point p = simple_point(mesh->shapes[0].mesh.positions[3 * v + 0],
			mesh->shapes[0].mesh.positions[3 * v + 1],
			mesh->shapes[0].mesh.positions[3 * v + 2]);

		int count = hashmap.count(p);
		std::cout << "hashmap count " << hashmap.count(p) << "\n";
		//std::cout << "point x " << p.x << " y " << p.y << " z " << p.z << "\n";
	}
	
	return points;
}

simple_point ClosestPointQuery::operator()(const simple_point& queryPoint, float maxDist) const
{
	// find the intersection point of a line from the query point to the meshes bounding box
	simple_point hit_point = get_bb_intersection(queryPoint);

	std::cout << "query point x " << queryPoint.x << " y " << queryPoint.y << " z " << queryPoint.z << " intersects mesh at hit point x " << hit_point.x << " y " << hit_point.y << " z " << hit_point.z << "\n";

	// search outwards from the intersection point up to maxDist for 
	std::vector<simple_point>* pts = get_points(hit_point);

	std::cout << "\nfound " << pts->size() << " points\n";

	// check resulting points to confirm which one is actually closest
	if (pts->size() > 0)
	{

	}

	return simple_point(1.0f, 1.0f, 1.0f);
}

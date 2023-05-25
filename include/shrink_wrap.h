#pragma once

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/alpha_wrap_3.h>
#include <CGAL/Polygon_mesh_processing/bbox.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Real_timer.h>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Surface_mesh<Point> Surface_mesh;


namespace AW3 = CGAL::Alpha_wraps_3;
namespace PMP = CGAL::Polygon_mesh_processing;
namespace fs = boost::filesystem;

Surface_mesh generate_wrap_for_organ(fs::directory_entry &organ_path, const double relative_alpha, const double relative_offset);

Surface_mesh generate_wrap_for_mesh(std::string mesh_path, const double relative_alpha, const double relative_offset);



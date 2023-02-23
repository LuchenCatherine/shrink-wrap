#include "shrink_wrap.h"

Surface_mesh generate_wrap_for_organ(fs::directory_entry &organ_path, const double relative_alpha, const double relative_offset)
{

  auto organ_name = organ_path.path().stem().string();
  
  // step 1: merge all the meshes of anatomical structures
  Surface_mesh combined_mesh;

  for (fs::directory_entry& AS : fs::directory_iterator(organ_path)) 
  {
    std::string file_path = AS.path().string();
    
    Surface_mesh cur_as_mesh;
    std::vector<Point> points;
    std::vector<std::vector<std::size_t> > polygons;
    if(!CGAL::IO::read_polygon_soup(file_path, points, polygons) || points.empty())
    {
      std::cerr << "Cannot open file " << file_path << std::endl;
      continue;
    }
    PMP::orient_polygon_soup(points, polygons);
    PMP::polygon_soup_to_polygon_mesh(points, polygons, cur_as_mesh);
    CGAL::copy_face_graph(cur_as_mesh, combined_mesh);
  }

  // step 2: create the alpha wrap for the combined_mesh
  CGAL::Bbox_3 bbox = PMP::bbox(combined_mesh);
  
  const double diag_length = std::sqrt(CGAL::square(bbox.xmax() - bbox.xmin()) +
                                       CGAL::square(bbox.ymax() - bbox.ymin()) +
                                       CGAL::square(bbox.zmax() - bbox.zmin()));
  const double alpha = diag_length / relative_alpha;
  const double offset = diag_length / relative_offset;

  CGAL::Real_timer t;
  t.start();
  Surface_mesh wrap;
  CGAL::alpha_wrap_3(combined_mesh, alpha, offset, wrap);
  t.stop();

  std::cout << organ_name << ":\nResult: " << num_vertices(wrap) << " vertices, " << num_faces(wrap) << " faces, ";
  std::cout << "Took " << t.time() << " s." << std::endl;
  
  // step 3: compute the volume of the wrap
  double volume = PMP::volume(wrap) * 1e9;
  std::cout << "Volume is " << volume << std::endl;
  // output_file << organ_name << "," << AS.path().stem().string() << "," << volume << '\n';

  // step 4: save the result of volume to csv
  // output_volume << organ_name << "," << volume << "\n";

  return wrap;

}
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


Surface_mesh generate_wrap_for_mesh(std::string mesh_path, const double relative_alpha, const double relative_offset)
{

  // step 1: load (non-)manifold mesh 
  Surface_mesh mesh;
  std::vector<Point> points;
  std::vector<std::vector<std::size_t> > polygons;
  
  if(!CGAL::IO::read_polygon_soup(mesh_path, points, polygons) || points.empty())
  {
    std::cerr << "Cannot open file " << mesh_path << std::endl;
    return mesh;
  }
  PMP::orient_polygon_soup(points, polygons);
  PMP::polygon_soup_to_polygon_mesh(points, polygons, mesh);

  // step 2: create the alpha wrap
  CGAL::Bbox_3 bbox = PMP::bbox(mesh);
  
  const double diag_length = std::sqrt(CGAL::square(bbox.xmax() - bbox.xmin()) +
                                       CGAL::square(bbox.ymax() - bbox.ymin()) +
                                       CGAL::square(bbox.zmax() - bbox.zmin()));
  const double alpha = diag_length / relative_alpha;
  const double offset = diag_length / relative_offset;

  CGAL::Real_timer t;
  t.start();
  Surface_mesh wrap;
  CGAL::alpha_wrap_3(mesh, alpha, offset, wrap);
  t.stop();

  std::cout << mesh_path << ":\nResult: " << num_vertices(wrap) << " vertices, " << num_faces(wrap) << " faces, ";
  std::cout << "Took " << t.time() << " s." << std::endl;
  
  // step 3: compute the volume of the wrap for each AS
  double volume = PMP::volume(wrap) * 1e9;
  std::cout << "Volume is " << volume << std::endl;

  return wrap;

}

Surface_mesh generate_wrap_for_mesh_if_not_watertight(std::string mesh_path, const double relative_alpha, const double relative_offset, std::ofstream &output)
{

  // return result
  Surface_mesh wrap;

  // step 1: load (non-)manifold mesh 
  Surface_mesh mesh;
  std::vector<Point> points;
  std::vector<std::vector<std::size_t> > polygons;

  bool is_fully_watertight_before_shrink_wrap = true;
  if(CGAL::IO::read_polygon_soup(mesh_path, points, polygons) && !points.empty())
  {

    PMP::orient_polygon_soup(points, polygons);
    PMP::polygon_soup_to_polygon_mesh(points, polygons, mesh);

    // step 2: check watertight or not
    bool is_self_intersected = PMP::does_self_intersect(mesh);
    bool is_closed = CGAL::is_closed(mesh);

    if (!is_self_intersected && is_closed)
    {
      std::cout << mesh_path << " is watertight." << std::endl;
      wrap = mesh;
    }
    else
    {
      is_fully_watertight_before_shrink_wrap = false;
      // step 3: create the alpha wrap if not watertight
      fs::path mesh_p = fs::path(mesh_path);
      fs::path plain_folder("/home/luchen/data/model/plain_v1.3/");
      fs::path manifold_folder("/home/luchen/data/model/plain_manifold_filling_hole_v1.3");
      fs::path relative_path = fs::relative(mesh_p, manifold_folder);
      fs::path plain_mesh_path = plain_folder / relative_path;
      std::cout << "plain_mesh_path: " << plain_mesh_path <<std::endl;

      wrap = generate_wrap_for_mesh(plain_mesh_path.string(), relative_alpha, relative_offset);
    }
  }
  else 
  {
    is_fully_watertight_before_shrink_wrap = false;
    // step 3: create the alpha wrap if not watertight
    fs::path mesh_p = fs::path(mesh_path);
    fs::path plain_folder("/home/luchen/data/model/plain_v1.3/");
    fs::path manifold_folder("/home/luchen/data/model/plain_manifold_filling_hole_v1.3");
    fs::path relative_path = fs::relative(mesh_p, manifold_folder);
    fs::path plain_mesh_path = plain_folder / relative_path;
    std::cout << "plain_mesh_path: " << plain_mesh_path <<std::endl;

    wrap = generate_wrap_for_mesh(plain_mesh_path.string(), relative_alpha, relative_offset);

    // CGAL::Bbox_3 bbox = PMP::bbox(mesh);
    
    // const double diag_length = std::sqrt(CGAL::square(bbox.xmax() - bbox.xmin()) +
    //                                     CGAL::square(bbox.ymax() - bbox.ymin()) +
    //                                     CGAL::square(bbox.zmax() - bbox.zmin()));
    // const double alpha = diag_length / relative_alpha;
    // const double offset = diag_length / relative_offset;

    // CGAL::Real_timer t;
    // t.start();
    // // Surface_mesh wrap;
    // CGAL::alpha_wrap_3(mesh, alpha, offset, wrap);
    // t.stop();

    // std::cout << mesh_path << ":\nResult: " << num_vertices(wrap) << " vertices, " << num_faces(wrap) << " faces, ";
    // std::cout << "Took " << t.time() << " s." << std::endl;
  }

  // step 4: compute the volume of the wrap for each AS
  double volume = PMP::volume(wrap) * 1e9;
  std::cout << "Volume is " << volume << std::endl;

  output << volume << ",";

  // always watertight after shrink wrap
  if (is_fully_watertight_before_shrink_wrap)
    output << "Yes" << "," << "Yes\n";
  else
    output << "No" << "," << "Yes\n";

  return wrap;

}
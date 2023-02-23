#include "shrink_wrap.h"

int main(int argc, char** argv)
{
  
    std::cout.precision(17);
    
    // Read the input args
    auto body_path = std::string(argv[1]);
    const double relative_alpha = (argc > 2) ? std::stod(argv[2]) : 50.;
    const double relative_offset = (argc > 3) ? std::stod(argv[3]) : 600.;
    
    const fs::path output_organ_dir = "./alph_wrap/";
    
    if (!fs::exists(output_organ_dir)) fs::create_directory(output_organ_dir);
    
    
    // create csv
    std::ofstream output_volume;
    output_volume.open("volume_of_organ.csv");

    // csv header
    output_volume << "organ, volume\n";
    
    // traverse all the organs
    for (fs::directory_entry& organ_path : fs::directory_iterator(body_path)) 
    {

        auto organ_name = organ_path.path().stem().string();
        Surface_mesh wrap = generate_wrap_for_organ(organ_path, relative_alpha, relative_offset);

        // save the volume for each organ
        auto volume = PMP::volume(wrap) * 1e9;
        output_volume << organ_name << "," << volume << "\n";
        
        // Save the result
        std::string input_name = "alpha_wrap_" + organ_name;
        std::string output_name = input_name + "_" + std::to_string(static_cast<int>(relative_alpha)) + "_" + std::to_string(static_cast<int>(relative_offset)) + ".off";
        std::cout << "Writing to " << output_name << std::endl;
        
        CGAL::IO::write_polygon_mesh(output_organ_dir.string() + output_name, wrap, CGAL::parameters::stream_precision(17));

    }

    return EXIT_SUCCESS;

}
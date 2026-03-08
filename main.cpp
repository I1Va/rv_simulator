

// #include <iostream>
// #include <fstream>
// #include <filesystem>




// bool file_exists(const char* path) {
//     std::ifstream f(path);
//     return f.good();
// }

// int main(int argc, char* argv[]) { // simulator driver 
//     // check args and launch simulator 

//     if (argc != 1) {
//         std::cerr << "Error: missing required argument (rv32i asm file path)\n";
//         return 1;
//     }
    
//     char *rv_file_path = argv[1];
//     if (!fs::exists(rv_file_path)) {
//         std::cerr << "File not found: " << rv_file_path << "\n";
//         return 1;
//     }



//     std::cout << "Program name: " << argv[0] << "\n";
//     std::cout << "Number of arguments: " << argc - 1 << "\n";

//     for (int i = 1; i < argc; ++i) {
//         std::cout << "Arg " << i << ": " << argv[i] << "\n";
//     }
// }
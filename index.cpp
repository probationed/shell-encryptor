#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stdexcept>

std::string to_hex(const std::string &input) { // convert raw
    std::ostringstream oss;
    for (char ch : input) {
        unsigned int byte = static_cast<unsigned char>(ch);
        oss << std::setw(2) << std::setfill('0') << std::hex << byte;
    }
    return oss.str();
}

std::string read_file(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Cannot open: " + filename);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

void createobf(const std::string &hex, const std::string &out) {
    std::ofstream f(out);
    if (!f.is_open()) throw std::runtime_error("Cannot write: " + out);

    f << "#!/bin/bash\n";
    f << "\n";
    f << "x9=$(echo \"787864\")\n";
    f << "x9=$(echo $x9 | xxd -r -p)\n";
    f << "x8=\"";
    
    for (size_t i = 0; i < hex.length(); i += 32) {
        f << hex.substr(i, 32) << "\"\n";
        if (i + 32 < hex.length()) f << "x8+=\"";
    }

    f << "l1=$(mktemp)\n";
    f << "echo \"$x8\" | $x9 -r -p > \"$l1\"\n";
    f << "c0=$(echo '63686d6f64')\n";
    f << "c0=$(echo $c0 | $x9 -r -p)\n";
    f << "$c0 +x \"$l1\"\n";
    f << "eval \"$l1\"\n";
    f << "rm \"$l1\"\n";

    f.close();
}

int main() {
    std::string file, out;
    std::cout << "Input   $ ";
    std::getline(std::cin, file);
    std::cout << "Output  $ ";
    std::getline(std::cin, out);

    try {
        std::string content = read_file(file);
        std::string hex = to_hex(content);
        createobf(hex, out);
        std::cout << "" << out << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "" << e.what() << std::endl;
    }

    return 0;
}

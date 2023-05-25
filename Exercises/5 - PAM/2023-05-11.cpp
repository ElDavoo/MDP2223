#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <tuple>
#include <string>


enum tupltype_t { GRAYSCALE, RGB };

char* to_c_string(std::string s) {
    const size_t length = s.length();
    char* char_array = new char[length + 1];
    strncpy_s(char_array, length + 1, s.c_str(), length + 1);
    return char_array;
}

std::tuple<std::string, unsigned int> get_token(std::istream& is) {
    std::string entire_token;
    is >> entire_token >> value;
    return std::tuple<std::string, unsigned int>(command, value);

}

class Pixel {
    enum tupltype_t { GRAYSCALE, RGB };

};

class PAM {
    unsigned int width_;
    unsigned int height_;
    uint8_t depth_;
    unsigned int maxval_;
    tupltype_t tupltype_;
public:
    PAM(std::istream& is) {
        auto tkn = get_token(is);
        while (std::get<std::string>(tkn) != "ENDHDR") {
            tkn = get_token(is);
            std::cout << std::get<std::string>(tkn) << " " << std::get<unsigned int>(tkn);
        }
    }
};


int main(int argc, char* argv[])
{
    if (argc != 2) return 1;
    std::ifstream is(argv[1], std::ios::binary);
    if (!is) {
        std::cout << "Apertura file fallita" << std::endl;
        return 1;
    }
    PAM pam(is);


}
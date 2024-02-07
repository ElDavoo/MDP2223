//#pragma warning(disable: 4720)

#include <fstream>
#include <iostream>
#include <vector>
#include <bitset>
#include <cmath>
#include <functional>
#include <cstring>

/*template <typename T> void raw_read() {

}*/

struct bitreader : std::ifstream {
    char buffer_ = 0;
    uint8_t counter_ = 0;

    std::istream& read(char* s, std::streamsize count) {
        if (counter_ == 8 || counter_ == 0) return std::ifstream::read(s, count); 
        throw std::runtime_error("to be done");
        
    }
    std::vector<bool> bit_read(std::streamsize count){
        std::vector<bool> v;
        if (count < 1) throw std::invalid_argument("hey, più di 1 bit");
        while (count-- > 0) {
            if (!counter_) {
                counter_ = 8;
                read(&buffer_, 1);
            }
            v.push_back(buffer_ >> counter_ & 1);
            --counter_;
        }
        return v;
    }
    bitreader(char* c , const std::ios_base::openmode o) : std::ifstream(c,o) {}
};

typedef enum {
    BI_RGB, BI_RLE8, BI_RLE4
} COMP_METHOD;

class BFH {
    uint32_t size_ = 0;
    uint32_t offset_ = 0;
    int16_t rev1_ = 0;
    int16_t rev2_ = 0;

public:

    BFH(std::istream& is) {
        char header[2];
        is.read(header, 2);
        if (is.fail()) throw std::runtime_error("Error reading file");
        if (header[0] != 0x42 || header[1] != 0x4D) throw std::invalid_argument("Invalid magic");
        is.read((char*) &size_, 4);
        if (is.fail()) throw std::runtime_error("Error reading file");
        is.read((char *) &rev1_, 2);
        if (is.fail()) throw std::runtime_error("Error reading file");
        is.read((char *) &rev2_, 2);
        if (is.fail()) throw std::runtime_error("Error reading file");
        is.read((char*) &offset_, 4);
        if (is.fail()) throw std::runtime_error("Error reading file");
    }

    BFH(void) {
        //bro

    }
};

class color {
    std::vector<bool> r_;
    std::vector<bool> g_;
    std::vector<bool> b_;
    uint32_t i_;

public:
    color(bitreader& is, std::streamsize bpp) {
        r_ = is.bit_read(bpp);
    }
};

class BIH {
    int32_t width_ = 0;
    int32_t height_ = 0;
    uint16_t color_planes_ = 0;
    // bit per PIXEL, NON per canale colore !!! =) =) 
    uint16_t bpp_ = 0;
    COMP_METHOD comp_method_ = BI_RGB;
    uint32_t raw_size_ = 0;
    int32_t hores_ = 0;
    int32_t veres_ = 0;
    uint32_t num_colors_ = 0;
    uint32_t important_colors_ = 0;

public:

    BIH(std::istream& is) {
        uint32_t header;
        uint32_t comp_method;
        is.read((char *)&header, 4);
        if (is.fail()) throw std::runtime_error("Error reading file");
        if (header != 40) throw std::invalid_argument("Invalid BIH size");

        is.read((char*)&width_, 4);
        if (is.fail()) throw std::runtime_error("Error reading file");
        if (width_ < 1) throw std::invalid_argument("Negative width");

        is.read((char*)&height_, 4);
        if (is.fail()) throw std::runtime_error("Error reading file");
        if (height_ < 1) throw std::invalid_argument("Negative width");

        is.read((char*)&color_planes_, 2);
        if (is.fail()) throw std::runtime_error("Error reading file");
        if (color_planes_ != 1) throw std::invalid_argument("Color planes not 1");

        is.read((char*)&bpp_, 2);
        if (is.fail()) throw std::runtime_error("Error reading file");

        is.read((char*)&comp_method, 4);
        if (is.fail()) throw std::runtime_error("Error reading file");
        if (comp_method == 0) {
            comp_method_ = BI_RGB;
        }
        else throw std::invalid_argument("Unsupported comp method");

        is.read((char*)&raw_size_, 4);
        if (is.fail()) throw std::runtime_error("Error reading file");
        //if (raw_size_) throw std::invalid_argument("Invalid raw size, should be 0");

        is.read((char*)&hores_, 4);
        if (is.fail()) throw std::runtime_error("Error reading file");
        if (hores_ < 1) throw std::invalid_argument("Negative horizontal resolution");

        is.read((char*)&veres_, 4);
        if (is.fail()) throw std::runtime_error("Error reading file");
        if (veres_ < 1) throw std::invalid_argument("Negative vertical resolution");

        is.read((char*)&num_colors_, 4);
        if (is.fail()) throw std::runtime_error("Error reading file");

        is.read((char*)&important_colors_, 4);
        if (is.fail()) throw std::runtime_error("Error reading file");

    }

    BIH(void){}

    auto get_num_colors() { return num_colors_; }
    auto bpp() { return bpp_; }
    auto get_height() { return height_; }
    auto get_width() { return width_; }
};

class BitMap {

    BFH bfh_;
    BIH bih_;
    std::vector<color> color_table;

public:

    BitMap(bitreader& is) {
        bfh_ = BFH(is);
        bih_ = BIH(is);
        if (bih_.get_num_colors()) {
            uint32_t num = bih_.get_num_colors();
            while (num-- > 0) {
                color_table.push_back(color(is, (bih_.bpp()/3)));
            }
        }
        uint32_t size = std::ceil(bih_.get_height() * bih_.get_width() * bih_.bpp()/8);
        data = (char *) malloc(size);
        if (data == nullptr) throw std::runtime_error("malloc failed");
        uint32_t count = 0;
        for (int i = bih_.get_height(); i > 0; --i) {
            size = std::floor(bih_.get_width() * bih_.bpp() / 8);
            is.read(&data[i*size], size);
            count += size;
            int padding = (bih_.get_width() * bih_.bpp() * 3) % 32;
            auto pad = is.bit_read(padding);
            for (const bool p : pad) {
                if (p) throw std::runtime_error("padding not 0");
            }

        }
    }
    ~BitMap() {
        free(data);
    }

    auto h() { return bih_.get_height(); }
    auto w() { return bih_.get_width(); }
    auto bpp() { return bih_.bpp(); }
    char* data;
    auto datasize() { return std::ceil(bih_.get_height() * bih_.get_width() * bih_.bpp() / 8); }

};

typedef enum { BLACKANDWHITE, GRAYSCALE, RGB } TUPLTYPE;

class PAM {
    uint32_t width_;
    uint32_t height_;
    uint8_t depth_ = 3;
    TUPLTYPE tupltype_ = RGB;
    char* data_;
    size_t size_;
public:
    PAM(BitMap& bmp) {
        width_ = bmp.w();
        height_ = bmp.h();
        data_ = (char*)malloc(static_cast<size_t>(bmp.h()) * bmp.w() * 3);
        if (data_ == nullptr) throw std::runtime_error("malloc failed");
        size_t j = 0;
        //brutto
        for (int i = 0; i < bmp.h() * bmp.w() * 3; i+=(bmp.bpp()/8)) {

            uint32_t px;
            memcpy(&px, &(bmp.data[i]), 3);
            //px = (uint32_t) bmp.data[i];
            px = px << 8;
            //px = px >> 8;
            uint8_t pxn = px >> 24;
            data_[j] = static_cast<char>(pxn);
            j++;

        }
        size_ = j;
    }
    void write(std::ofstream& os) {
        os << "P7" << std::endl;
        os << "WIDTH " << width_ << std::endl;
        os << "HEIGHT " << height_ << std::endl;
        os << "DEPTH 3" << std::endl;
        os << "MAXVAL 255" << std::endl;
        os << "TUPLTYPE ";
        if (tupltype_ == BLACKANDWHITE) os << "BLACKANDWHITE" << std::endl;
        if (tupltype_ == GRAYSCALE) os << "GRAYSCALE" << std::endl;
        if (tupltype_ == RGB) os << "RGB" << std::endl;
        os << "ENDHDR" << std::endl;
        os.write(data_, size_);

    }
    

};

int main(int argc, char* argv[])
{

    if (argc != 3) {
        std::cout << "Usage: " << argv << "<input file .BMP> <output file .PAM>" << std::endl;
        return EXIT_FAILURE;
    }

    bitreader ifs(argv[1], std::ios::binary);

    if (ifs.fail()) {
        std::cout << "Couldn't open input file" << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream ofs(argv[2], std::ios::binary);

    if (ofs.fail()) {
        std::cout << "Couldn't open output file" << std::endl;
        return EXIT_FAILURE;
    }

    // e mo?
    BitMap bmp = BitMap(ifs);
    PAM pam = PAM(bmp);
    pam.write(ofs);



    return EXIT_SUCCESS;
}
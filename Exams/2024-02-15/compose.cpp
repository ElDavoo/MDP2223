#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cassert>
typedef enum { BLACKANDWHITE, GRAYSCALE, RGB, RGB_ALPHA } tupltype;

template <typename T>
class mat {
    unsigned int rows_;
    unsigned int cols_;
    unsigned int depth_;
public:
    mat() {
        rows_ = 0;
        cols_ = 0;
        depth_ = 0;
    }
    std::vector<T> mat_;

    mat(unsigned int rows, unsigned int cols, unsigned int depth) : rows_(rows), cols_(cols), depth_(depth) {
        mat_.resize(rows * cols * depth);
    }
    auto resize(unsigned int rows, unsigned int cols) {
        mat_.resize(rows * cols * depth_);
        rows_ = rows;
        cols_ = cols;
    }
    T* rawdata() { return &mat_[0]; }
    //auto push_back(const T& elem) { return mat_.push_back(elem); }

};

struct coord {
    unsigned int x;
    unsigned int y;
    coord() { x = 0; y = 0; }
};

std::vector<uint8_t> blend_pixel(uint8_t *p1, uint8_t *p2) {
    std::vector<uint8_t> p;
    p.resize(4);

    float a1 = p1[3] / static_cast < float>(255);
    float a2 = p2[3] / static_cast<float>(255);
    float a0 = a1 + a2 * (1 - a1);

    // l'arrotondamento come viene fatto? undefined behaviour
    p[3] = static_cast<uint8_t>(a0 * 255);

    float r0 = (p1[0] * a1 + p2[0] * a2 * (1 - a1)) / a0;
    float g0 = (p1[1] * a1 + p2[1] * a2 * (1 - a1)) / a0;
    float b0 = (p1[2] * a1 + p2[2] * a2 * (1 - a1)) / a0;

    p[0] = static_cast<uint8_t>(r0);
    p[1] = static_cast<uint8_t>(g0);
    p[2] = static_cast<uint8_t>(b0);

    return p;

}

class pam {
    unsigned int w_;
    unsigned int h_;
    unsigned int depth_;
    uint16_t maxval_;
    tupltype tupl_;
    coord offset_;
public:
    [[nodiscard]] auto getW() const { return w_; }
    [[nodiscard]] auto getH() const { return h_; }
    [[nodiscard]] auto getD() const { return depth_; }
    [[nodiscard]] auto getOffset() const { return offset_; }
    mat<uint8_t> data_;




    pam(std::ifstream& is, coord offset) : offset_(offset) {
        maxval_ = 255;
        tupl_ = RGB;
        std::string token;
        is >> token;
        assert(token == "P7");
        while (token != "ENDHDR") {
            is >> token;
            if (token[0] == '#') continue;
            if (token == "WIDTH"){
                //is >> token;
                //w_ = std::stoi(token);
                is >> w_;
            }
            if (token == "HEIGHT") {
                is >> h_;
            }
            if (token == "DEPTH") {
                is >> depth_;
            }
            if (token == "MAXVAL") {
                is >> maxval_;
            }
            if (token == "TUPLTYPE") {
                is >> token;
                if (token == "RGB_ALPHA") {
                    tupl_ = RGB_ALPHA;
                }
                if (token == "RGB") {
                    tupl_ = RGB;
                }
                // TODO etc etc
            }

        }

        // 0x0a finale
        is.ignore(1);

        data_ = mat<uint8_t>(w_, h_, depth_);
        is.read(reinterpret_cast<char *>(data_.rawdata()), w_ * h_ * depth_);

        /*
        while (!is.fail()) {
            data_.push_back(;
            if (is.peek() == EOF) break;
        }*/


    }
    pam(unsigned int w, unsigned int h, unsigned int depth, uint16_t maxval, tupltype tupl) : w_(w), h_(h), depth_(depth), maxval_(maxval), tupl_(tupl) {
        data_ = mat<uint8_t>(w_, h_, depth_);
    }
    void save(std::ofstream& os) {
        os << "P7\n";
        os << "WIDTH " << w_ << '\n';
        os << "HEIGHT " << h_ << '\n';
        os << "DEPTH " << depth_ << '\n';
        os << "MAXVAL " << maxval_ << '\n';
        os << "TUPLTYPE ";
        switch (tupl_) {
        case(RGB_ALPHA):
                os << "RGB_ALPHA";
                break;
        default:
            break;
        }
        os << '\n';
        os << "ENDHDR\n";
        os.write(reinterpret_cast<const char *>(data_.rawdata()), data_.mat_.size());

    }


    void combine(const pam& other) {
        assert(tupl_ == RGB_ALPHA);

        for (unsigned int j = 0; j < other.getH(); j++) {

            //std::vector<uint8_t> row_other(&other.data_.mat_[a], &other.data_.mat_[b]);
            unsigned int i = 0;
            
            while (i < other.getW()) {
                int other_index = (other.depth_ * j * other.getW()) + (other.depth_ * i);
                // depth si può mettere in evidenza
                int our_index = (depth_ * j * w_) + (depth_ * i) + ( depth_ * other.getOffset().x  + depth_ * other.getOffset().y * w_);
                
                // bruttissimo (?) ma forse salva l'esame
                uint8_t pixel1[4];
                pixel1[0] = data_.mat_[our_index];
                pixel1[1] = data_.mat_[our_index + 1];
                pixel1[2] = data_.mat_[our_index + 2];
                pixel1[3] = data_.mat_[our_index + 3];

                /*std::vector<uint8_t> pixel1;
                pixel1.push_back(data_.mat_[our_index]);
                pixel1.push_back(data_.mat_[our_index + 1]);
                pixel1.push_back(data_.mat_[our_index + 2]);
                pixel1.push_back(data_.mat_[our_index + 3]);*/

                uint8_t pixel2[4];
                pixel2[0] = other.data_.mat_[other_index];
                pixel2[1] = other.data_.mat_[other_index + 1];
                pixel2[2] = other.data_.mat_[other_index + 2];


                //std::vector<uint8_t> pixel2(&other.data_.mat_[other_index], &other.data_.mat_[other_index + 3]);
                
                if (other.tupl_ == RGB) {
                    // set opacity to 1
                    pixel2[3] = 255;
                }
                else if (other.tupl_ == RGB_ALPHA) {
                    pixel2[3] = (other.data_.mat_[other_index + 3]);
                    //pixel2[3] = 255;
                }
                
                std::vector<uint8_t> blended = blend_pixel(pixel2, pixel1);
                // replace the calculated pixel onto the image
                std::copy(blended.begin(), blended.end(), data_.mat_.begin() + our_index);
                
                i++;
            }
            //std::vector<int>   sub(&data[100000], &data[101000]);
        }
    }
};


pam compute_base_layer(const std::vector<pam> &imgs) {
    unsigned int max_w = 0, max_h = 0;
    for (auto img : imgs) {
        unsigned int img_w = img.getW() + img.getOffset().x;
        unsigned int img_h = img.getH() + img.getOffset().y;

        max_w = std::max(img_w, max_w);
        max_h = std::max(img_h, max_h);
    }
    pam p(max_w, max_h, 4, 255, RGB_ALPHA);
    std::fill(p.data_.mat_.begin(), p.data_.mat_.end(), 0);
    return p;
}

int main(int argc, char ** argv)
{
    const std::string mp("-p");
    if (argc < 3) return EXIT_FAILURE;
    std::string output_filename(argv[1]);
    output_filename.append(".pam");
    std::ofstream os(output_filename, std::ofstream::binary);
    if (!os) return EXIT_FAILURE;

    std::vector<pam> images;
    for (int i = 2; i < argc; i++) {
        coord c;
        if (argv[i] == mp) {
            c.x = std::stoi(argv[++i]);
            c.y = std::stoi(argv[++i]);
            ++i;
        }
        std::string input_filename = argv[i];
        input_filename.append(".pam");
        std::ifstream is(input_filename, std::ios::binary);
        if(!is) return EXIT_FAILURE;
        images.push_back(pam(is, c));
        is.close();

    }

    pam base_layer = compute_base_layer(images);

    for (auto img : images) {
        base_layer.combine(img);
    }

    base_layer.save(os);
    os.close();
    return EXIT_SUCCESS;
}
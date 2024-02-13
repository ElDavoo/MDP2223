#include <cassert>
#include <iostream>
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <map>

template <typename T>
std::ostream& raw_write(std::ostream& os, T& content, std::streamsize size = sizeof(T)) {
    return os.write(reinterpret_cast<char*>(&content), size);
}

template <typename T>
class bitwriter {
    uint8_t buf_;
    uint8_t pos_;
    std::ofstream& os_;
    unsigned long counter;

    void write_bit(bool bit) {
        if (pos_ == 8) {
            raw_write(os_, buf_);
            counter++;
            pos_ = 0;
        }
        ++pos_;
        buf_ = (buf_ << 1) | (bit & 1);
    }
public:
    explicit bitwriter(std::ofstream& os) : os_(os) {
        buf_ = 0;
        pos_ = 0;
        counter = 0;
    }
    void write(T content, unsigned long n) {
        while (n-- > 0) {
            write_bit((content >> n) & 1);
        }
    }
    ~bitwriter() {
        while (pos_ != 1) write_bit(false);
        os_.close();
    }
};

class dictionary {
    std::map<std::vector<uint8_t>, uint32_t> dict_;
    //std::vector<std::vector<uint8_t>> dict_;
    uint8_t maxsize_;
    unsigned int maxindex_;
    uint32_t currentindex_;
public:
    explicit dictionary(const int maxsize) : maxsize_(maxsize) {
        maxindex_ = (1 << maxsize_) - 1;
        currentindex_ = 0;
    }
    int search(const std::vector<uint8_t>& candidate) {
        auto c = dict_.find(candidate);
        if (c != dict_.end()) {
            return c->second;
        }
        return -1;
        /*for (unsigned int i = 0; i < dict_.size(); ++i) {
            //if (dict_[i][0] != candidate[0]) continue;
            if (dict_[i] == candidate) return i + 1;
            //if (!dict_[i].compare(candidate)) return i + 1;

        }*/
        // return -1;
        //auto iter = std::find(dict_.begin(), dict_.end(), candidate);
        //if (iter == dict_.end()) return -1;
        //return std::distance(dict_.begin(), iter) + 1;
    }
    void push_back(const std::vector<uint8_t>& item) {
        assert(search(item) == -1);
        if (dict_.size() == maxindex_) {
            dict_.clear();
            currentindex_ = 0;
        }
        else {
            dict_.emplace(item, ++currentindex_);
        }
    }
    [[nodiscard]] unsigned int getMaxIndex() const { return maxindex_; }
    [[nodiscard]] unsigned int getMaxSize() const { return maxsize_; }
    [[nodiscard]] auto getDictSize() const { return dict_.size(); }



};

bool lz78encode(const std::string& input_filename, const std::string& output_filename, int maxbits) {
    std::ifstream is(input_filename, std::ios::binary);
    if (!is) return false;

    dictionary dict(maxbits);


    std::ofstream os(output_filename, std::ofstream::binary);
    if (!os) return false;

    bitwriter<uint32_t> bw(os);

    //os << "LZ78";
    bw.write('L', 8);
    bw.write('Z', 8);
    bw.write('7', 8);
    bw.write('8', 8);


    bw.write(dict.getMaxSize(), 5);


    while (!is.fail()) {
        std::vector<uint8_t> a;
        char c = 0;
        while (true) {
            if (is.peek() == EOF) {
                break;
            }
            is.read(&c, 1);
            // std::cout << c;
            a.push_back(c);
            if (dict.search(a) == -1) break;
        }
        if (a.size() == 1) {
            // std::cout << "(0," << c << ") ";
            unsigned long ciao = dict.getDictSize();
            unsigned long n = 0;
            while (ciao > 0) {
                ciao = ciao >> 1;
                n++;
            }
            bw.write(0, n);
            bw.write(c, 8);
            if (is.peek() != EOF) dict.push_back(a);
        }
        else if (!a.empty()) {
            a.pop_back();
            int i = dict.search(a);
            // std::cout << "(" << i << "," << c << ") ";
            unsigned long ciao = dict.getDictSize();
            unsigned long n = 0;
            while (ciao > 0) {
                ciao = ciao >> 1;
                n++;
            }
            bw.write(i, n);
            bw.write(c, 8);
            a.push_back(c);

            if (is.peek() != EOF) dict.push_back(a);
        }

    }

    return true;

}

int main(int argc, char** argv)
{
    if (argc != 4) return EXIT_FAILURE;
    bool result = lz78encode(argv[1], argv[2], std::stoi(argv[3]));

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
    
}
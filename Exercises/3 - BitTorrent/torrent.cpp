#include <vector>
#include <string>
#include <map>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>
#include <sstream>
#include <cstdint>
#include <iomanip>

typedef enum {S,I,L,D} type_t;

class belem {
	type_t type_;
	void* obj_;
public:
	explicit belem(std::ifstream& is);
	void walk(std::ostream& os, int size = 0) const;
};

class bencode {
	uint64_t length_;
	std::string string_;

public:
	explicit bencode(const std::string& string) : string_(string) {
		length_ = string_.size();
	}
	explicit bencode(std::ifstream& is) {
		is >> length_;
		char c = 0;
		is.read(&c, 1);
		assert(c == ':' && !is.fail());
		string_.resize(length_);
		is.read(&string_[0], length_);
	}
	bool operator< (const bencode &other) const{
		return string_ < other.string_;
	}

	std::string st() const {
		std::ostringstream oss;
		//oss << length_ << ":" << string_;
		oss << string_;
		std::string ostring = oss.str();
		return ostring;
	}

};

class bint{
	int64_t n_;
public:
	explicit bint(int64_t n) : n_(n) {}
	explicit bint(std::ifstream& is) {
		is >> n_;
		char c = 0;
		is.read(&c, 1);
		assert(c == 'e' && !is.fail());

	}
	[[nodiscard]] int64_t get() const { return n_; }
};




struct blist {
	std::vector<belem> list_;
	explicit blist(std::ifstream& is) {
		while (is.peek() != 'e') {
			list_.push_back(belem(is));
		}
		is.ignore(1);
	}
};

struct bdict {
	std::map<bencode, belem> dict_;
	explicit bdict(std::ifstream& is) {
		while (is.peek() != 'e') {
			bencode bs(is);
			belem be(is);
			auto pair = std::make_pair(bs, be);
			dict_.emplace(pair);
		}
		is.ignore(1);
	}
};

belem::belem(std::ifstream& is) {
	if (is.peek() == EOF) throw EOF;
	if (std::isdigit(is.peek())) {
		//std::cout << "s";
		bencode *bst = new bencode(is);
		obj_ = bst;
		type_ = S;
	}
	else {

		char c = 0;
		is.read(&c, 1);
		//std::cout << c;
		switch (c) {
		case 'i': {
			bint* bin = new bint(is);
			obj_ = bin;
			type_ = I;
		}
				break;
		case 'l':
		{
			blist *blis = new blist(is);
			obj_ = blis;
			type_ = L;
		}
		break;
		case 'd':
		{
			bdict *bdic = new bdict(is);
			obj_ = bdic;
			type_ = D;
		}
		break;
		default:
			throw c;
		}
	}
}

std::string sgen(unsigned int spaces, char c = '\t') {
	return std::string(spaces, c);
}

std::string ToHex(const std::string& input) {
	std::stringstream ss;
	ss << std::hex << std::nouppercase;
	for (unsigned char  c : input) {
		ss << std::setfill('0') << std::setw(2) << static_cast<int>(c);
	}
	return ss.str();
}

void belem::walk(std::ostream& os, int nspaces) const {
	switch (type_) {
	case L: {
		os << '[' << std::endl << sgen(nspaces);
		blist b = *(blist*)(obj_);
		for (auto e : b.list_) {
			os << sgen(1);
			e.walk(os, nspaces + 1);
			os << std::endl << sgen(nspaces);
		}
		os << ']';
	}
	break;
	case I: {
		bint b = *static_cast<bint*>(obj_);
		os << b.get();
	}
	break;
	case D: {
		os << '{' << std::endl << sgen(nspaces);
		bdict b = *(bdict*)(obj_);
		for (auto e : b.dict_) {
			os << sgen(1) << '"' << e.first.st() << "\" => ";
			if (e.first.st() == "pieces" && e.second.type_ == S) {
				bencode b = *(bencode*)(e.second.obj_);
				//assert(b.st().size() % 20 == 0);

				auto size = b.st().size();
				for (int i = 0; i < size; i = i + 20) {
					os << std::endl << sgen(nspaces + 2);
					os << ToHex(b.st().substr(i, 20));
				}
				// 
			}
			else {
				e.second.walk(os, nspaces + 1);
			}
			os << std::endl << sgen(nspaces);
		}
		os << '}';
	}
	break;
	case S: {
		bencode b = *(bencode*)(obj_);
		os << '"';
		for (char c : b.st()) {
			if (c < 32 || c > 126) os << '.';
			os << c;
		}
		os << '"';
	}
	break;
	default:
		throw type_;
	}
}
int main(int argc, char** argv) {
	
	if (argc != 2) return EXIT_FAILURE;

	std::ifstream is(argv[1]);
	if (!is) return EXIT_FAILURE;

	belem b(is);

	//std::cout << std::endl;

	b.walk(std::cout);

	return EXIT_SUCCESS;
}
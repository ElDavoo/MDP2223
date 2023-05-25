#include <iostream>
#include <fstream>
#include <list>
#include <map>
typedef enum { INT, LIST, DICT, STRING} type_t;

class Token {
	type_t type_;
	void* value_;
	bool is_iterator() {
		return type_ == LIST || type_ == DICT;
	}
public:
	Token(type_t type, void* value) : type_(type), value_(value) {

	}
	type_t type() { return type_; }
	void* value() { return value_; }
	void push_back(Token* t) {
		if (is_iterator()) {
			if (type_ == LIST) {
				static_cast<std::list<Token>*>(value_)->push_back(*t);
			}
			else {
				std::cout << "Hai provato a inserire un solo valore in un dizionario" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		else {
			std::cout << "Hai provato a inserire un valore in un altro valore" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	void push_back(Token* key, Token* value) {
		std::string a = "2" * 99;
		if (is_iterator()) {
			if (type_ == DICT) {
				std::map<Token, Token> t;
				//reinterpret_cast<std::map<Token, Token>*>(value_)->insert(std::pair<Token, Token>(*key, *value));
			}
			else {
				std::cout << "Hai provato a inserire una coppia chiave valore in una lista" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		else {
						std::cout << "Hai provato a inserire una coppia chiave valore in un altro valore" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
};

bool isdigit(char ch)
{
	return std::isdigit(static_cast<unsigned char>(ch));
}



void read_loop(std::istream* is, Token *l) {
	char c = is->peek();
	switch (c) {
	case 'i':
		std::cout << "int" << std::endl;
		int64_t i;
		*is >> i;
		// Quale è lo stato dopo la lettura? la e viene già letta?
		std::cout << "i:" << i << std::endl;
		std::cout << is->get() << std::endl;

		break;
	case 'l':
		std::cout << "list" << std::endl;
		is->get();
		while (is->peek() != 'e') {
			read_loop(is, l);
		}
		break;
	case 'd':
		std::cout << "dict" << std::endl;
		is->get();
		while (is->peek() != 'e') {
			// Create a new dictionary
			Token t(DICT, new std::map<Token, Token>);
			// Add the dictionary to the list
			l->push_back(&t);
			read_loop(is, l);
		}
		break;
	default:
		if (isdigit(c)) {
			std::cout << "digit (string)" << std::endl;
			uint64_t length;
			*is >> length;
			std::cout << "length:" << length << std::endl;
			if (is->get() != ':') {
				std::cout << "error" << std::endl;
				exit(EXIT_FAILURE);
			}
			char* str = new char[length + 1];
			is->read(str, length);
			str[length] = '\0';
			std::cout << "string:" << str << std::endl;
			delete[] str;

		} else {
			std::cout << "error" << std::endl;
			exit(EXIT_FAILURE);
		}

	}
}

int main(int argc, char* argv[]) {
	if (argc != 2) return 1;
	std::ifstream is(argv[1], std::ios::binary);
	std::list<Token> l;
	// empty list of pointers
	Token t(LIST, &l);
	if (!is) {
		std::cout << "Apertura file fallita" << std::endl;
		return 1;
	}
	read_loop(&is, &t);
}
#include <cstdlib>
#include <cstdint>
#include <stdio.h>
#include <iterator>
#include <iostream>
#include <fstream>

#include <algorithm>

template <class T> class arr {
private:
	T* arr_;
	uint64_t n_;
	uint64_t cap_;

	void reall() {
		if (n_ != cap_) return;
		cap_ = cap_ * 1.1 + 2;
		T* tmp = arr_;
		arr_ = new T[cap_];
		for (int i = 0; i < n_; i++) {
			arr_[i] = tmp[i];
		}
		delete[] tmp;
		//realloc(arr_, cap_ * sizeof(T));

	}
	template <typename U> 
	friend std::ostream& operator<< (std::ostream& out, const arr<U>& arr);

public:
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using reference = T&;
	using iterator_category = std::random_access_iterator_tag;


	arr() {
		puts("No argument constructor");
		arr_ = nullptr;
		n_ = 0;
		cap_ = 0;
	}
	arr(const int size) {
		puts("sized constructor");
		cap_ = size;
		arr_ = new T[size];
	}
	arr(const arr& other) {
		puts("costruttore di copia");
		n_ = other.n_;
		cap_ = other.n_;
		arr_ = new T[n_];
		for (int i = 0; i < n_; i++) {
			arr_[i] = other.arr_[i];
		}
	}
	arr& operator=(const arr &other) {
		puts("assegnatore di copia");
		if (this != &other) {
			n_ = other.n_;
			cap_ = other.n_;
			arr_ = new T[n_];
			for (int i = 0; i < n_; i++) {
				arr_[i] = other.arr_[i];
			}
		}
		return *this;
	}
	arr& operator=(arr&& other) {
		puts("assegnatore di spostamento");
		arr_ = other.arr_;
		n_ = other.n_;
		cap_ = other.cap_;
		other.arr_ = nullptr;
		return *this;

	}
	arr(arr &&other) {
		puts("costruttore di spostamento");
		n_ = other.n_;
		cap_ = other.cap_;
		arr_ = other.arr_;
		other.arr_ = nullptr;
	}
	~arr() {
		puts("distruttore");
		if (arr_ != nullptr) delete[] arr_;
	}
	void push_back(const T &obj) {
		reall();
		arr_[n_++] = obj;
	}
	T& operator[](const int i) {
		puts("accesso nornmale");
		return arr_[i];
	}
	const T& operator[](const int i) const {
		puts("accesso const");
		return arr_[i];
	}
	T* begin() {
		std::cout << "begin iterator" << std::endl;
		return &arr_[0];
	}
	T* end() {
		std::cout << "end iterator" << std::endl;
		return &arr_[n_];
	}
	const T* begin() const {
		std::cout << "begin const iterator" << std::endl;
		return &arr_[0];
	}
	const T* end() const {
		std::cout << "end const iterator" << std::endl;
		return &arr_[n_];
	}

};

template <typename T>
std::ostream& operator<< (std::ostream& out, const arr<T> &arr) {
	out << "n: " << arr.n_ << ", cap: " << arr.cap_, " ; ";
	for (const auto &e : arr) {
		out << e;
		if (&e != *arr.end()) out << ", ";
	}
	return out;
}


int main(int argc, char** argv) {
	if (argc != 3) return EXIT_FAILURE;

	std::ifstream inputs(argv[1]);
	if (!inputs) return EXIT_FAILURE;

	std::ofstream outputs(argv[2]);
	if (!outputs) return EXIT_FAILURE;

	arr<int32_t> arr;
	int32_t num;

	while (inputs >> num) {
		arr.push_back(num);
	}

	inputs.close();



	std::sort(arr.begin(), arr.end());

	for (const auto &e : arr) {
		outputs << e << std::endl;
	}

	outputs.close();

	return EXIT_SUCCESS;
}

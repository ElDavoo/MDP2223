#include <fstream>
#include <iterator>
#include <unordered_map>
#include <memory>
#include <vector>
#include <queue>

template<typename T>
struct frequency {
	std::unordered_map<T, size_t> _count;
	void operator() (const T& sym) { ++_count[sym]; }
	auto begin() -> decltype(_count.begin()) { return _count.begin(); }
	auto end() -> decltype(_count.end()) { return _count.end(); }
	auto begin() const -> decltype(_count.begin()) { return _count.begin(); }
	auto end() const -> decltype(_count.end()) { return _count.end(); }
};
template<typename T>
class huffman {
public:
	struct elem {
		T _sym;
		uint8_t _len = 0;
		uint32_t _code = 0;
		elem() {}
		elem(const T& sym) : _sym(sym) {}
		bool operator<(const elem& rhs) const {
			if (_len < rhs._len)
				return true;
			else if (_len > rhs._len)
				return false;
			else
				return _sym < rhs._sym;
		}
	};
private:
	std::vector<elem> _table;
	struct node {
		elem _elem;
		size_t _count;
		node* _left = nullptr;
		node* _right = nullptr;
		node* _parent = nullptr;
		node(const T& sym, size_t count) : _elem(sym), _count(count) {}
		node(node* left, node* right) : _count(left->_count + right->_count), _left(left), _right(right) {
			_left->_parent = this;
			_right->_parent = this;
		}
		bool leaf() const {
			return _left == nullptr;
		}
		bool operator>(const node& rhs) const {
			return _count > rhs._count;
		}
	};
	static void create_table(node* node, std::vector<elem>& tab, uint8_t len = 0) {
		if (!node->leaf()) {
			create_table(node->_left, tab, len + 1);
			create_table(node->_right, tab, len + 1);
		}
		else {
			node->_elem._len = len;
			tab.push_back(node->_elem);
		}
	}
	template<typename TX>
	struct ptr_greater : public std::binary_function<TX, TX, bool> {
		bool operator()(const TX& lhs, const TX& rhs) const {
			return *lhs > *rhs;
		}
	};
public:
	huffman(const frequency<T>& f) {
		std::priority_queue<node*, std::vector<node*>, ptr_greater<node*>> pq;
		std::vector<std::unique_ptr<node>> nodes;
		for (auto x : f) {
			nodes.emplace_back(std::make_unique<node>(x.first, x.second));
			pq.push(nodes.back().get());
		}
		while (pq.size() > 1) {
			auto n1 = pq.top();
			pq.pop();
			auto n2 = pq.top();
			pq.pop();

			nodes.emplace_back(std::make_unique<node>(n1, n2));
			pq.push(nodes.back().get());
		}
		node* root = pq.top();
		pq.pop();
		create_table(root, _table);
		sort(begin(_table), end(_table));
		uint8_t len = 0;
		uint32_t code = 0;
		for (auto& x : _table) {
			code <<= x._len - len;
			len = x._len;
			x._code = code;
			++code;
		}
	}
	const std::vector<elem>& table() const {
		return _table;
	}
};

int main() 
{
	std::ifstream is("bibbia.txt", std::ios::binary);
	if (!is) {
		return EXIT_FAILURE;
	}

	// Calcolo le frequenze dei byte nel file - Compute byte frequencies in the provided file
	frequency<uint8_t> f;
	f = std::for_each(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), f);
	// Aggiungo 1 a ogni possibile byte per essere sicuro di avere un codice di Huffman per ogni possibile simbolo
	// Add 1 to every byte ensuring that every symbol gets a Huffman code
	for (int i = 0; i < 256; ++i)
		f(i);
	// Calcolo i codici canonici di Huffman - Compute canonical Huffman codes
	huffman<uint8_t> huff(f);

	// Qui bisogna salvare la tabella di Huffman nel file table.bin
	// Here you need to save the Huffman table onto table.bin

	// Una chiamata a huff.table() restituisce un std::vector<elem> che è la
	// tabella dei codici di Huffman. Ogni elem ha un uint8_t _sym (il byte/carattere),
	// un uint8_t _len (la lunghezza del codice) e un uint32_t _code. I _len
	// bit meno significativi di _code sono il codice di Huffman.
	// A call to huff.table() returns a std::vector<elem> which is the 
	// Huffman table. Each elem has a uint8_t _sym (the byte/character),
	// a uint8_t _len (the code length), and a uint32_t _code. The _len
	// least significant bits of _code are the Huffman code.
	
	// TODO

	return EXIT_SUCCESS;
}
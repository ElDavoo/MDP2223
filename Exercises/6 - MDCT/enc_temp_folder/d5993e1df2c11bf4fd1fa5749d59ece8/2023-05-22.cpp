#include <iostream>
#include <math.h>
#include <numbers>
#include <vector>
#include <fstream>

#define pi std::numbers::pi

double weight(double N, double n) {
    return sin((pi / 2 * N)*(n + 0.5));
}

double couscous(double N, double n, double k) {
    return cos((pi / N) * (n + 0.5 + (N / 2)) * (k + 0.5));
}

double mdct_formula(double sample, double n, double N, double k) {
    return sample * weight(N, n) * couscous(N, n, k);
}

std::vector<double> mdct(std::vector<double> samples) {
    size_t N = (samples.size() + 1) / 2;
    auto out = std::vector<double>(N);
    for (int k = 0; k < N - 1; ++k) {
        for (int n = 0; n < 2 * N - 1; ++n) {
            out[k] += mdct_formula(samples[n], n, N, k);
		}
	}
    	return out;
}

std::vector<double> imdct(std::vector<double> samples) {
    size_t N = samples.size();
    auto out = std::vector<double>(2 * N);
    for (int n = 0; n < 2 * N - 1; ++n) {
        for (int k = 0; k < N - 1; ++k) {
			out[n] += samples[k] * couscous(N, n, k);
		}
        out[n] *= weight(N, n) * 2 / N;
	}
    return out;
}

void quantize(std::vector<int16_t> &input, int factor) {
    double temp = 0;
    for (int i = 0; i < input.size(); ++i) {
        if (input[i] == -4878) {
            i = i;
        }
		temp = (input[i] / (double)factor) * INT16_MAX;
        std::cout << input[i] << "\t-->\t" << static_cast<int16_t>(temp) << std::endl;
        if (!(i % 100)) {
            i = i;
        }
        input[i] = static_cast<int16_t>(temp);
    }
}

void dequantize(std::vector<int16_t>& input, int factor) {
	double temp = 0;
    for (int i = 0; i < input.size(); ++i) {
		temp = (input[i] * INT16_MAX) / factor;
        std::cout << input[i] << "\t-->\t" << static_cast<int16_t>(temp) << std::endl;
        if (!(i % 100)) {
            i = i;
        }
		input[i] = static_cast<int16_t>(temp);
	}
}

double get_entropy(std::vector<int16_t> input) {
    double entropy = 0;
	double p = 0;
    for (int i = 0; i < input.size(); ++i) {
		p = input[i] / INT16_MAX;
		entropy += p * log2(p);
	}
	return -entropy;

}

int main()
{
// Open test.raw as binary
    std::ifstream file("test.raw", std::ios::binary);
    if (!file) {
		std::cout << "Error opening file" << std::endl;
		return 1;
	}
    std::vector<int16_t> samples;
    int16_t sample;
    while (file.read(reinterpret_cast<char*>(&sample), sizeof(sample))) {
        samples.push_back(sample);
    }
    file.close();
    auto original = samples;
    std::cout << "Entropy: " << get_entropy(samples) << std::endl;
    quantize(samples, 2600);
    std::cout << "Entropy: " << get_entropy(samples) << std::endl;
    dequantize(samples, 2600);
    std::cout << "Entropy: " << get_entropy(samples) << std::endl;
    // save dequantized to output_qt.raw
    std::ofstream output_qt("output_qt.raw", std::ios::binary);
    if (!output_qt) {
        std::cout << "Error opening file" << std::endl;
        return 1;
    }
    for (int i = 0; i < samples.size(); ++i) {
		output_qt.write(reinterpret_cast<char*>(&samples[i]), sizeof(samples[i]));
	}
    auto error = original;
    for (int i = 0; i < error.size(); ++i) {
        error[i] -= samples[i];
    }
    std::ofstream error_qt("error_qt.raw", std::ios::binary);
    if (!error_qt) {
		std::cout << "Error opening file" << std::endl;
		return 1;
	}
    for (int i = 0; i < error.size(); ++i) {
        error_qt.write(reinterpret_cast<char*>(&error[i]), sizeof(error[i]));
    }



}

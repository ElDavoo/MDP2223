#include <vector>
#include <cstdint>
#include <cmath>

int main(void) {
	const nseconds = 10;
	const int sample_freq = 44100;
	const int nsamples = nseconds * sample_freq;
	std::vector<int16_t> samples(nsamples);

	const double freq = 440.0;
	const double amplitude = INT16_MAX;

	for (size_t i = 0; i < nsamples; ++i) {
		samples[i] = round(amplitude * std::sin(2 * M_PI * i / sample_freq));
	}

	std::ofstream os("output.raw", std::ios::binary);
	os.write(reinterpret_cast<char*>(samples.data()), samples.size() * sizeof(int16_t));
	os.close();
}
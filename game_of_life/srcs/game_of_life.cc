#include "../libs/game_of_life.hh"

using namespace GOL;

bool bit_set::getn_in_el(size_t n, char el) const noexcept {
	return (el >> n) & 1;
}

bool bit_set::getb(const size_t n, const size_t m) const {
	if (n >= __n) throw std::invalid_argument("Wrong access to first dimension\n");
	if (m >= __m) throw std::invalid_argument("Wrong access to second dimension\n");

	return getn_in_el(m % C2B, __data[n * (__m/C2B + __is_alignment) + m/C2B]);
}

void bit_set::dump(const char el, std::ostream &out, const size_t count) const noexcept {
	size_t mn = std::min(C2B, count);
	for (size_t i = 0; i < mn; ++i) out << (getn_in_el(i, el) ? "X" : "0");
}

void bit_set::dump(std::ostream &out, const bool buf_num) const noexcept {
	unsigned char *buf = ((buf_num) ? __data : __temp);
	for (size_t i = 0; i < __n; ++i) {
		for (size_t j = 0; j < __m/C2B; ++j) {
			dump(buf[i * (__m/C2B + __is_alignment) + j], out);
		}
		dump(buf[i * (__m/C2B + __is_alignment) + __m/C2B], out, __m%C2B);
		out << std::endl;
	}
}

void bit_set::setb(const size_t n, const size_t m, const bool state) {
	__temp[n * (__m/C2B + __is_alignment) + m/C2B] ^= (-(unsigned char)state ^ __temp[n * (__m/C2B + __is_alignment) + m/C2B]) & (1UL << m%C2B); 
}

void bit_set::swap_bufp() noexcept {
	unsigned char *buf = __data;
	__data = __temp, __temp = buf;
	__state = !__state;
}

void bit_set::rand_filling() {
	srand(time(NULL));
	for (size_t i = 0; i < __n; ++i) {
		for (size_t j = 0; j < __m; ++j) {
			setb(i, j, rand() % 2);
		}
	}
	swap_bufp();
}

uint64_t game_t::get_borders(uint64_t last_num, int rank, int np) {
	uint32_t right_border = 1 + (last_num / np) * rank ;
	uint32_t left_border  = (rank == np - 1) ? last_num : (last_num / np) * (rank + 1);
		uint32_t temporary = left_border - right_border;
		temporary = ((temporary == 0) ? 0 : temporary + 1); 
	return ((uint64_t)right_border << 32) | (temporary); 
}

void game_t::dump(std::ostream &out, const bool buf_num) const noexcept {
	__data->dump(out, buf_num);
}

size_t game_t::neighbours_count(const size_t i, const size_t j) const noexcept {
	for ()
}

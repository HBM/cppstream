#ifndef _HBM_SIGNALEXTRACT_H
#define _HBM_SIGNALEXTRACT_H

#include <cstring>
#include <stdint.h>

#include "endianess.h"

namespace hbm {
	namespace streaming {
		enum endian {little, big};

		template<size_t n, endian e>
		struct swap_impl {};

		template<>
		struct swap_impl<4, big> {
			static inline uint32_t swap(uint32_t value) { return be32toh(value); }
		};

		template<>
		struct swap_impl<4, little> {
			static inline uint32_t swap(uint32_t value) { return le32toh(value); }
		};

		template<>
		struct swap_impl<8, big> {
			static inline uint64_t swap(uint64_t value) { return be64toh(value); }
		};

		template<>
		struct swap_impl<8, little> {
			static inline uint64_t swap(uint64_t value) { return le64toh(value); }
		};

		template<typename T> struct copy_traits;
		template<> struct copy_traits<uint32_t> {typedef uint32_t copy_type;};
		template<> struct copy_traits<int32_t> {typedef uint32_t copy_type;};
		template<> struct copy_traits<uint64_t> {typedef uint64_t copy_type;};
		template<> struct copy_traits<int64_t> {typedef uint64_t copy_type;};
		template<> struct copy_traits<float> {typedef uint32_t copy_type;};
		template<> struct copy_traits<double> {typedef uint64_t copy_type;};

		template<typename RET, endian e>
		struct extract {
			inline RET operator()(unsigned char **p) const
			{
				typedef copy_traits<RET> traits;
				typename traits::copy_type temp;
				std::memcpy(&temp, *p, sizeof(temp));
				swap_impl<sizeof(temp), e> swapper;
				temp = swapper.swap(temp);
				RET value;
				std::memcpy(&value, &temp, sizeof(value));
				*p = (*p) + sizeof(RET);
				return value;
			}
		};
	}
}
#endif

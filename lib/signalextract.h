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

		template<typename RET, size_t size, endian e>
		struct extract_impl {};

		template<typename RET, endian e>
		struct extract_impl<RET, 4, e> {
			inline RET operator()(unsigned char **p) const
			{
				uint32_t temp;
				std::memcpy(&temp, *p, sizeof(temp));
				swap_impl<sizeof(temp), e> swapper;
				temp = swapper.swap(temp);
				RET value;
				std::memcpy(&value, &temp, sizeof(value));
				*p = (*p) + sizeof(RET);
				return value;
			}
		};

		template<typename RET, endian e>
		struct extract_impl<RET, 8, e> {
			inline RET operator()(unsigned char **p) const
			{
				uint64_t temp;
				std::memcpy(&temp, *p, sizeof(temp));
				swap_impl<sizeof(temp), e> swapper;
				temp = swapper.swap(temp);
				RET value;
				std::memcpy(&value, &temp, sizeof(value));
				*p = (*p) + sizeof(RET);
				return value;
			}
		};

		template<typename RET, endian e>
		struct extract {
			inline RET operator()(unsigned char **p) const
			{
				return extract_impl<RET, sizeof(RET), e>()(p);
			}
		};
	}
}
#endif

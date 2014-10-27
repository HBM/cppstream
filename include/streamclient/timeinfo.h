#ifndef _HBM__STREAMING__TIMEINFO
#define _HBM__STREAMING__TIMEINFO

#include <stdint.h>

#include <boost/multiprecision/cpp_int.hpp>

namespace hbm {
	namespace streaming {
		class timeInfo_t {
		public:
			timeInfo_t();
			timeInfo_t(boost::multiprecision::uint128_t timeStamp);
			boost::multiprecision::uint128_t timeStamp() const;
			void set(const Json::Value& StampNode);
			void setTimestamp(boost::multiprecision::uint128_t timeStamp);
			void clear();
			void increment(const timeInfo_t& op);

			uint32_t era() const;
			uint32_t seconds() const;
			uint32_t fraction() const;
			uint32_t subFraction() const;

		private:
			/// the upper 32 bit are the era
			/// next follows 32 bit seconds
			/// followed by 32 bit with the fraction of a second
			/// the lower 32 bits is the sub fraction of a second
			boost::multiprecision::uint128_t m_timestamp;
		};
	}
}
#endif

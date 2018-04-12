// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#include <cstdint>
#include <iostream>
#include "deltatimeinfo.h"

namespace hbm {
	namespace streaming {
		deltaTimeInfo::deltaTimeInfo()
			: m_deltaNtpTimestamp(0)
			, m_deltaSubFraction(0)
			, m_deltaSubFractionCollected(0)

			, m_time()
		{
		}

		uint64_t deltaTimeInfo::increment(unsigned int count)
		{
			uint64_t delta = m_deltaNtpTimestamp*count;

			// transfer sub fraction overflow to fractions
			m_deltaSubFractionCollected += m_deltaSubFraction * count;
			delta += (m_deltaSubFractionCollected >> 32) & UINT32_MAX;
			// remove overflow that was added to the fractions
			m_deltaSubFractionCollected &= UINT32_MAX;

			return m_time.add(delta);
		}

		void deltaTimeInfo::setTime(const Json::Value& params)
		{
			// we are getting the correct time => clear collected correction!
			m_deltaSubFractionCollected = 0;
			m_time.set(params);
		}


		void deltaTimeInfo::setDelta(const Json::Value& params)
		{
			uint32_t samples = 1;
			uint32_t seconds = 0;
			uint32_t fraction = 0;
			uint32_t subFraction = 0;

			Json::Value value;

			value = params["samples"];
			if(value.isNull()==false) {
				samples = value.asUInt();
				if(samples==0) {
					throw std::runtime_error("samples must be > 0");
				}
			}

			const Json::Value& timeObject = params["delta"];
			if(timeObject["type"]=="ntp") {
				value = timeObject["seconds"];
				if(value.isNull()==false) {
					seconds = value.asUInt();
				}

				value = timeObject["fraction"];
				if(value.isNull()==false) {
					fraction = value.asUInt();
				}

				value = timeObject["subFraction"];
				if(value.isNull()==false) {
					subFraction = value.asUInt();
				}
			}
			uint64_t ntpTimestamp = seconds;
			ntpTimestamp <<= 32;
			ntpTimestamp |= fraction;



			// we are loosing precision here. In order to compensate this, we calculate a correction to use.
			m_deltaNtpTimestamp = ntpTimestamp/samples;
			m_deltaSubFraction = subFraction/samples;

			// determine remainder and calculate sub fraction from it.
			uint64_t remainder = m_deltaNtpTimestamp%samples;
			remainder <<= 32;
			remainder /= samples;
			m_deltaSubFraction += static_cast < uint32_t > (remainder & 0xffffffff);

			std::cout << __FUNCTION__ << std::endl;
			std::cout << "seconds = " << seconds << std::hex << " (0x" << seconds << ")" << std::dec << std::endl;
			std::cout << "fraction = " << fraction << std::hex << " (0x" << fraction << ")" << std::dec << std::endl;
			std::cout << "subFraction = " << subFraction << std::hex << " (0x" << subFraction << ")" << std::dec << std::endl;
		}

		void deltaTimeInfo::clear()
		{
			m_deltaNtpTimestamp = 0;
			m_deltaSubFraction = 0;
			m_deltaSubFractionCollected = 0;

			m_time.clear();
		}

		uint64_t deltaTimeInfo::ntpTimeStamp() const
		{
			return m_time.ntpTimeStamp();
		}
	}
}

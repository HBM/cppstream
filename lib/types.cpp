#include <vector>
#include <stdexcept>
#include <iostream>
#include <json/value.h>

#include "types.h"

namespace hbm {
	namespace streaming {
		timeInfo_t::timeInfo_t()
			: m_ntpTimestamp(0)
		{
		}

		timeInfo_t::timeInfo_t(uint64_t ntpTimeStamp)
			: m_ntpTimestamp(ntpTimeStamp)
		{
		}

		uint64_t timeInfo_t::ntpTimeStamp() const
		{
			return m_ntpTimestamp;
		}

		uint32_t timeInfo_t::seconds() const
		{
			return m_ntpTimestamp >> 32;
		}
		uint32_t timeInfo_t::fractions() const
		{
			return m_ntpTimestamp & 0xffffffff;
		}


		void timeInfo_t::set(const Json::Value& timeObject)
		{
			try {
				if(timeObject["type"]=="ntp") {
					m_ntpTimestamp = timeObject["seconds"].asUInt();
					m_ntpTimestamp <<= 32;
					m_ntpTimestamp |= timeObject["fraction"].asUInt();
				}
			} catch(const std::runtime_error& e) {
				std::cerr << e.what();
			}
		}

		void timeInfo_t::setNtpTimestamp(uint64_t ntpTimeStamp)
		{
			m_ntpTimestamp = ntpTimeStamp;
		}

		void timeInfo_t::clear()
		{
			m_ntpTimestamp = 0;
		}

		void timeInfo_t::increment(const timeInfo_t& inc)
		{
			m_ntpTimestamp += inc.m_ntpTimestamp;
		}
	}
}

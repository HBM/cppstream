#include <stdexcept>
#include <iostream>
#include <json/value.h>

#include "timeinfo.h"

namespace hbm {
	namespace streaming {
		timeInfo::timeInfo()
			: m_era(0)
			, m_ntpTimestamp(0)
			, m_subFraction(0)
		{
		}

		uint64_t timeInfo::ntpTimeStamp() const
		{
			return m_ntpTimestamp;
		}

		uint32_t timeInfo::era() const
		{
			return m_era;
		}

		uint32_t timeInfo::seconds() const
		{
			return m_ntpTimestamp >> 32;
		}
		uint32_t timeInfo::fraction() const
		{
			return m_ntpTimestamp & 0xffffffff;
		}

		uint32_t timeInfo::subFraction() const
		{
			return m_subFraction;
		}

		void timeInfo::set(const Json::Value& timeObject)
		{
			try {
				if(timeObject["type"]=="ntp") {
					m_era = timeObject["era"].asUInt();
					m_ntpTimestamp = timeObject["seconds"].asUInt();
					m_ntpTimestamp <<= 32;
					m_ntpTimestamp |= timeObject["fraction"].asUInt();
					m_subFraction = timeObject["subFraction"].asUInt();
				}
			} catch(const std::runtime_error& e) {
				std::cerr << e.what();
			}
		}


		void timeInfo::clear()
		{
			m_ntpTimestamp = 0;
		}

		void timeInfo::add(uint64_t inc)
		{
			m_ntpTimestamp += inc;
		}
	}
}

#include <vector>
#include <stdexcept>
#include <iostream>
#include <json/value.h>

#include "timeinfo.h"

namespace hbm {
	namespace streaming {
		timeInfo::timeInfo()
			: m_ntpTimestamp(0)
		{
		}

		timeInfo::timeInfo(uint64_t ntpTimeStamp)
			: m_ntpTimestamp(ntpTimeStamp)
		{
		}

		uint64_t timeInfo::ntpTimeStamp() const
		{
			return m_ntpTimestamp;
		}

		uint32_t timeInfo::seconds() const
		{
			return m_ntpTimestamp >> 32;
		}
		uint32_t timeInfo::fractions() const
		{
			return m_ntpTimestamp & 0xffffffff;
		}


		void timeInfo::set(const Json::Value& timeObject)
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

		void timeInfo::setNtpTimestamp(uint64_t ntpTimeStamp)
		{
			m_ntpTimestamp = ntpTimeStamp;
		}

		void timeInfo::clear()
		{
			m_ntpTimestamp = 0;
		}

		void timeInfo::increment(const timeInfo& inc)
		{
			m_ntpTimestamp += inc.m_ntpTimestamp;
		}
	}
}

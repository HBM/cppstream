// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

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
				const Json::Value& stmapNode = timeObject["stamp"];
				if(stmapNode["type"]=="ntp") {
					uint32_t seconds = 0;
					uint32_t fraction = 0;
					Json::Value value;
					value = stmapNode["era"];
					if(value.isNull()) {
						m_era = 0;
					} else {
						m_era = value.asUInt();
					}

					value = stmapNode["seconds"];
					if(value.isNull()==false) {
						seconds = value.asUInt();
					}

					value = stmapNode["fraction"];
					if(value.isNull()==false) {
						fraction = value.asUInt();
					}
					m_ntpTimestamp = seconds;
					m_ntpTimestamp <<= 32;
					m_ntpTimestamp |= fraction;

					value = stmapNode["subFraction"];
					if(value.isNull()) {
						m_subFraction = 0;
					} else {
						m_subFraction = value.asUInt();
					}
				}
			} catch(const std::runtime_error& e) {
				std::cerr << e.what();
			}
		}


		void timeInfo::clear()
		{
			m_ntpTimestamp = 0;
		}

		uint64_t timeInfo::add(uint64_t inc)
		{
			m_ntpTimestamp += inc;
			return m_ntpTimestamp;
		}
	}
}

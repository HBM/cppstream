#include <vector>
#include <stdexcept>
#include <iostream>
#include <json/value.h>

#include "timeinfo.h"

namespace hbm {
	namespace streaming {
		timeInfo_t::timeInfo_t()
			: m_timestamp(0)
		{
		}

		timeInfo_t::timeInfo_t(boost::multiprecision::uint128_t timeStamp)
			: m_timestamp(timeStamp)
		{
		}

		boost::multiprecision::uint128_t timeInfo_t::timeStamp() const
		{
			return m_timestamp;
		}

		uint32_t timeInfo_t::era() const
		{
			return static_cast < uint32_t > (m_timestamp >> (32*3));
		}

		uint32_t timeInfo_t::seconds() const
		{
			return static_cast < uint32_t > ((m_timestamp >> (32*2)) & 0xffffffff);
		}

		uint32_t timeInfo_t::fraction() const
		{
			return static_cast < uint32_t > ((m_timestamp >> 32) & 0xffffffff);
		}

		uint32_t timeInfo_t::subFraction() const
		{
			return static_cast < uint32_t > (m_timestamp & 0xffffffff);
		}


		void timeInfo_t::set(const Json::Value& timeObject)
		{
			try {
				if(timeObject["type"]=="ntp") {
					m_timestamp = timeObject["era"].asUInt();
					m_timestamp <<= 32;
					m_timestamp |= timeObject["seconds"].asUInt();
					m_timestamp <<= 32;
					m_timestamp |= timeObject["fraction"].asUInt();
					m_timestamp <<= 32;
					m_timestamp |= timeObject["subFraction"].asUInt();
				}
			} catch(const std::runtime_error& e) {
				std::cerr << "error setting time info from JSON '" << e.what() << "'";
			}
		}

		void timeInfo_t::setTimestamp(boost::multiprecision::uint128_t timeStamp)
		{
			m_timestamp = timeStamp;
		}

		void timeInfo_t::clear()
		{
			m_timestamp = 0;
		}

		void timeInfo_t::increment(const timeInfo_t& inc)
		{
			m_timestamp += inc.m_timestamp;
		}
	}
}

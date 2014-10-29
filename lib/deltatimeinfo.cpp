#include "deltatimeinfo.h"

namespace hbm {
	namespace streaming {
		deltaTimeInfo::deltaTimeInfo()
			: m_deltaNtpTimestamp(0)
			, m_deltaSubFraction(0)
			, m_deltaSubFractionCollected(0)
		{
		}

		void deltaTimeInfo::increment(unsigned int count)
		{
			uint64_t delta = m_deltaNtpTimestamp*count;

			// add sub fractions overflowed to fraction
			m_deltaSubFractionCollected += m_subFraction * count;
			delta += (m_deltaSubFractionCollected >> 32) & 0xffffffff;

			add(delta);
		}

		void deltaTimeInfo::setSignalRate(const Json::Value& params)
		{
			uint32_t samples = 1;
			uint64_t ntpTimestamp = 0;
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
					ntpTimestamp = value.asUInt();
					ntpTimestamp <<= 32;
				}

				value = timeObject["fraction"];
				if(value.isNull()==false) {
					ntpTimestamp |= value.asUInt();
				}

				value = timeObject["subFraction"];
				if(value.isNull()==false) {
					subFraction = value.asUInt();
				}
			}


			// we are loosing precision here. In order to compensate this, we calculate a correction to use.
			m_deltaNtpTimestamp = ntpTimestamp/samples;
			m_subFraction = subFraction/samples;

			// determine remainder and calculate sub fraction from it.
			uint64_t rest = m_ntpTimestamp%samples;
			rest <<= 32;
			rest /= samples;
			m_deltaSubFraction += static_cast < uint32_t > (rest & 0xffffffff);
		}
	}
}

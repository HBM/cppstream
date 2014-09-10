#include "subscribedsignals.h"
#include "subscribedsignal.h"

namespace hbm {
	namespace streaming {
		SubscribedSignals::SubscribedSignals()
			: m_subscribedSignals()
		{
		}

		void SubscribedSignals::processMeasuredData(unsigned int signalNumber, unsigned char* data, size_t len)
		{
			m_subscribedSignals[signalNumber].processData(data, len);
		}

		void SubscribedSignals::processMetaInformation(unsigned int signalNumber, const std::string& method, const Json::Value& params)
		{
			m_subscribedSignals[signalNumber].processSignalMetaInformation(method, params);
		}

		size_t SubscribedSignals::erase(unsigned int signalNumber)
		{
			return m_subscribedSignals.erase(signalNumber);
		}

		void SubscribedSignals::clear()
		{
			m_subscribedSignals.clear();
		}
	}
}

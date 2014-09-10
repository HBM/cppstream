#include "subscribedsignals.h"
#include "subscribedsignal.h"

namespace hbm {
	namespace streaming {
		SubscribedSignals::SubscribedSignals()
			: m_subscribedSignals()
			, m_signalMetaCb()
		{
		}

		void SubscribedSignals::setSignalMetaCb(SignalMetaCb_t cb)
		{
			m_signalMetaCb = cb;
		}

		void SubscribedSignals::processMeasuredData(unsigned int signalNumber, unsigned char* data, size_t len)
		{
			m_subscribedSignals[signalNumber].processData(data, len);
		}

		void SubscribedSignals::processMetaInformation(unsigned int signalNumber, const std::string& method, const Json::Value& params)
		{
			SubscribedSignal& signal = m_subscribedSignals[signalNumber];
			if (m_signalMetaCb) {
				m_signalMetaCb(signal, method, params);
			}
			signal.processSignalMetaInformation(method, params);

			if(method=="unsubscribe") {
				m_subscribedSignals.erase(signalNumber);
			}
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

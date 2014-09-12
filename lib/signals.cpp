#include "signals.h"
#include "subscribedsignal.h"

namespace hbm {
	namespace streaming {
		Signals::Signals()
			: m_subscribedSignals()
			, m_signalMetaCb()
			, m_dataCb()
		{
		}

		void Signals::setSignalMetaCb(SignalMetaCb_t cb)
		{
			m_signalMetaCb = cb;
		}

		void Signals::setDataCb(DataCb_t cb)
		{
			m_dataCb = cb;
		}

		void Signals::processMeasuredData(unsigned int signalNumber, unsigned char* data, size_t len)
		{
			m_subscribedSignals[signalNumber].processData(data, len, m_dataCb);
		}

		void Signals::processMetaInformation(unsigned int signalNumber, const std::string& method, const Json::Value& params)
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

		void Signals::clear()
		{
			m_subscribedSignals.clear();
		}
	}
}

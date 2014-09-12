#include "signals.h"
#include "subscribedsignal.h"

namespace hbm {
	namespace streaming {
		Signals::Signals()
			: m_signals()
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

		int Signals::processMeasuredData(unsigned int signalNumber, unsigned char* data, size_t len)
		{
			signals_t::iterator iter = m_signals.find(signalNumber);
			if (iter != m_signals.end()) {
				iter->second.processData(data, len, m_dataCb);
				return 0;
			}
			return -1;
		}

		void Signals::processMetaInformation(unsigned int signalNumber, const std::string& method, const Json::Value& params)
		{
			SubscribedSignal& signal = m_signals[signalNumber];
			if (m_signalMetaCb) {
				m_signalMetaCb(signal, method, params);
			}
			signal.processSignalMetaInformation(method, params);

			if(method=="unsubscribe") {
				m_signals.erase(signalNumber);
			}
		}

		void Signals::clear()
		{
			m_signals.clear();
		}
	}
}

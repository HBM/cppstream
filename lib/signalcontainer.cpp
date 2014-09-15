#include "signalcontainer.h"
#include "subscribedsignal.h"

namespace hbm {
	namespace streaming {
		SignalContainer::SignalContainer()
			: m_subscribedsignals()
			, m_signalMetaCb()
			, m_dataCb()
		{
		}

		void SignalContainer::setSignalMetaCb(SignalMetaCb_t cb)
		{
			m_signalMetaCb = cb;
		}

		void SignalContainer::setDataCb(DataCb_t cb)
		{
			m_dataCb = cb;
		}

		ssize_t SignalContainer::processMeasuredData(unsigned int signalNumber, unsigned char* data, size_t len)
		{
			signals_t::iterator iter = m_subscribedsignals.find(signalNumber);
			if (iter != m_subscribedsignals.end()) {
				return iter->second.processMeasuredData(data, len, m_dataCb);
			}
			return -1;
		}

		void SignalContainer::processMetaInformation(unsigned int signalNumber, const std::string& method, const Json::Value& params)
		{
			SubscribedSignal& signal = m_subscribedsignals[signalNumber];
			if (m_signalMetaCb) {
				m_signalMetaCb(signal, method, params);
			}
			signal.processSignalMetaInformation(method, params);

			if(method=="unsubscribe") {
				m_subscribedsignals.erase(signalNumber);
			}
		}

		void SignalContainer::clear()
		{
			m_subscribedsignals.clear();
		}
	}
}

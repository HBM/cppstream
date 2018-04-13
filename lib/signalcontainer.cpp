// Copyright 2014 Hottinger Baldwin Messtechnik
// Distributed under MIT license
// See file LICENSE provided

#include "signalcontainer.h"
#include "subscribedsignal.h"

namespace hbm {
	namespace streaming {
		SignalContainer::SignalContainer()
			: m_subscribedsignals()
			, m_signalMetaCb()
			, m_dataAsDoubleCb()
		{
		}

		void SignalContainer::setSignalMetaCb(SignalMetaCb_t cb)
		{
			m_signalMetaCb = cb;
		}

		void SignalContainer::setDataAsDoubleCb(DataAsDoubleCb_t cb)
		{
			m_dataAsDoubleCb = cb;
		}

		void SignalContainer::setDataAsRawCb(DataAsRawCb_t cb)
		{
			m_dataAsRawCb = cb;
		}

		ssize_t SignalContainer::processMeasuredData(unsigned int signalNumber, unsigned char* data, size_t len)
		{
			signals_t::iterator iter = m_subscribedsignals.find(signalNumber);
			if (iter != m_subscribedsignals.end()) {
				return iter->second.processMeasuredData(data, len, m_dataAsDoubleCb, m_dataAsRawCb);
			}
			return -1;
		}

		void SignalContainer::processMetaInformation(unsigned int signalNumber, const std::string& method, const Json::Value& params)
		{
			signals_t::iterator iter = m_subscribedsignals.find(signalNumber);

			if(method=="unsubscribe") {
				m_subscribedsignals.erase(signalNumber);
				return;
			}

			if (iter == m_subscribedsignals.end()) {
#ifdef _MSC_VER
				std::pair < signals_t::iterator, bool > result = m_subscribedsignals.insert(std::make_pair(signalNumber, SubscribedSignal(signalNumber)));
#else
				std::pair < signals_t::iterator, bool > result = m_subscribedsignals.emplace(signalNumber, SubscribedSignal(signalNumber));
#endif
				iter = result.first;
			}

			SubscribedSignal& signal = iter->second;
			signal.processSignalMetaInformation(method, params);
			if (m_signalMetaCb) {
				m_signalMetaCb(signal, method, params);
			}
		}

		void SignalContainer::clear()
		{
			m_subscribedsignals.clear();
		}
	}
}

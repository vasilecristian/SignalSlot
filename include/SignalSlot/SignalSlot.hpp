//--------------------------------------------------------------------------------
// SignalSlot.hpp
//
// Copyright (c) 2022, Cristian Vasile
// All rights reserved.
//
// Thanks to https://schneegans.github.io/tutorials/2015/09/20/signal-slot which is
// the original source of inspiration.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL CRISTIAN VASILE BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: Simon Schneegans modified by me (Cristian Vasile)
//--------------------------------------------------------------------------------
#ifndef SignalSlot_hpp
#define SignalSlot_hpp

#include <functional>
#include <map>

namespace cb
{
	typedef int SlotId;

	template <typename... Args>
	class Signal;

	template <typename... Args>
	class Slot
	{
		Signal<Args...>& m_signal;
		SlotId m_slotId;
	public:
		Slot(Signal<Args...>& signal, SlotId slotId): m_signal(signal), m_slotId(slotId){};
		virtual ~Slot() { m_signal.Disconnect(*this); }
		Slot& operator=(Slot const& other)
		{
			if (this != &other)
			{
				m_signal.Disconnect(*this);
				m_signal = other.m_signal;
				m_slotId = other.m_slotId;
			}
			return *this;
		}
		SlotId GetSlotId() const { return m_slotId; }
	};

	template <typename... Args>
	class Signal
	{
		friend class Slot<Args...>;
	public:
		Signal() = default;
		~Signal() = default;

		Signal(Signal const&) {}

		Signal& operator=(Signal const& other)
		{
			if (this != &other)
			{
				DisconnectAll();
			}
			return *this;
		}

		Signal(Signal&& other) noexcept :
			m_slots(std::move(other.m_slots)),
			m_currentId(other.m_currentId)
		{
		}

		Signal& operator=(Signal&& other) noexcept
		{
			if (this != &other)
			{
				m_slots = std::move(other.m_slots);
				m_currentId = other.m_currentId;
			}

			return *this;
		}


		Slot<Args...> Connect(std::function<void(Args...)> const& slotId)
		{
			m_slots.insert(std::make_pair(++m_currentId, slotId));
			return Slot<Args...>(*this, m_currentId);
		}

		template <typename T>
		Slot<Args...> Connect(T *inst, void (T::*func)(Args...))
		{
			return Connect([=](Args... args) { (inst->*func)(args...); });
		}

		template <typename T>
		Slot<Args...> Connect(T *inst, void (T::*func)(Args...) const)
		{
			return Connect([=](Args... args) { (inst->*func)(args...); });
		}

		void Disconnect(const Slot<Args...>& slot) const
		{
			m_slots.erase(slot.GetSlotId());
		}

		void DisconnectAll() const
		{
			m_slots.clear();
		}

		void Emit(Args... p)
		{
			for (auto const& it : m_slots)
			{
				it.second(p...);
			}
		}

		void EmitForAllButOne(const Slot<Args...>& slot, Args... p)
		{
			for (auto const& it : m_slots)
			{
				if (it.first != slot.GetSlotId())
				{
					it.second(p...);
				}
			}
		}

		void EmitFor(const Slot<Args...>& slot, Args... p)
		{
			auto const& it = m_slots.find(slot.GetSlotId());
			if (it != m_slots.end())
			{
				it->second(p...);
			}
		}

	private:
		mutable std::map<SlotId, std::function<void(Args...)> > m_slots;
		mutable int m_currentId{ 0 };
	};
}

#endif //SignalSlot_hpp

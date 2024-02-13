#pragma once

#include "vppch.h"

#include "Viper/Core/Core.h"

namespace Viper {


	// Events are blocking rather than buffered

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyTyped, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		JoystickConnected
	};

	// Use BIT() so an Event can be in multiple Categories
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication       = BIT(0),
		EventCategoryInput             = BIT(1),
		EventCategoryKeyboard          = BIT(2),
		EventCategoryMouse             = BIT(3),
		EventCategoryMouseButton       = BIT(4),
		EventCategoryJoystick		   = BIT(5)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; } \
							          virtual EventType GetEventType() const override { return GetStaticType(); } \
							          virtual const char* GetName() const override { return #type; }
								       
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class  Event
	{
		friend class EventDispatcher;

	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
		inline bool IsHandled() { return m_Handled; }

	protected:
		bool m_Handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>; // Return false event if already handled

	public:
		EventDispatcher(Event& event) : m_Event(event) {}

		template <typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	using EventCallbackFn = std::function<void(Event&)>;

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}
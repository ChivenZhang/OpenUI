#pragma
#include "Widget/IRmGUISignal.h"

class RmGUISignalPrivate {};
using RmGUISignalPrivateRaw = RmRaw<RmGUISignalPrivate>;

/// @brief implementation of signal with argments.
/// @tparam ...T 
template <class... T>
class RMGUI_API RmGUISignalAs : public IRmGUISignalAs<T...>
{
public:
	RmGUISignalAs();
	~RmGUISignalAs();
	virtual uint32_t connect(IRmGUIWidgetRaw owner, RmLambda<void(T...)> slot) override;
	virtual void disconnect(IRmGUIWidgetRaw owner, uint32_t handle) override;
	virtual void disconnect(IRmGUIWidgetRaw owner) override;
	virtual void emit(T... args) override;

private:
	RmGUISignalPrivateRaw m_Private;
};
template <class... T>
using RmGUISignalAsRaw = RmGUISignalAs<T...>;

template <class... T>
class RmGUISignalSlot
{
public:
	uint32_t Handle = 0;
	IRmGUIWidgetRaw Owner = nullptr;
	RmLambda<void(T...)> Slot;
};
template <class... T>
using RmGUISignalSlotRef = RmRef<RmGUISignalSlot<T...>>;

template <class... T>
class RmGUISignalPrivateData : public RmGUISignalPrivate
{
public:
	uint32_t ConnectID = 0;
	RmVector<RmGUISignalSlotRef<T...>> ConnectList;
};
#define PRIVATE_SIGNAL() ((RmGUISignalPrivateData<T...>*)m_Private)

template<class... T>
inline RmGUISignalAs<T...>::RmGUISignalAs()
	:
	m_Private(nullptr)
{
	m_Private = new RmGUISignalPrivateData<T...>;
}

template<class... T>
inline RmGUISignalAs<T...>::~RmGUISignalAs()
{
	delete m_Private; m_Private = nullptr;
}

template<class... T>
inline uint32_t RmGUISignalAs<T...>::connect(IRmGUIWidgetRaw owner, RmLambda<void(T...)> slot)
{
	auto handle = ++PRIVATE_SIGNAL()->ConnectID;
	auto element = RmNew<RmGUISignalSlot<T...>>();
	element->Handle = handle;
	element->Owner = owner;
	element->Slot = slot;
	PRIVATE_SIGNAL()->ConnectList.emplace_back(element);
	return handle;
}

template<class... T>
inline void RmGUISignalAs<T...>::disconnect(IRmGUIWidgetRaw owner, uint32_t handle)
{
	auto& connectList = PRIVATE_SIGNAL()->ConnectList;
	for (size_t i = 0; i < connectList.size(); ++i)
	{
		if (connectList[i]->Owner == owner && connectList[i]->Handle == handle)
		{
			connectList.erase(connectList.begin() + i);
			return;
		}
	}
}

template<class... T>
inline void RmGUISignalAs<T...>::disconnect(IRmGUIWidgetRaw owner)
{
	RmVector<RmGUISignalSlotRef<T...>> result;
	auto& connectList = PRIVATE_SIGNAL()->ConnectList;
	for (size_t i = 0; i < connectList.size(); ++i)
	{
		if (connectList[i]->Owner == owner) result.push_back(connectList[i]);
	}
	for (size_t i = 0; i < result.size(); ++i)
	{
		auto index = std::find(connectList.begin(), connectList.end(), result[i]);
		if (index == connectList.end()) continue;
		connectList.erase(index);
	}
}

template<class... T>
inline void RmGUISignalAs<T...>::emit(T... args)
{
	auto& connectList = PRIVATE_SIGNAL()->ConnectList;
	for (size_t i = 0; i < connectList.size(); ++i)
		if (connectList[i]->Slot) connectList[i]->Slot(std::forward<T>(args)...);
}

// ===================================================

template <>
class RMGUI_API RmGUISignalAs<void> : public IRmGUISignalAs<>
{
public:
	RmGUISignalAs();
	~RmGUISignalAs();
	virtual uint32_t connect(IRmGUIWidgetRaw owner, RmLambda<void()> slot) override;
	virtual void disconnect(IRmGUIWidgetRaw owner, uint32_t handle) override;
	virtual void disconnect(IRmGUIWidgetRaw owner) override;
	virtual void emit() override;

private:
	RmGUISignalPrivateRaw m_Private;
};

template <>
class RmGUISignalSlot<void>
{
public:
	uint32_t Handle = 0;
	IRmGUIWidgetRaw Owner = nullptr;
	RmLambda<void()> Slot;
};
using RmGUISignalSlotVoidRef = RmRef<RmGUISignalSlot<>>;

template <>
class RmGUISignalPrivateData<void> : public RmGUISignalPrivate
{
public:
	uint32_t ConnectID = 0;
	RmVector<RmGUISignalSlotVoidRef> ConnectList;
};
#define PRIVATE_SIGNAL_VOID() ((RmGUISignalPrivateData<>*)m_Private)

inline RmGUISignalAs<void>::RmGUISignalAs()
	:
	m_Private(nullptr)
{
	m_Private = new RmGUISignalPrivateData<>();
}

inline RmGUISignalAs<void>::~RmGUISignalAs()
{
	delete m_Private; m_Private = nullptr;
}

inline uint32_t RmGUISignalAs<void>::connect(IRmGUIWidgetRaw owner, RmLambda<void()> slot)
{
	auto handle = ++PRIVATE_SIGNAL_VOID()->ConnectID;
	auto element = RmNew<RmGUISignalSlot<>>();
	element->Handle = handle;
	element->Owner = owner;
	element->Slot = slot;
	PRIVATE_SIGNAL_VOID()->ConnectList.emplace_back(element);
	return handle;
}

inline void RmGUISignalAs<void>::disconnect(IRmGUIWidgetRaw owner, uint32_t handle)
{
	auto& connectList = PRIVATE_SIGNAL_VOID()->ConnectList;
	for (size_t i = 0; i < connectList.size(); ++i)
	{
		if (connectList[i]->Owner == owner && connectList[i]->Handle == handle)
		{
			connectList.erase(connectList.begin() + i);
			return;
		}
	}
}

inline void RmGUISignalAs<void>::disconnect(IRmGUIWidgetRaw owner)
{
	RmVector<RmGUISignalSlotVoidRef> result;
	auto& connectList = PRIVATE_SIGNAL_VOID()->ConnectList;
	for (size_t i = 0; i < connectList.size(); ++i)
	{
		if (connectList[i]->Owner == owner) result.push_back(connectList[i]);
	}
	for (size_t i = 0; i < result.size(); ++i)
	{
		auto index = std::find(connectList.begin(), connectList.end(), result[i]);
		if (index == connectList.end()) continue;
		connectList.erase(index);
	}
}

inline void RmGUISignalAs<void>::emit()
{
	auto& connectList = PRIVATE_SIGNAL_VOID()->ConnectList;
	for (size_t i = 0; i < connectList.size(); ++i)
		if (connectList[i]->Slot) connectList[i]->Slot();
}
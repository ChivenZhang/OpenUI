#pragma once
/*=================================================
* Copyright © 2020-2024 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
*=====================History========================
* Created by ChivenZhang at 1/14/2024 1:11:06 AM.
*
* =================================================*/

#include <shared_mutex>
#include <thread>

namespace std
{
	class shared_recursive_mutex : public shared_mutex
	{
	public:
		void lock() noexcept
		{
			if (owner != this_thread::get_id())
			{
				shared_mutex::lock();
				owner = this_thread::get_id();
			}
			count.fetch_add(1);
		}
		bool try_lock() noexcept
		{
			bool result = true;
			if (owner != this_thread::get_id())
			{
				result = shared_mutex::try_lock();
				if (result) owner = this_thread::get_id();
			}
			if (result) count.fetch_add(1);
			return result;
		}
		void unlock() noexcept
		{
			if (count.fetch_sub(1) == 1)
			{
				owner = thread::id();
				shared_mutex::unlock();
			}
		}
		void lock_shared() noexcept
		{
			if (owner != this_thread::get_id()) shared_mutex::lock_shared();
		}
		bool try_lock_shared() noexcept
		{
			bool result = true;
			if (owner != this_thread::get_id()) result = shared_mutex::try_lock_shared();
			return result;
		}
		void unlock_shared() noexcept
		{
			if (owner != this_thread::get_id()) shared_mutex::unlock_shared();
		}
	private:
		atomic<thread::id> owner;
		atomic_int32_t count = 0;
	};
}
#include <array>
#include <gtest/gtest.h>

#include "FixedSizeAllocator.h"
#include "MemoryOverrides.h"

#if __GNUC__
// ignoring some gcc warnings because we are deliberately messing with memory
#pragma GCC diagnostic ignored "-Wdelete-incomplete"
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
#endif

namespace dae
{
	constexpr size_t NR_BLOCKS = 50;

	class Object
	{
	public:
		int m_integer{ 0 };
		float m_float{ 0 };
	};

	TEST(FixedSizeAllocatorTests, SingleAllocation)
	{
		FixedSizeAllocator<Object> allocator(NR_BLOCKS);
		Object* objectPtr{ new (allocator) Object{1, 0.6f} };
		delete objectPtr;
	}

	TEST(FixedSizeAllocatorTests, NewDelete)
	{
		FixedSizeAllocator<Object> allocator(NR_BLOCKS);
	
		Object* pointer = new (allocator) Object();
		EXPECT_NE(pointer, nullptr);
		std::memset(pointer, 1, sizeof(*pointer));
		delete pointer;
	}

	TEST(FixedSizeAllocatorTests, InvalidRelease)
	{
		FixedSizeAllocator<Object> allocator(NR_BLOCKS);
	
		void* pointer = new char;
		EXPECT_THROW(allocator.Release(pointer), std::runtime_error);
		delete pointer;
	}

	TEST(FixedSizeAllocatorTests, TwoAllocations)
	{
		FixedSizeAllocator<Object> allocator(NR_BLOCKS);
		
		constexpr size_t objectSize{ sizeof(Object) };
		constexpr size_t allocateSize{ objectSize + sizeof(MemoryAllocator::Tag) };
	
		void* pointer_a{};
		void* pointer_b{};
		pointer_a = allocator.Acquire(allocateSize);
		pointer_b = allocator.Acquire(allocateSize);
		EXPECT_NE(pointer_a, nullptr);
		EXPECT_NE(pointer_b, nullptr);
		std::memset(pointer_a, 1, objectSize);
		std::memset(pointer_b, 1, objectSize);
		allocator.Release(pointer_a);
		allocator.Release(pointer_b);
	}

	TEST(FixedSizeAllocatorTests, FullAllocation)
	{
		FixedSizeAllocator<Object> allocator(NR_BLOCKS);
		std::array<void*, NR_BLOCKS> objects{};
		
		for (void*& object : objects)
			object = allocator.Acquire(sizeof(Object) + sizeof(MemoryAllocator::Tag));
		for (void* object : objects)
			allocator.Release(object);
	}

	TEST(FixedSizeAllocatorTests, FullAllocationDeleteNew)
	{
		FixedSizeAllocator<Object> allocator(NR_BLOCKS);
		std::array<Object*, NR_BLOCKS> objects{};
	
		for (auto& object : objects)
		{
			object = new (allocator) Object{ 10, 0 };
			EXPECT_NE(object, nullptr);
		}
	
		for (auto& object : objects)
		{
			delete object;
			object = nullptr;
		}
	}

	TEST(FixedSizeAllocatorTests, OutOfMemory)
	{
		FixedSizeAllocator<Object> allocator(NR_BLOCKS);
		std::array<Object*, NR_BLOCKS> objects{};

		for (auto& object : objects)
		{
			object = new (allocator) Object{ 10, 0 };
			EXPECT_NE(object, nullptr);
		}

		EXPECT_THROW(new (allocator) Object{}, std::bad_alloc);
#
		for (auto& object : objects)
		{
			delete object;
			object = nullptr;
		}
	}

	TEST(FixedSizeAllocatorTests, Fragmentionless)
	{
		FixedSizeAllocator<Object> allocator(NR_BLOCKS);
		std::array<Object*, NR_BLOCKS> objects{};

		for (auto& object : objects)
		{
			object = new (allocator) Object{ 10, 0 };
			EXPECT_NE(object, nullptr);
		}

		for (size_t idx{}; idx < objects.size(); idx += 2)
		{
			delete objects[idx];
			objects[idx] = nullptr;
		}

		for (size_t idx{}; idx < objects.size(); idx += 2)
		{
			objects[idx] = new (allocator) Object{10, 0};
			EXPECT_NE(objects[idx], nullptr);
		}
	}
}

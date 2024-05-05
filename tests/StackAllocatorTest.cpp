#include <algorithm>
#include <array>
#include <gtest/gtest.h>

#include "FixedSizeAllocator.h"
#include "StackAllocator.h"
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
	class Object
	{
	public:
		int m_integer{ 0 };
		float m_float{ 0 };
	};

	constexpr size_t NR_OBJECTS{ 4 };
	constexpr size_t OBJECT_SIZE{ sizeof(Object) };
	constexpr size_t NR_BYTES{ OBJECT_SIZE * NR_OBJECTS };

	TEST(StackAllocatorTests, SingleAllocation)
	{
		StackAllocator allocator(NR_BYTES);
		void* pointer{ allocator.Acquire(OBJECT_SIZE) };
		EXPECT_NE(pointer, nullptr);
		allocator.Release(pointer);
	}

	TEST(StackAllocatorTests, NewDelete)
	{
		StackAllocator allocator(NR_BYTES);
	
		Object* pointer = new (allocator) Object();
		EXPECT_NE(pointer, nullptr);
		std::memset(pointer, 1, sizeof(*pointer));
		delete pointer;
	}

	
	TEST(StackAllocatorTests, InvalidRelease)
	{
		StackAllocator allocator(NR_BYTES);

		void* pointer = new char;
		EXPECT_THROW(allocator.Release(pointer), std::runtime_error);
		delete pointer;
	}

	TEST(StackAllocatorTests, TwoAllocations)
	{
		StackAllocator allocator(NR_BYTES);
	
		void* pointer_a{};
		void* pointer_b{};
		pointer_a = allocator.Acquire(OBJECT_SIZE);
		pointer_b = allocator.Acquire(OBJECT_SIZE);
		EXPECT_NE(pointer_a, nullptr);
		EXPECT_NE(pointer_b, nullptr);
		std::memset(pointer_a, 1, OBJECT_SIZE);
		std::memset(pointer_b, 1, OBJECT_SIZE);
		allocator.Release(pointer_a);
		allocator.Release(pointer_b);
	}

	
	TEST(StackAllocatorTests, FullAllocation)
	{
		StackAllocator allocator(NR_BYTES);
		std::array<void*, NR_OBJECTS> objects{};
		
		for (void*& object : objects)
			object = allocator.Acquire(OBJECT_SIZE);
		for (void* object : objects)
			allocator.Release(object);
	}

	
	TEST(StackAllocatorTests, FullAllocationDeleteNew)
	{
		StackAllocator allocator(NR_BYTES + sizeof(MemoryAllocator::Tag) * NR_OBJECTS);
		std::array<Object*, NR_OBJECTS> objects{};
	
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

	TEST(StackAllocatorTests, OverAllocate)
	{
		StackAllocator allocator(NR_BYTES + sizeof(MemoryAllocator::Tag) * NR_OBJECTS);
		std::array<Object*, NR_OBJECTS> objects{};

		for (auto& object : objects)
		{
			object = new (allocator) Object{ 10, 0 };
			EXPECT_NE(object, nullptr);
		}

		EXPECT_THROW(new (allocator) Object{}, std::bad_alloc);

		for (auto& object : objects)
		{
			delete object;
			object = nullptr;
		}
	}

	
	TEST(StackAllocatorTests, FullRelease)
	{
		StackAllocator allocator(NR_BYTES + sizeof(MemoryAllocator::Tag) * NR_OBJECTS);
		std::array<Object*, NR_OBJECTS> objects{};

		for (auto& object : objects)
		{
			object = new (allocator) Object{ 10, 0 };
			EXPECT_NE(object, nullptr);
		}

		allocator.FullRelease();
	}

	
	TEST(StackAllocatorTests, MarkerRelease)
	{
		StackAllocator allocator(NR_BYTES + sizeof(MemoryAllocator::Tag) * NR_OBJECTS);
		std::array<Object*, NR_OBJECTS> objects{};
		constexpr size_t halfCount{ NR_OBJECTS / 2 };

		std::for_each_n(objects.begin(), halfCount, [&allocator](Object*& objectPtr)
			{
				objectPtr = new (allocator) Object{ 10, 0 };
				EXPECT_NE(objectPtr, nullptr);
			});

		allocator.SetMarker();


		std::for_each_n(objects.begin() + halfCount, halfCount, [&allocator](Object*& objectPtr)
			{
				objectPtr = new (allocator) Object{ 10, 0 };
				EXPECT_NE(objectPtr, nullptr);
			});

		allocator.ReleaseTillMarker();


		// Reallocate after marker release
		std::for_each_n(objects.begin() + halfCount, halfCount, [&allocator](Object*& objectPtr)
			{
				objectPtr = new (allocator) Object{ 10, 0 };
				EXPECT_NE(objectPtr, nullptr);
			});

		allocator.FullRelease();
	}
}

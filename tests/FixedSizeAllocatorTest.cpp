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

	//TEST(FixedSizeAllocatorTests, SingleAllocation)
	//{
	//	FixedSizeAllocator<Object> allocator(NR_BLOCKS);
	//	Object* objectPtr{ new (allocator) Object{1, 0.6f} };
	//	delete objectPtr;
	//}

	//TEST(FixedSizeAllocatorTests, NewDelete)
	//{
	//	FixedSizeAllocator<Object> allocator(NR_BLOCKS);
	//
	//	Object* pointer = new (allocator) Object();
	//	EXPECT_NE(pointer, nullptr);
	//	std::memset(pointer, 1, sizeof(*pointer));
	//	delete pointer;
	//}

	//TEST(FixedSizeAllocatorTests, InvalidRelease)
	//{
	//	FixedSizeAllocator<Object> allocator(NR_BLOCKS);
	//
	//	void* pointer = new char;
	//	EXPECT_THROW(allocator.Release(pointer), std::runtime_error);
	//	delete pointer;
	//}

	//TEST(FixedSizeAllocatorTests, TwoAllocations)
	//{
	//	FixedSizeAllocator<Object> allocator(NR_BLOCKS);
	//	
	//	constexpr size_t objectSize{ sizeof(Object) };
	//	constexpr size_t allocateSize{ objectSize + sizeof(MemoryAllocator::Tag) };
	//
	//	void* pointer_a{};
	//	void* pointer_b{};
	//	pointer_a = allocator.Acquire(allocateSize);
	//	pointer_b = allocator.Acquire(allocateSize);
	//	EXPECT_NE(pointer_a, nullptr);
	//	EXPECT_NE(pointer_b, nullptr);
	//	std::memset(pointer_a, 1, objectSize);
	//	std::memset(pointer_b, 1, objectSize);
	//	allocator.Release(pointer_a);
	//	allocator.Release(pointer_b);
	//}

	TEST(FixedSizeAllocatorTests, FullAllocation)
	{
		//FixedSizeAllocator<Object> allocator(NR_BLOCKS);
		//std::array<Object*, NR_BLOCKS> objects{};
		//for (auto& object : objects)
		//	object = new (allocator) Object{};
		//std::fill(objects.begin(), objects.end(), new (allocator) Object{});

		//for (Object* object : objects)
		//	delete object;

		//const size_t nbPieces = allocator_size / block_size;
		//void** pointers = new void* [nbPieces];
		//const size_t test_size = allocator_size / nbPieces - pointer_size;
		//for (unsigned int i = 0; i < nbPieces; i++)
		//{
		//	pointers[i] = allocator.Acquire(test_size);
		//	EXPECT_NE(pointers[i], nullptr);
		//	std::memset(pointers[i], 1, test_size);
		//}
		//for (unsigned int i = 0; i < nbPieces; i++)
		//	allocator.Release(pointers[i]);
		//delete[] pointers;
	}

	//TEST(SingleLinkAllocatorTests, OutOfMemory)
	//{
	//	SingleLinkAllocator allocator(allocator_size);
	//	const size_t test_size = allocator_size - pointer_size;
	//	void* pointer;
	//	pointer = allocator.Acquire(test_size);
	//	EXPECT_NE(pointer, nullptr);
	//	std::memset(pointer, 1, test_size);
	//	EXPECT_THROW(allocator.Acquire(4), std::bad_alloc);
	//	allocator.Release(pointer);
	//}
}

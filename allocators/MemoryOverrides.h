#pragma once
#include <stddef.h> // for size_t

// overloads global new and delete.

namespace dae {
	class MemoryAllocator;
}

void * operator new (size_t nbBytes);

void * operator new[] (size_t nbBytes);

void * operator new (size_t nbBytes, dae::MemoryAllocator& storagePool);

void * operator new[] (size_t nbBytes, dae::MemoryAllocator& storagePool);

void operator delete (void* pointerToBuffer) noexcept;

void operator delete[] (void* pointerToBuffer) noexcept;

void operator delete (void* pointerToBuffer, long unsigned int) noexcept;

void operator delete[] (void* pointerToBuffer, long unsigned int) noexcept;

void operator delete (void* pointerToBuffer, dae::MemoryAllocator &storagePool) noexcept;


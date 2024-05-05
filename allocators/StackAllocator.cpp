#include "StackAllocator.h"

#include <new>
#include <stdexcept>

dae::StackAllocator::StackAllocator(size_t nrBytes)
	: m_PoolSize{ nrBytes }
	, m_Pool{ new std::byte[nrBytes]{} }
{
}

dae::StackAllocator::~StackAllocator()
{
	delete[] m_Pool;
}

void* dae::StackAllocator::Acquire(size_t nbBytes)
{
	if (m_HeadByte + nbBytes >= m_PoolSize)
		throw std::bad_alloc();

	const size_t current{ m_HeadByte };
	m_HeadByte += nbBytes;

	return &m_Pool[current];
}

void dae::StackAllocator::Release(void* pointer)
{
	const auto* objectPtr{ reinterpret_cast<std::byte*>(pointer) };
	const ptrdiff_t ptrDiff{ objectPtr - m_Pool };

	if (ptrDiff > static_cast<ptrdiff_t>(m_PoolSize) || ptrDiff < 0)
		throw std::runtime_error("Pointer to invalid object");

	const size_t distance{ static_cast<size_t>(ptrDiff) };
	if (m_MarkerByte > distance)
		m_MarkerByte = distance;

	m_HeadByte = distance;
}

void dae::StackAllocator::ReleaseTillMarker()
{
	m_HeadByte = m_MarkerByte;
	m_MarkerByte = 0;
}

void dae::StackAllocator::FullRelease()
{
	m_HeadByte = 0;
	m_MarkerByte = 0;
}

void dae::StackAllocator::SetMarker()
{
	m_MarkerByte = m_HeadByte;
}

size_t dae::StackAllocator::GetPtrDist(void* pointer) const
{
	const auto* objectPtr{ reinterpret_cast<std::byte*>(pointer) };
	const ptrdiff_t dist{ objectPtr - m_Pool };

	if (dist > static_cast<ptrdiff_t>(m_PoolSize) || dist < 0)
		throw std::runtime_error("Pointer to invalid object");

	return static_cast<size_t>(dist);
}
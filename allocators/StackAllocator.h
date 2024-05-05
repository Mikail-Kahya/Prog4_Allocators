#pragma once
#include <cstddef>

#include "MemoryAllocator.h"

namespace dae
{
	/// <summary>
	/// </summary>

	class StackAllocator final : public MemoryAllocator
	{
	public:
		explicit StackAllocator(size_t nrBytes);
		~StackAllocator() override;
		StackAllocator(const StackAllocator&)					= delete;
		StackAllocator(StackAllocator&&) noexcept				= delete;
		StackAllocator& operator= (const StackAllocator&)		= delete;
		StackAllocator& operator= (StackAllocator&&) noexcept	= delete;

		void* Acquire(size_t nbBytes) override;
		void Release(void* pointer) override;
		void ReleaseTillMarker();
		void FullRelease();

		void SetMarker();

	private:
		size_t GetPtrDist(void* pointer) const;

		size_t m_PoolSize{};
		std::byte* m_Pool{ nullptr };

		size_t m_HeadByte{};
		size_t m_MarkerByte{};
	};
}
#pragma once
#include "MemoryAllocator.h"
#include <vector>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <typeinfo>

namespace dae
{
	/// <summary>
	/// </summary>

	template<typename Object>
	class FixedSizeAllocator final : public MemoryAllocator
	{
		struct Block
		{
			size_t nextIdx{};
			Object object;
		};

		using Pool = std::vector<Block>;

	public:
		explicit FixedSizeAllocator(size_t nrBlocks)
			: m_Pool{ Pool(nrBlocks) }
		{
		}
		~FixedSizeAllocator() override = default;

		void* Acquire(size_t nbBytes) override;

		void Release(void* pointer) override;

		FixedSizeAllocator(const FixedSizeAllocator&)					= delete;
		FixedSizeAllocator(FixedSizeAllocator&&) noexcept				= delete;
		FixedSizeAllocator& operator= (const FixedSizeAllocator&)		= delete;
		FixedSizeAllocator& operator= (FixedSizeAllocator&&) noexcept	= delete;

	private:
		bool OverridingOldBlock() const { return m_HeadIdx != m_LastIdx; }

		Pool m_Pool;

		size_t m_HeadIdx{};
		size_t m_LastIdx{}; // suggestion by Julian Rijken to keep track of latest idx
	};

	template <typename Object>
	void* FixedSizeAllocator<Object>::Acquire(size_t nbBytes)
	{
		constexpr size_t checkSize{ sizeof(Object) + sizeof(MemoryAllocator::Tag) };
		const bool isSameSize{ nbBytes == checkSize };

		assert(isSameSize && "FixedSizeAllocated without proper size");
		if (!isSameSize)
			throw std::bad_alloc();

		if (m_Pool.size() <= m_HeadIdx)
			throw std::bad_alloc();

		const size_t current{ m_HeadIdx };

		if (OverridingOldBlock())
			m_LastIdx = m_HeadIdx++;
		else
			m_HeadIdx = m_Pool[m_HeadIdx].nextIdx; // Acts like an instruction pointer
		
		return reinterpret_cast<void*>(&m_Pool[current]);
	}

	template <typename Object>
	void FixedSizeAllocator<Object>::Release(void* pointer)
	{
		//auto poolPtr{ m_Pool.data() };
		const auto* firstData{ &m_Pool[0].object };
		const auto* lastData{ &m_Pool[m_Pool.size() - 1].object };
		const auto* objectPtr{ reinterpret_cast<Object*>(pointer) };

		const ptrdiff_t maxDist{ lastData - firstData - 1 };
		const ptrdiff_t dist{ firstData -  objectPtr - 1 };

		if (dist > maxDist)
			throw std::runtime_error("Pointer to invalid object");

		m_Pool[dist].nextIdx = m_HeadIdx;
		m_HeadIdx = static_cast<size_t>(dist);
	}
}

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
		union Block
		{
			size_t nextIdx{};
			Object object;
		};

	public:
		explicit FixedSizeAllocator(size_t nrBlocks)
			: m_PoolSize{ nrBlocks }
			, m_Pool{ new Block[nrBlocks + 1]{} } // Avoid heap corruption
		{
		}
		~FixedSizeAllocator() override
		{
			delete[] m_Pool;
			m_Pool = nullptr;
		}
		FixedSizeAllocator(const FixedSizeAllocator&)					= delete;
		FixedSizeAllocator(FixedSizeAllocator&&) noexcept				= delete;
		FixedSizeAllocator& operator= (const FixedSizeAllocator&)		= delete;
		FixedSizeAllocator& operator= (FixedSizeAllocator&&) noexcept	= delete;

		void* Acquire(size_t nbBytes) override;
		void Release(void* pointer) override;

	private:
		bool OverridingOldBlock() const { return m_HeadIdx != m_LastIdx; }

		size_t m_PoolSize{};
		Block* m_Pool;

		size_t m_HeadIdx{};
		size_t m_LastIdx{}; // suggestion by Julian Rijken to keep track of latest idx
	};

	template <typename Object>
	void* FixedSizeAllocator<Object>::Acquire(size_t nbBytes)
	{
		constexpr size_t checkSize{ sizeof(Object) + sizeof(MemoryAllocator::Tag) };
		const bool isSameSize{ nbBytes == checkSize };

		if (!isSameSize)
			throw std::bad_alloc();

		const size_t current{ m_HeadIdx };

		if (OverridingOldBlock())
			m_HeadIdx = m_Pool[m_HeadIdx].nextIdx; // Acts like an instruction pointer
		else
		{
			if (m_PoolSize <= m_LastIdx)
				throw std::bad_alloc();
			m_LastIdx = ++m_HeadIdx;
		}
			
		
		return reinterpret_cast<void*>(&m_Pool[current]);
	}

	template <typename Object>
	void FixedSizeAllocator<Object>::Release(void* pointer)
	{
		const auto* objectPtr{ reinterpret_cast<Block*>(pointer) };
		const ptrdiff_t dist{ objectPtr - m_Pool };
		
		if (dist > static_cast<ptrdiff_t>(m_PoolSize) || dist < 0)
			throw std::runtime_error("Pointer to invalid object");
		
		m_Pool[dist].nextIdx = m_HeadIdx;
		m_HeadIdx = static_cast<size_t>(dist);
	}
}

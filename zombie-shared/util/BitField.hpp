#pragma once

#include <vector>

#include "util/IntTypes.hpp"
#include "util/Math.hpp"

class BitField {
public:
	using BlockType = u64;

	BitField();
	~BitField();

	void Resize(u64 count);
	bool Get(u64 bit_index) const;
	void Set(u64 bit_index, bool value);
	u64 GetSize() const;
	u64 GetNumBlocks() const;

	constexpr static u64 GetNumBitsPerBlock() {
		return sizeof(BlockType) * 8;
	}

	void Set(u64 block_index, u64 block_local_index, bool value);

	BlockType GetBlock(std::size_t block_index) const;

protected:
	std::vector<BlockType> m_bitfield;
};

template<class TBlockType, std::size_t c_num_blocks>
class StaticBitfield {
public:
	StaticBitfield() {
		for (auto& block : m_bitfield) {
			block = 0;
		}
	}

	bool Get(std::size_t bit_index) const {
		auto block_index = bit_index / GetBitsPerBlock();
		auto data_index = bit_index & (GetBitsPerBlock() - 1);
		return (m_bitfield[block_index] & (1llu << data_index)) != 0;
	}

	void Set(std::size_t bit_index, bool value) {
		auto vector_index = bit_index / GetBitsPerBlock();
		auto data_index = bit_index & (GetBitsPerBlock() - 1);

		auto& slot = m_bitfield[vector_index];
		if (value) {
			slot |= 1llu << data_index;
		} else {
			slot &= ~(1llu << data_index);
		}
	}

	void Clear() {
		for (auto& block : m_bitfield) {
			block = 0;
		}
	}

	TBlockType GetBlock(std::size_t block_index) {
		return m_bitfield[block_index];
	}

	constexpr static std::size_t GetNumBlocks() {
		return c_num_blocks;
	}

	constexpr static std::size_t GetNumBits() {
		return c_num_blocks * GetBitsPerBlock();
	}

	constexpr static std::size_t GetBitsPerBlock() {
		return sizeof(TBlockType) * 8;
	}

private:
	std::array<TBlockType, c_num_blocks> m_bitfield;
};
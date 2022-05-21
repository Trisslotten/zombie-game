#include "BitField.hpp"

constexpr u64 c_num_data_bits = sizeof(BitField::BlockType) * 8;

BitField::BitField() {
}

BitField::~BitField() {
}

void BitField::Resize(u64 count) {
	auto previous_size = m_bitfield.size();
	u64 num_needed = DivRoundUp(count, c_num_data_bits);
	m_bitfield.resize(num_needed);

	for (u64 i = previous_size; i < m_bitfield.size(); ++i) {
		m_bitfield[i] = 0;
	}
}

bool BitField::Get(u64 bit_index) const {
	auto vector_index = bit_index / c_num_data_bits;
	auto data_index = bit_index & (c_num_data_bits - 1);
	return (m_bitfield[vector_index] & (1llu << data_index)) != 0;
}

void BitField::Set(u64 bit_index, bool value) {
	auto vector_index = bit_index / c_num_data_bits;
	auto data_index = bit_index & (c_num_data_bits - 1);

	auto& slot = m_bitfield[vector_index];
	if (value) {
		slot |= 1llu << data_index;
	} else {
		slot &= ~(1llu << data_index);
	}
}

u64 BitField::GetSize() const {
	return m_bitfield.size() * c_num_data_bits;
}

u64 BitField::GetNumBlocks() const {
	return m_bitfield.size();
}

void BitField::Set(u64 block_index, u64 block_local_index, bool value) {
}

BitField::BlockType BitField::GetBlock(std::size_t block_index) const {
    return m_bitfield[block_index];
}

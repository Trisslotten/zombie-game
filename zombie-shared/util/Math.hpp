#pragma once

template <class T>
T DivRoundUp(T x, T y) {
	return ((x + y) - 1) / y;
}

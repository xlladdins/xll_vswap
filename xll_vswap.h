#pragma once
#include <cmath>
#include <span>
#include "xll24/include/xll.h"

namespace fms::vswap {

	template<class X = double>
	constexpr X realized_return_payoff(std::span<X> x)
	{
		X s2 = 0;

		for (size_t i = 0; i < x.size() - 1; ++i) {
			X dx_x = (x[i + 1] - x[i]) / x[i];
			s2 += dx_x * dx_x;
		}

		return s2;
	}
	template<class X = double>
	constexpr X log_return_payoff(std::span<X> x)
	{
		X s2 = 0;

		for (size_t i = 0; i < x.size() - 1; ++i) {
			X dx_x = std::log(x[i + 1] / x[i]);
			s2 += dx_x * dx_x;
		}

		return s2;
	}


} // namespace fms
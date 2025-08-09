// fms_vswap.h - Variance swap 
#pragma once
#include <cmath>
#include <array>
#include <initializer_list>
#include <span>
#include <vector>

namespace fms::vswap {

	template<class X = double>
	constexpr X realized_return(std::span<const X> x)
	{
		X s2 = 0;

		for (size_t i = 0; i < x.size() - 1; ++i) {
			X dx_x = (x[i + 1] - x[i]) / x[i];
			s2 += dx_x * dx_x;
		}

		return s2;
	}
#ifdef _DEBUG
	namespace hide {
		constexpr const double x[] = {1., 2., 3.};
		static_assert(realized_return(std::span(x, 3)) == 1.25);
	}
#endif // _DEBUG
	template<class X = double>
	inline X log_return_payoff(std::span<const X> x)
	{
		X s2 = 0;

		for (size_t i = 0; i < x.size() - 1; ++i) {
			X dx_x = std::log(x[i + 1] / x[i]);
			s2 += dx_x * dx_x;
		}

		return s2;
	}

	// Intitial European variance swap payoff
	template<class X = double>
	inline X static_payoff(X x, X x0, X z)
	{
		return - 2 * std::log(x / x0) + 2 * (x - x0) / z;
	}

	// Number of futures to hold over (t_i, t_{j+1}]
	template<class X = double>
	constexpr X dynamic_payoff(X x, X z)
	{
		return 2 / x - 2 / z;
	}

	// Carr-Madan: f(x) = f(z) + f'(z)(x - z) + int_0^z f''(k) p(k) dk + int_z^\infty f''(k) c(k) dk
	// where p(x) = max(0, z - x) and c(x) = max(0, x - z).
	// Fit piecewise linear function with nodes (k_i, payoff(k_i)).
	// (f(z), f''(k[1]), ..., f''(k[n - 2]), f'(z))
	template<class X = double>
	constexpr void carr_madan(std::span<const X> k, std::span<X> f, X z)
	{
		X fz, f_z; // f(z) and f'(z)
		// f' working backwards
		for (size_t i = k.size() - 1; i > 0; --i) {
			f[i] = (f[i] - f[i - 1]) / (k[i] - k[i - 1]);
			if (k[i - 1] <= z && z < k[i]) {
				// Interpolate at z
				f_z = f[i];
				fz = f[i - 1] + f_z * (z - k[i - 1]);
			}
		}
		// f'' working forwards
		for (size_t i = 1; i < k.size() - 1; ++i) {
			f[i] = f[i + 1] - f[i];
		}
		f.front() = fz;
		f.back() = f_z;
	}
//#if 0
#ifdef _DEBUG
	template<size_t N, class X = double>
	inline bool carr_madan_test()
	{
		std::span<const double> k{ 1, 2, 3 };
		std::span<double> v{ 1, 2, 3 };
		carr_madan(k, v, 1.5);

		return v[0] == 0.5 && v[1] == -1.5 && v[2] == -1.;
	}
#endif // _DEBUG
	// Evaluate Carr-Madan formula at x
	template<class X = double>
	constexpr double carr_madan_eval(X x, std::span<const X> k, std::span<const X> f, X z)
	{
		double fx = f.front() + f.back() * (x - z);

		for (size_t i = 1; i < k.size() - 1; ++i) {
			if (k[i] <= z) {
				fx += f[i] * std::max(k[i] - x, 0.);
			}
			else {
				fx += f[i] * std::max(x - k[i], 0.);
			}
		}

		return fx;
	}


} // namespace fms
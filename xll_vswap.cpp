#include "fms_vswap.h"
#include "xll_vswap.h"

using namespace fms::vswap;
using namespace xll;

Auto<Open> xll_open([]() {
	try {
		//ensure_xll();
		// register functions here
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
		return false;
	}
	return true;
});

AddIn xai_static_payoff(
	Function(XLL_DOUBLE, L"?xll_static_payoff", CATEGORY L".STATIC_PAYOFF")
	.Arguments({
		Arg(XLL_DOUBLE, L"x", L"is the value at which to evaluate the payoff."),
		Arg(XLL_DOUBLE,  L"x0", L"is the initial price of the underlying."),
		Arg(XLL_DOUBLE, L"z", L"put-call separator."),
		})
	.Category(CATEGORY)
	.FunctionHelp(L"Initial variance swap payoff.")
);	
double WINAPI xll_static_payoff(double x, double x0, double z)
{
#pragma XLLEXPORT
	return static_payoff(x, x0, z);
}

AddIn xai_carr_madan(
	Function(XLL_FP, L"?xll_carr_madan", CATEGORY L".CARR_MADAN")
	.Arguments({
		Arg(XLL_FP, L"k", L"is the value at which to evaluate the payoff."),
		Arg(XLL_FP,  L"f", L"is the initial price of the underlying."),
		Arg(XLL_DOUBLE, L"z", L"put-call separator."),
		})
	.Category(CATEGORY)
	.FunctionHelp(L"Fit piecewise linear function to cash, puts, calls, and forward.")
);
_FP12* WINAPI xll_carr_madan(const _FP12* pk, _FP12* pf, double z)
{
#pragma XLLEXPORT
	try {
		ensure(pk && pf);
		carr_madan(xll::span(*pk), xll::span(*pf), z);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
		return nullptr;
	}

	return pf;
}

AddIn xai_carr_madan_eval(
	Function(XLL_DOUBLE, L"?xll_carr_madan_eval", CATEGORY L".CARR_MADAN_EVAL")
	.Arguments({
		Arg(XLL_DOUBLE, L"x", L"is the value at which to evaluate the function."),
		Arg(XLL_FP, L"k", L"is an array of strikes."),
		Arg(XLL_FP,  L"f", L"is the Carr-Madan coefficients."),
		Arg(XLL_DOUBLE, L"z", L"put-call separator."),
		})
		.Category(CATEGORY)
	.FunctionHelp(L"Return cash + sum of puts + sum of calls + forward.")
);
double WINAPI xll_carr_madan_eval(double x, const _FP12* pk, const _FP12* pf, double z)
{
#pragma XLLEXPORT
	double result = std::numeric_limits<double>::quiet_NaN();
	try {
		ensure(pk && pf);
		result = carr_madan_eval(x, xll::span(*pk), xll::span(*pf), z);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());
		return std::numeric_limits<double>::quiet_NaN();
	}

	return result;
}
#pragma once
#include <cstdint>
namespace FreeWear::Policy {
constexpr bool WaitForEarlier(std::int32_t own,std::int32_t other,bool finished){return other>0 && other<own && !finished;}
constexpr bool ContinuePastFailedEquip(bool renderedWorn,bool equipFinished,bool equipOK){return !renderedWorn && equipFinished && !equipOK;}
}

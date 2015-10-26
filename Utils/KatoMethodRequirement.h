#ifdef UTILS_EXPORTS
#define KATOMETHODREQUIREMENT_API __declspec(dllexport)
#else
#define KATOMETHODREQUIREMENT_API __declspec(dllimport)
#endif

#pragma once
#include "BasicRequirement.h"

namespace Requirement {

	///<summary>
	/// ������@�ɕK�v��Requirement
	///</summary>
	class KATOMETHODREQUIREMENT_API KatoMethodRequirement : public Requirement::BasicRequirement
	{
	public:
		time_t max_pause_time;
		time_t min_pause_time;

		time_t max_variation_of_pause_time;
		double max_variation_of_speed;

		int cycle_of_anonymous_area = 3;
		
		KatoMethodRequirement();
		KatoMethodRequirement(double max_pause_time, double min_pause_time);
		virtual ~KatoMethodRequirement();
	};
}
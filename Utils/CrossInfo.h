#ifdef UTILS_EXPORTS
#define CROSS_INFO_API __declspec(dllexport)
#else
#define CROSS_INFO_API __declspec(dllimport)
#endif
#pragma once
#include "MobileEntity.h"

namespace Evaluate
{

	///<summary>
	/// ���������i�[����N���X
	///</summary>
	struct CROSS_INFO_API CrossInfo
	{
	public:
		int phase;
		std::vector<Entity::entity_id> crossing_entities;

		CrossInfo();
		CrossInfo(int phase, const std::vector<Entity::entity_id>& crossing_entities);
		~CrossInfo();
	};
}



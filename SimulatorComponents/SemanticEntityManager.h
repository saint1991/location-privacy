#ifdef SIMULATORCOMPONENTS_EXPORTS
#define SEMANTIC_ENTITY_MANAGER_API __declspec(dllexport)
#else
#define SEMANTIC_ENTITY_MANAGER_API __declspec(dllimport)
#endif

#pragma once
#include "TimeSlotManager.h"
#include "Coordinate.h"
#include "LatLng.h"
#include "BasicUser.h"
#include "Dummy.h"
#include "EntityManager.h"


namespace Entity
{


	///<summary>
	/// ユーザ，ダミーを管理するクラス
	///</summary>
	template <typename POSITION_TYPE = Geography::LatLng>
	class SEMANTIC_ENTITY_MANAGER_API SemanticEntityManager : public Entity::EntityManager<Dummy<POSITION_TYPE>, User::BasicUser<POSITION_TYPE>, POSITION_TYPE>
	{
	public:
		SemanticEntityManager(std::shared_ptr<User::BasicUser<POSITION_TYPE>> user, int num_of_dummy, std::shared_ptr<Time::TimeSlotManager> timeslot);
		~SemanticEntityManager();

		void set_category_of_phase(entity_id id, const category_id& category, int phase);
		void set_category_at(entity_id id, const category_id& category, time_t time);

		void set_state_of_phase(entity_id id, const POSITION_TYPE& position, const category_id& category, int phase);
		void set_state_at(entity_id id, const POSITION_TYPE& position, const category_id& category, time_t time);
	};

	template class SemanticEntityManager<Graph::Coordinate>;
	template class SemanticEntityManager<Geography::LatLng>;
}



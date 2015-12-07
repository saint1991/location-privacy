#ifdef SIMULATOR_EXPORTS
#define BASE_SIMULATOR_API __declspec(dllexport)
#else
#define BASE_SIMULATOR_API __declspec(dllimport)
#endif

#pragma once
#include "BasicDbMap.h"
#include "LatLng.h"
#include "BasicUser.h"
#include "Dummy.h"
#include "PreferenceRequirement.h"
#include "ISimulator.h"
#include "DbTrajectoryLoader.h"
#include "PreferenceTree.h"
#include "Dijkstra.h"
#include "TimeUtility.h"

namespace Simulation
{

	///<summary>
	/// シミュレータ実装の一例
	/// user_idはDB上でのユーザのID
	///</summary>
	class BASE_SIMULATOR_API BaseSimulator
		: public ISimulator<Map::BasicDbMap, User::BasicUser<Geography::LatLng>, Entity::Dummy<Geography::LatLng>, Requirement::PreferenceRequirement, Geography::LatLng, Graph::SemanticTrajectory<Geography::LatLng>>
	{
	private:
		//トラジェクトリ加工用 (最低この間隔がない場合間隔を広げる)
		static constexpr int MIN_SERVICE_INTERVAL = 300;
		const int TRAJECTORY_LENGTH_THRESHOLD = 3;
		const double TRAININGSET_PROPORTION;
		const std::string DB_NAME;
		const unsigned int USER_ID;
		
	
	protected:
		unsigned int current_trajectory_id;
		std::shared_ptr<User::PreferenceTree> user_preference_tree;
		std::shared_ptr<User::PreferenceTree> observed_preference_tree;

		Graph::Rectangle<Geography::LatLng> calc_map_boundary(const Graph::Rectangle<Geography::LatLng>& trajectory_boundary, double boundary_side_length = 10000.0);
		void build_map(const Graph::Rectangle<Geography::LatLng>& boundary);
		void create_trajectories();
		void build_user_preference_tree();
		virtual void make_requirement_list() = 0;
		
	public:
		BaseSimulator(unsigned int user_id = 1, double trainingset_proportion = 0.3, const std::string& db_name = "map_tokyo_category_top_level");
		virtual ~BaseSimulator();
		void prepare();
		virtual void run() = 0;
		virtual void evaluate() = 0;
		virtual void export_evaluation_result() = 0;
	};
}



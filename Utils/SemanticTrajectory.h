#ifdef UTILS_EXPORTS
#define SEMANCIT_TRAJECTORY_API __declspec(dllexport)
#else
#define SEMANCIT_TRAJECTORY_API __declspec(dllimport)
#endif

#pragma once
#include "Coordinate.h"
#include "LatLng.h"
#include "Sequence.h"
#include "Trajectory.h"

namespace Graph
{

	typedef std::string category_id;

	template <typename POSITION_TYPE = Geography::LatLng>
	class  SEMANCIT_TRAJECTORY_API SemanticTrajectory : public Trajectory<POSITION_TYPE>
	{
	protected:
		std::shared_ptr<Collection::Sequence<category_id>> category_sequence;
	
	public:
		SemanticTrajectory(std::unique_ptr<std::vector<time_t>> times, bool use_relative_time = true);
		SemanticTrajectory(std::unique_ptr<std::vector<std::string>> times, bool use_relative_time = true);
		SemanticTrajectory(std::shared_ptr<Time::TimeSlotManager> timeslot);
		SemanticTrajectory(std::unique_ptr<std::vector<std::string>> times, std::shared_ptr<std::vector<MapNodeIndicator>>node_ids, std::shared_ptr<std::vector<std::shared_ptr<POSITION_TYPE>>> positions, std::shared_ptr<Collection::Sequence<category_id>>category_sequence, bool use_relative_time = true);
		~SemanticTrajectory();

		bool set_category_of_phase(int phase, const std::string& category_id);
		bool set_category_at(time_t time, const std::string& category_id);

		category_id category_of_phase(int phase) const;
		category_id category_at(time_t time) const;

		Collection::Sequence<category_id> get_category_sequence(int phase_from, int phase_end) const;
	};

	template class SemanticTrajectory<Geography::LatLng>;
	template class SemanticTrajectory<Graph::Coordinate>;
}



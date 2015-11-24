#include "stdafx.h"
#include "SemanticTrajectory.h"

namespace Graph
{

	#pragma region SemanticTrajectoryState

	///<summary>
	/// コンストラクタ
	///</summary>
	template <typename POSITION_TYPE>
	SemanticTrajectoryState<POSITION_TYPE>::SemanticTrajectoryState(time_t time, const category_id& category, std::shared_ptr<POSITION_TYPE> position)
		: TrajectoryState<POSITION_TYPE>(time, position), category(category)
	{

	}

	///<summary>
	/// ファイル出力用データ
	///</summary>
	template <typename POSITION_TYPE>
	std::unordered_map<std::string, std::string> SemanticTrajectoryState<POSITION_TYPE>::SemanticTrajectoryState::get_export_data() const
	{
		std::unordered_map<std::string, std::string> ret = TrajectoryState<POSITION_TYPE>::get_export_data();
		ret.insert(std::make_pair(CATEGORY, category));
		return ret;
	}

	#pragma endregion SemanticTrajectoryState


	///<summary>
	/// コンストラクタ
	///</summary>
	template <typename POSITION_TYPE>
	SemanticTrajectory<POSITION_TYPE>::SemanticTrajectory(std::shared_ptr<Time::TimeSlotManager const> timeslot) 
		: Trajectory<POSITION_TYPE>(timeslot), category_sequence(std::make_shared<Collection::Sequence<category_id>>(timeslot->phase_count()))
	{
	}


	///<summary>
	/// コンストラクタ
 	///</summary>
	template <typename POSITION_TYPE>
	SemanticTrajectory<POSITION_TYPE>::SemanticTrajectory(std::shared_ptr<Time::TimeSlotManager const> timeslot, std::shared_ptr<std::vector<Graph::MapNodeIndicator>> node_ids, std::shared_ptr<std::vector<std::shared_ptr<POSITION_TYPE>>> positions, std::shared_ptr<Collection::Sequence<category_id>> category_sequence)
		: Trajectory<POSITION_TYPE>(timeslot, node_ids, positions), category_sequence(category_sequence)
	{

	}


	///<summary>
	/// デストラクタ
	///</summary>
	template <typename POSITION_TYPE>
	SemanticTrajectory<POSITION_TYPE>::~SemanticTrajectory()
	{
	}


	///<summary>
	/// 指定したPhaseにおけるカテゴリを設定する
	///</summary>
	template <typename POSITION_TYPE>
	bool SemanticTrajectory<POSITION_TYPE>::set_category_of_phase(int phase, const std::string& category_id)
	{
		if (phase < 0 || category_sequence->size() <= phase) return false;
		category_sequence->at(phase) = category_id;
	}



	///<summary>
	/// 指定した時刻におけるカテゴリを設定する
	///</summary>
	template <typename POSITION_TYPE>
	bool SemanticTrajectory<POSITION_TYPE>::set_category_at(time_t time, const std::string& category_id)
	{
		int phase = timeslot->find_phase_of_time(time);
		return set_category_of_phase(phase, category_id);
	}


	///<summary>
	/// 指定したPhaseにおけるカテゴリIDを返す
	/// 不正なPhaseの場合は空文字列を返す
	///</summary>
	template <typename POSITION_TYPE>
	category_id SemanticTrajectory<POSITION_TYPE>::category_of_phase(int phase) const
	{
		if (phase < 0 || category_sequence->size() <= phase) return "";
		return category_sequence->at(phase);
	}


	///<summary>
	/// 指定した時刻におけるカテゴリIDを返す
	/// 不正な時刻の場合は空文字列を返す
	///</summary>
	template <typename POSITION_TYPE>
	category_id SemanticTrajectory<POSITION_TYPE>::category_at(time_t time) const
	{
		int phase = timeslot->find_phase_of_time(time);
		return category_of_phase(phase);
	}


	///<summary>
	/// 各時刻における状態について繰り返し実行する
	///</summary>
	template <typename POSITION_TYPE>
	void SemanticTrajectory<POSITION_TYPE>::foreach(const std::function<void(int, time_t, std::shared_ptr<POSITION_TYPE const>, const category_id&)>& execute_function) const
	{
		timeslot->for_each_time([&](time_t time, long interval, int phase) {
			std::shared_ptr<POSITION_TYPE const> position = positions->at(phase);
			category_id category = category_sequence->at(phase);
			execute_function(phase, time, position, category);
		});
	}


	///<summary>
	/// from_phaseからto_phaseまでのカテゴリシークエンスを取り出す
	///</summary>
	template <typename POSITION_TYPE>
	Collection::Sequence<category_id> SemanticTrajectory<POSITION_TYPE>::get_category_sequence(int from_phase, int to_phase) const
	{
		return category_sequence->subsequence(from_phase, to_phase);
	}



	///<summary>
	/// ファイルエクスポート用トラジェクトリデータを取得する
	///</summary>
	template <typename POSITION_TYPE>
	std::list<std::shared_ptr<IO::FileExportable const>> SemanticTrajectory<POSITION_TYPE>::get_export_data() const
	{
		std::list<std::shared_ptr<IO::FileExportable const>> ret;
		timeslot->for_each_time([this, &ret](time_t time, long interval, int phase) {
			std::shared_ptr<POSITION_TYPE> position = positions->at(phase);
			category_id category = category_sequence->at(phase);
			std::shared_ptr<IO::FileExportable const> data = std::make_shared<SemanticTrajectoryState<POSITION_TYPE> const>(time, category, position);
			ret.push_back(data);
		});
		return ret;
	}

}

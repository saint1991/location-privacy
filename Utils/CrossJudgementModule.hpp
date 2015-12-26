
namespace Observer
{

	///<summary>
	/// コンストラクタ
	///</summary>
	template <typename MAP_TYPE, typename POSITION_TYPE, typename TRAJECTORY_TYPE, typename DUMMY_TYPE, typename USER_TYPE>
	CrossJudgementModule<MAP_TYPE, POSITION_TYPE, TRAJECTORY_TYPE, DUMMY_TYPE, USER_TYPE>::CrossJudgementModule(
		std::shared_ptr<MAP_TYPE const> map,
		std::shared_ptr<Entity::EntityManager<POSITION_TYPE, TRAJECTORY_TYPE, DUMMY_TYPE, USER_TYPE> const> entities,
		double move_speed,
		const std::function<bool(std::shared_ptr<MAP_TYPE const>, const Graph::MapNodeIndicator&, const Graph::MapNodeIndicator&, const Graph::MapNodeIndicator&, const Graph::MapNodeIndicator&, double, long)>& cross_rule
		) : map(map), entities(entities), move_speed(move_speed), cross_rule(cross_rule)
	{
	}


	///<summary>
	/// デストラクタ
	///</summary>
	template <typename MAP_TYPE, typename POSITION_TYPE, typename TRAJECTORY_TYPE, typename DUMMY_TYPE, typename USER_TYPE>
	CrossJudgementModule<MAP_TYPE, POSITION_TYPE, TRAJECTORY_TYPE, DUMMY_TYPE, USER_TYPE>::~CrossJudgementModule()
	{
	}

	///<summary>
	/// 基本の交差判定ルール
	///</summary>
	template <typename MAP_TYPE, typename POSITION_TYPE, typename TRAJECTORY_TYPE, typename DUMMY_TYPE, typename USER_TYPE>
	bool CrossJudgementModule<MAP_TYPE, POSITION_TYPE, TRAJECTORY_TYPE, DUMMY_TYPE, USER_TYPE>::basic_cross_rule(std::shared_ptr<MAP_TYPE const>, const Graph::MapNodeIndicator& previous_position1, const Graph::MapNodeIndicator& current_position1, const Graph::MapNodeIndicator& previous_position2, const Graph::MapNodeIndicator& current_position2, double move_speed, long interval) const
	{
		return map->is_reachable(previous_position1, current_position2, move_speed, (double)interval) && map->is_reachable(previous_position2, current_position1, move_speed, (double)interval);
	}

	///<summary>
	/// 指定したエンティティの交差情報を全て取得する
	///</summary>
	template <typename MAP_TYPE, typename POSITION_TYPE, typename TRAJECTORY_TYPE, typename DUMMY_TYPE, typename USER_TYPE>
	std::vector<CrossInfo> CrossJudgementModule<MAP_TYPE, POSITION_TYPE, TRAJECTORY_TYPE, DUMMY_TYPE, USER_TYPE>::get_all_cross_info_of_entity(Entity::entity_id id) const
	{
		std::vector<CrossInfo> ret;
		std::shared_ptr<Entity::MobileEntity<POSITION_TYPE, TRAJECTORY_TYPE> const> entity = entities->read_entity_by_id(id);
		std::shared_ptr<Time::TimeSlotManager const> timeslot = entities->read_timeslot();

		timeslot->for_each_time([&](time_t time, long interval, int phase) {
			if (phase == 0) return;

			//交差相手を格納する
			std::vector<Entity::entity_id> cross_entities;

			//指定エンティティの位置と1フェーズ前の位置
			Graph::MapNodeIndicator previous_node = entity->read_node_pos_info_of_phase(phase - 1).first;
			Graph::MapNodeIndicator current_node = entity->read_node_pos_info_of_phase(phase).first;
			for (Entity::entity_id target_id = 0; target_id <= entities->get_dummy_count(); target_id++) {
				//交差相手のエンティティ
				std::shared_ptr<Entity::MobileEntity<POSITION_TYPE, TRAJECTORY_TYPE> const> target_entity = entities->read_entity_by_id(target_id);
				Graph::MapNodeIndicator target_previous_node = target_entity->read_node_pos_info_of_phase(phase - 1).first;
				Graph::MapNodeIndicator target_current_node = target_entity->read_node_pos_info_of_phase(phase).first;

				//交差条件の判定
				if (cross_rule(map, previous_node, current_node, target_previous_node, target_current_node, move_speed, interval)) {
					cross_entities.push_back(target_id);
				}
			}
			ret.push_back(CrossInfo(phase - 1, id, cross_entities));
		});

		return ret;
	}


	///<summary>
	/// 対象のエンティティの取りうるトラジェクトリの木構造を取得する
	/// phaseがlast_phaseの場合は空のvectorを返す
	///</summary>
	template <typename MAP_TYPE, typename POSITION_TYPE, typename TRAJECTORY_TYPE, typename DUMMY_TYPE, typename USER_TYPE>
	std::vector<CrossInfo> CrossJudgementModule<MAP_TYPE, POSITION_TYPE, TRAJECTORY_TYPE, DUMMY_TYPE, USER_TYPE>::get_all_cross_info_of_phase(int phase) const
	{

		std::shared_ptr<Time::TimeSlotManager const> timeslot = entities->read_timeslot();
		long interval = timeslot->time_of_phase(phase + 1) - timeslot->time_of_phase(phase);
		int last_phase = timeslot->phase_count() - 1;

		if (phase == last_phase) return std::vector<CrossInfo>();

		//対象エンティティのトラジェクトリ
		std::vector<CrossInfo> ret(entities->get_dummy_count() + 1);
		for (Entity::entity_id id = 0; id <= entities->get_dummy_count(); id++) {
			
			std::shared_ptr<Entity::MobileEntity<POSITION_TYPE, TRAJECTORY_TYPE> const> entity = entities->read_entity_by_id(id);
			Graph::MapNodeIndicator current_node = entity->read_node_pos_info_of_phase(phase).first;
			Graph::MapNodeIndicator next_node = entity->read_node_pos_info_of_phase(phase + 1).first;

			std::vector<Entity::entity_id> cross_entities;

			for (Entity::entity_id target_id = 0; target_id <= entities->get_dummy_count(); target_id++) {
				if (id == target_id) continue;
				
				std::shared_ptr<Entity::MobileEntity<POSITION_TYPE, TRAJECTORY_TYPE> const> target_entity = entities->read_entity_by_id(target_id);
				Graph::MapNodeIndicator current_target_node = target_entity->read_node_pos_info_of_phase(phase).first;
				Graph::MapNodeIndicator next_target_node = target_entity->read_node_pos_info_of_phase(phase + 1).first;

				if (cross_rule(map, current_node, next_node, current_target_node, next_target_node, move_speed, interval)) {
					cross_entities.push_back(target_id);
				}
			}
			ret.at(id) = CrossInfo(phase, id, cross_entities);
		}
		return ret;
	}

	///<summary>
	/// トラジェクトリとその交差関係を基に観測されるすべてのトラジェクトリモデルを作成する
	///</summary>
	template <typename MAP_TYPE, typename POSITION_TYPE, typename TRAJECTORY_TYPE, typename DUMMY_TYPE, typename USER_TYPE>
	std::shared_ptr<ObservedTrajectoryStructure> CrossJudgementModule<MAP_TYPE, POSITION_TYPE, TRAJECTORY_TYPE, DUMMY_TYPE, USER_TYPE>::create_observed_trajectory_structure() const
	{
		std::shared_ptr<ObservedTrajectoryStructure> trajectory_structure = std::make_shared<ObservedTrajectoryStructure>();
		ObservedTrajectoryStructure::base_iterator iter = trajectory_structure->root<ObservedTrajectoryStructure::base_iterator>();

		//まずは交差を考えずに個々の可観測なトラジェクトリ情報を作成する
		std::shared_ptr<Time::TimeSlotManager const> timeslot = entities->read_timeslot();
		timeslot->for_each_time([&](time_t time, long interval, int phase) {

			int depth = phase + 1;

			for (Entity::entity_id id = 0; id <= entities->get_dummy_count(); id++) {

				//着目しているエンティティに応じて親ノードを更新
				std::shared_ptr<Entity::MobileEntity<Geography::LatLng, Graph::SemanticTrajectory<Geography::LatLng>> const> entity = entities->read_entity_by_id(id);
				if (phase >= 1) {
					Graph::MapNodeIndicator previous_node = entity->read_node_pos_info_of_phase(phase - 1).first;
					iter = trajectory_structure->find_node(previous_node, phase - 1);
				}

				//次の訪問地点をエンティティ数1で子ノードとして接続する
				//既存のノードの場合はエンティティ数をカウントアップする
				Graph::MapNodeIndicator current_node = entity->read_node_pos_info_of_phase(phase).first;
				ObservedTrajectoryStructure::base_iterator child = iter.find_child_if([&current_node](std::shared_ptr<ObservedTrajectoryNode const> node) {
					return *node->data == current_node;
				});
				if (*child == nullptr) {
					std::shared_ptr<ObservedTrajectoryNode> new_node = std::make_shared<ObservedTrajectoryNode>(trajectory_structure->node_count(), depth, std::make_shared<Graph::MapNodeIndicator>(current_node));
					iter.add_child(new_node);
				}
				else {
					child->count_up();
				}
			}
		});

		//交差情報を元に取りうるトラジェクトリの接続関係を追加する
		for (Entity::entity_id id = 0; id <= entities->get_dummy_count(); id++) {

			//着目するエンティティとその交差情報
			std::shared_ptr<Entity::MobileEntity<Geography::LatLng, Graph::SemanticTrajectory<Geography::LatLng>> const> entity = entities->read_entity_by_id(id);
			std::vector<Evaluate::CrossInfo> cross_infos = get_all_cross_info_of_entity(id);

			//交差相手の辿ったノードへのパスがない場合はそれを作成する
			for (std::vector<Evaluate::CrossInfo>::const_iterator cross_info = cross_infos.begin(); cross_info != cross_infos.end(); cross_info++) {

				//交差を作成するtrajectory_nodeを取得する
				int cross_phase = cross_info->phase;
				Graph::MapNodeIndicator cross_node = entity->read_node_pos_info_of_phase(cross_phase).first;
				ObservedTrajectoryStructure::base_iterator iter = trajectory_structure->find_node(cross_node, cross_info->phase);

				//交差相手のノードを取得しtrajectory_nodeにエッジを追加
				for (std::vector<Entity::entity_id>::const_iterator target_entity_id = cross_info->crossing_entities.begin(); target_entity_id != cross_info->crossing_entities.end(); target_entity_id++) {

					//エッジを張る先のノードのIDを取得(交差相手のcross_phase + 1の訪問場所)
					Graph::MapNodeIndicator next_node = entities->read_entity_by_id(id)->read_node_pos_info_of_phase(cross_phase + 1).first;
					Graph::node_id next_node_id = trajectory_structure->find_node_id(next_node, cross_phase + 1);

					if (!iter->is_connecting_to(next_node_id)) iter->connect_to(std::make_shared<Graph::BasicEdge>(next_node_id));
				}
			}
		}
		return trajectory_structure;
	}
}


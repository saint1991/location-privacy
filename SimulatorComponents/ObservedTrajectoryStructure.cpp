#include "stdafx.h"
#include "ObservedTrajectoryStructure.h"

namespace Observer
{

	///<summary>
	/// コンストラクタ
	///</summary>
	ObservedTrajectoryStructure::ObservedTrajectoryStructure() : Graph::Tree<ObservedTrajectoryNode, Graph::MapNodeIndicator, Graph::FlowEdge, double>()
	{
		initialize(std::make_shared<ObservedTrajectoryNode>(0, 0, nullptr));
	}


	///<summary>
	/// デストラクタ
	///</summary>
	ObservedTrajectoryStructure::~ObservedTrajectoryStructure()
	{
	}


	///<summary>
	/// 指定した値でroot_nodeを初期化
	///</summary>
	void ObservedTrajectoryStructure::initialize(std::shared_ptr<ObservedTrajectoryNode> root_node)
	{
		if (root_node->get_depth() != 0) throw std::invalid_argument("Depth of root node must be 0");
		Graph::Tree<ObservedTrajectoryNode, Graph::MapNodeIndicator, Graph::FlowEdge, double>::initialize(root_node);
	}


	///<summary>
	/// MapNodeIndicatorとPhaseを基にノードを探索し，見つかった場合はそのノードを指すイテレータを返す．
	/// 見つからない場合はnullptrを指すイテレータを返す．
	///</summary>
	ObservedTrajectoryStructure::base_iterator ObservedTrajectoryStructure::find_node(const Graph::MapNodeIndicator& id, int phase)
	{
		int depth = phase + 1;
		Collection::IdentifiableCollection<Graph::node_id, Observer::ObservedTrajectoryNode>::iterator iter = std::find_if(node_collection->begin(), node_collection->end(), [id, depth](std::shared_ptr<Identifiable<Graph::node_id>> element) {
			std::shared_ptr<Observer::ObservedTrajectoryNode> node = std::dynamic_pointer_cast<Observer::ObservedTrajectoryNode>(element);
			return node != nullptr && *node->data == id && depth == node->get_depth();
		});

		return iter == node_collection->end() ? ObservedTrajectoryStructure::base_iterator(-1, nullptr) : ObservedTrajectoryStructure::base_iterator((*iter)->get_id(), node_collection);
	}


	///<summary>
	/// マップのノードIDと訪問フェーズから対応するTrajectoryNodeのIDを取得する
	/// 該当するノードがない場合-1を返す
	///</summary>
	Graph::node_id ObservedTrajectoryStructure::find_node_id(const Graph::MapNodeIndicator& id, int phase) const
	{
		int depth = phase + 1;
		Collection::IdentifiableCollection<Graph::node_id, Observer::ObservedTrajectoryNode>::iterator iter = std::find_if(node_collection->begin(), node_collection->end(), [id, depth](std::shared_ptr<Identifiable<Graph::node_id>> element) {
			std::shared_ptr<Observer::ObservedTrajectoryNode> node = std::dynamic_pointer_cast<Observer::ObservedTrajectoryNode>(element);
			return node != nullptr && node->data != nullptr && *node->data == id && depth == node->get_depth();
		});

		return iter == node_collection->end() ? -1 : (*iter)->get_id();
	}


	///<summary>
	/// トラジェクトリ長を取得する
	/// ノードが存在しない場合は-1を返す
	///</summary>
	size_t ObservedTrajectoryStructure::trajectory_length() const
	{
		Collection::IdentifiableCollection<Graph::node_id, ObservedTrajectoryNode>::const_iterator iter = std::max_element(node_collection->begin(), node_collection->end(), [](std::shared_ptr<Identifiable<Graph::node_id>> largest, std::shared_ptr<Identifiable<Graph::node_id>> first) {
			std::shared_ptr<ObservedTrajectoryNode> largest_node = std::dynamic_pointer_cast<ObservedTrajectoryNode>(largest);
			std::shared_ptr<ObservedTrajectoryNode> target_node = std::dynamic_pointer_cast<ObservedTrajectoryNode>(first);
			return largest_node->get_depth() < target_node->get_depth();
		});

		std::shared_ptr<ObservedTrajectoryNode> node = std::dynamic_pointer_cast<ObservedTrajectoryNode>(*iter);
		return node == nullptr ? -1 : node->get_depth();
	}


	///<summary>
	/// 指定したIDを持つノードをルートとする部分構造のコピーを取得する
	/// 指定したIDのノードが存在しない場合nullptrを返す
	///</summary>
	std::shared_ptr<ObservedTrajectoryStructure> ObservedTrajectoryStructure::sub_structure(Graph::node_id root_node_id) const
	{
		std::shared_ptr<ObservedTrajectoryStructure> ret = std::make_shared<ObservedTrajectoryStructure>();

		//深さ優先探索でStructureを探索し，コピーすべきノードのIDリストを作成
		ret->root_node->connect_to(std::make_shared<Graph::FlowEdge>(root_node_id));
		ObservedTrajectoryStructure::depth_first_const_iterator iter = ObservedTrajectoryStructure::depth_first_const_iterator(root_node_id, node_collection);
		if (*iter == nullptr) return nullptr;
		int depth_offset = iter->get_depth() - 1;
		while (*iter != nullptr) {
			Graph::node_id new_node_id = ret->node_collection->size();
			std::shared_ptr<ObservedTrajectoryNode> node = std::make_shared<ObservedTrajectoryNode>(**iter);
			node->set_depth(iter->get_depth() - depth_offset);
			ret->node_collection->add(node);
			iter++;
		}
		return ret;
	}


	///<summary>
	/// 指定したPhaseに指定したMapNodeIndicatorに訪問するノードをルートとする部分構造のコピーを取得する
	/// 指定したIDのノードが存在しない場合nullptrを返す
	///</summary>
	std::shared_ptr<ObservedTrajectoryStructure> ObservedTrajectoryStructure::sub_structure(const Graph::MapNodeIndicator& map_node_id, int phase) const
	{
		Graph::node_id id = find_node_id(map_node_id, phase);
		return id == -1 ? nullptr : sub_structure(id);
	}


	///<summary>
	/// 深さ優先探索の順序通りにノードを取得し，それぞれのノードについてexecute_functionを実行する
	///</summary>
	void ObservedTrajectoryStructure::depth_first_iteration(Graph::node_id start_node_id, const std::function<void(std::shared_ptr<ObservedTrajectoryNode>)>& execute_function)
	{
		ObservedTrajectoryStructure::depth_first_iterator iter = get_iter_by_id<ObservedTrajectoryStructure::depth_first_iterator>(start_node_id);
		while (*iter != nullptr) {
			execute_function(*iter);
			iter++;
		}
	}


	///<summary>
	/// 幅優先探索の順序通りにノードを取得し，それぞれのノードについてexecute_functionを実行する
	///</summary>
	void ObservedTrajectoryStructure::breadth_first_iteration(Graph::node_id start_node_id, const std::function<void(std::shared_ptr<ObservedTrajectoryNode>)>& execute_function)
	{
		ObservedTrajectoryStructure::breadth_first_iterator iter = get_iter_by_id<ObservedTrajectoryStructure::breadth_first_iterator>(start_node_id);
		while (*iter != nullptr) {
			execute_function(*iter);
			iter++;
		}
	}



	///<summary>
	/// 根から葉までの全てのパスについてexecute_functionを実行する
	///</summary>
	void ObservedTrajectoryStructure::for_each_possible_trajectory(const std::function<void(const Collection::Sequence<Graph::MapNodeIndicator>&)>& execute_function) const
	{
		size_t length = trajectory_length();
		ObservedTrajectoryStructure::revisit_depth_first_const_iterator iter = const_root<ObservedTrajectoryStructure::revisit_depth_first_const_iterator>();
		iter++;
		
		Collection::Sequence<Graph::MapNodeIndicator> trajectory;
		while (*iter != nullptr) {
			int phase = iter->get_depth() - 1;
			if (phase == 0) trajectory.clear();
			else trajectory = trajectory.subsequence(0, phase - 1);
			trajectory.push_back(*iter->data);
			if (trajectory.size() == length) execute_function(trajectory);
			iter++;
		}
	}

	
	///<summary>
	/// 取りうるトラジェクトリを全て取得する
	///</summary>
	std::vector<Collection::Sequence<Graph::MapNodeIndicator>> ObservedTrajectoryStructure::get_all_possible_trajectories() const
	{
		std::vector<Collection::Sequence<Graph::MapNodeIndicator>> trajectories;
		for_each_possible_trajectory([&trajectories](const Collection::Sequence<Graph::MapNodeIndicator>& trajectory) {
			trajectories.push_back(trajectory);
		});
		return trajectories;
	}


	///<summary>
	/// trajectoryに該当するエンティティがその経路で移動した確率を算出する
	/// 長さ0のトラジェクトリの場合は0.0になるので注意
	///</summary>
	double ObservedTrajectoryStructure::calc_probability_of_trajectory(const std::vector<Graph::MapNodeIndicator>& trajectory) const
	{
		if (trajectory.size() == 0) return 0.0;
		double probability = 1.0;
		
		ObservedTrajectoryStructure::base_const_iterator iter = const_root<ObservedTrajectoryStructure::base_const_iterator>();

		for (int phase = 0; phase < trajectory.size(); phase++) {
			Graph::MapNodeIndicator node = trajectory.at(phase);
			iter = iter.find_child_if([&node](std::shared_ptr<ObservedTrajectoryNode const> child) {
				return *child->data == node;
			});
			if (*iter == nullptr) return 0.0;
			if (phase < trajectory.size() - 1) {
				Graph::MapNodeIndicator next_node = trajectory.at(phase + 1);
				Graph::node_id next_node_id = find_node_id(next_node, phase + 1);
				probability *= iter->get_static_edge_to(next_node_id)->get_flow();
			}
		}

		return probability;
	}
}


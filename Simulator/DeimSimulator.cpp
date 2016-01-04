#include "stdafx.h"
#include "DeimSimulator.h"

namespace Simulation
{

	DeimSimulator::DeimSimulator() : BaseSimulator(USER_ID, DB_NAME), current_trajectory_id(0)
	{
	}


	DeimSimulator::~DeimSimulator()
	{
	}

	///<summary>
	/// シミュレーションのパラメータリスト
	///</summary>
	void DeimSimulator::make_requirement_list()
	{
		requirements = {
			std::make_shared<Requirement::PreferenceRequirement>(std::pow(1000.0, 2), 16, 1.0, AVERAGE_SPEED, 1.0),
			std::make_shared<Requirement::PreferenceRequirement>(std::pow(1000.0, 2), 16, 0.9, AVERAGE_SPEED, 1.0),
			std::make_shared<Requirement::PreferenceRequirement>(std::pow(1000.0, 2), 16, 0.8, AVERAGE_SPEED, 1.0),
			std::make_shared<Requirement::PreferenceRequirement>(std::pow(1000.0, 2), 16, 0.7, AVERAGE_SPEED, 1.0),
			std::make_shared<Requirement::PreferenceRequirement>(std::pow(1000.0, 2), 16, 0.6, AVERAGE_SPEED, 1.0),
			std::make_shared<Requirement::PreferenceRequirement>(std::pow(1000.0, 2), 16, 0.5, AVERAGE_SPEED, 1.0),
			std::make_shared<Requirement::PreferenceRequirement>(std::pow(1000.0, 2), 16, 0.4, AVERAGE_SPEED, 1.0),
			std::make_shared<Requirement::PreferenceRequirement>(std::pow(1000.0, 2), 16, 0.3, AVERAGE_SPEED, 1.0),
			std::make_shared<Requirement::PreferenceRequirement>(std::pow(1000.0, 2), 16, 0.2, AVERAGE_SPEED, 1.0),
			std::make_shared<Requirement::PreferenceRequirement>(std::pow(1000.0, 2), 16, 0.1, AVERAGE_SPEED, 1.0),
			std::make_shared<Requirement::PreferenceRequirement>(std::pow(1000.0, 2), 16, 0.0, AVERAGE_SPEED, 1.0)
		};
	}


	void DeimSimulator::run()
	{

		int failed_count = 0;

		for (std::list<std::shared_ptr<Requirement::PreferenceRequirement const>>::const_iterator iter = requirements.begin(); iter != requirements.end(); iter++) {
			for (current_trajectory_id = 0; current_trajectory_id < user_trajectories->size(); current_trajectory_id++) {
			
				//対象のユーザトラジェクトリ
				std::shared_ptr<Graph::SemanticTrajectory<Geography::LatLng>> user_trajectory = user_trajectories->at(current_trajectory_id);
				std::shared_ptr<Time::TimeSlotManager const> timeslot = user_trajectory->read_timeslot();
				
				////読み込む地図の領域
				//Graph::Rectangle<Geography::LatLng> map_boundary = user_trajectory->get_trajectory_boundary();
				//map_boundary = Map::BasicDbMap::calc_map_boundary(map_boundary);
				
				////地図の読み込み
				//build_map(map_boundary);

				User::UserTrajectoryConverter converter(map);

				//トラジェクトリ変換
				std::shared_ptr<Graph::SemanticTrajectory<Geography::LatLng>> converted_trajectory = converter.extract_walking_semantic_trajectory(user_trajectory, TRAJECTORY_LENGTH_THRESHOLD, AVERAGE_SPEED);
				if (converted_trajectory == nullptr) continue;

				//可観測な嗜好の木のコピー
				std::shared_ptr<User::PreferenceTree> observed_preference_tree_copy = std::make_shared<User::PreferenceTree>(*observed_preference_tree);
				
				//ユーザの作成
				std::shared_ptr<User::BasicUser<Geography::LatLng>> user = std::make_shared<User::BasicUser<Geography::LatLng>>(user_trajectory, user_preference_tree);
				
				//提案手法のインスタンス化
				std::shared_ptr<Framework::IProposedMethod<
					Map::BasicDbMap, User::BasicUser<Geography::LatLng>, Entity::Dummy<Geography::LatLng>,
					Requirement::PreferenceRequirement, Geography::LatLng, Graph::SemanticTrajectory<Geography::LatLng>
				>> proposed = std::make_shared<Method::MizunoMethod>(map, user, observed_preference_tree_copy, *iter, timeslot);
				
				//各トラジェクトリに対して手法を適用した後のコールバック
				proposed->set_execution_callback(std::bind(&DeimSimulator::each_trajectory_end_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
				
				try {
					//提案手法の起動
					proposed->run();
				}
				catch (Framework::TrajectoryNotFoundException &e) {
					std::cout << e.what() << std::endl;
					failed_count++;
				}
				

				//今だけ
				break;
			}

			//今だけ
			break;
		}

		double fail_rate = failed_count / user_trajectories->size();
		std::cout << "Fail Rate: " << std::to_string(fail_rate) << std::endl;
	}

	///<summary>
	/// 各トラジェクトリ実行後のコールバック
	///</summary>
	void DeimSimulator::each_trajectory_end_callback(std::shared_ptr<Entity::EntityManager<Geography::LatLng, Graph::SemanticTrajectory<Geography::LatLng>, Entity::Dummy<>, User::BasicUser<>>> entities, std::shared_ptr<Requirement::PreferenceRequirement const> requirement, std::shared_ptr<Time::Timer> timer)
	{
		
		std::list<std::pair<std::string, std::string>> export_name_map = {
			{Geography::LatLng::LATITUDE, "緯度" },
			{Geography::LatLng::LONGITUDE, "経度" },
			{Graph::TrajectoryState<>::TIME, "タイムスタンプ"},
			{Graph::SemanticTrajectoryState<>::CATEGORY, "カテゴリID"},
			{Graph::SemanticTrajectoryState<>::CATEGORY_NAME, "カテゴリ名"},
			{Graph::TrajectoryState<>::VENUE_NAME, "POI名"}
		};
		IO::FileExporter exporter(export_name_map, "C:/Users/Mizuno/Desktop/EvaluationResults/user_trajectory");
		
		//ユーザトラジェクトリのエクスポート
		std::list<std::shared_ptr<IO::FileExportable const>> user_trajectory = entities->get_user()->read_trajectory()->get_export_data();
		exporter.export_lines(user_trajectory);

		//各ダミーのエクスポート
		for (int dummy_id = 1; dummy_id <= entities->get_dummy_count(); dummy_id++) {
			std::shared_ptr<Entity::Dummy<> const> dummy = entities->read_dummy_by_id(dummy_id);
			std::list<std::shared_ptr<IO::FileExportable const>> dummy_trajectory = dummy->read_trajectory()->get_export_data();
			IO::FileExporter dummy_exporter(export_name_map, "C:/Users/Mizuno/Desktop/EvaluationResults/dummy" + std::to_string(dummy_id));
			dummy_exporter.export_lines(dummy_trajectory);
		}

		std::shared_ptr<Observer::SemanticObserver<Entity::Dummy<Geography::LatLng>, User::BasicUser<Geography::LatLng>>> observer
			= std::make_shared<Observer::SemanticObserver<Entity::Dummy<Geography::LatLng>, User::BasicUser<Geography::LatLng>>>(map, entities, user_preference_tree, AVERAGE_SPEED + 1.0);
		
		//AR系の評価
		double ar_count = observer->calc_ar_count(requirement->required_anonymous_area);
		std::cout << "AR-Count: " << std::to_string(ar_count) << std::endl;
		
		double ar_size = observer->calc_ar_size(requirement->required_anonymous_area);
		std::cout << "AR-Size: " << std::to_string(ar_size) << "m^2" << std::endl;
		
		//MTCの評価
		double mtc1 = observer->calc_mtc_without_semantics();
		std::cout << "MTC1: " << std::to_string(mtc1) << "sec" << std::endl;
		
		double mtc2 = observer->calc_mtc_with_semantics();
		std::cout << "MTC2: " << std::to_string(mtc2) << "sec" << std::endl;

		//可観測な木の更新
		observer->for_each_category_sequence_of_possible_trajectory([&](const Collection::Sequence<User::category_id>& sequence, double expected_frequency) {
			observed_preference_tree->add_sequence_counter(sequence, expected_frequency);
		});

		double similarity = User::similarity(*user_preference_tree, *observed_preference_tree);
		similarity_vector_proposed.push_back(similarity);
	}

	void DeimSimulator::evaluate()
	{
		
	}

	void DeimSimulator::export_evaluation_result()
	{

	}



}



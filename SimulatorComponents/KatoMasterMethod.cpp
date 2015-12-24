#include "stdafx.h"
#include "KatoMasterMethod.h"

namespace Method
{

	///<summary>
	/// コンストラクタ
	/// これにSimulatorで作成した各種入力への参照を渡す
	///</summary>
	KatoMasterMethod::KatoMasterMethod(std::shared_ptr<Map::HayashidaDbMap const> map, std::shared_ptr<Entity::DifferentMovementUser<Geography::LatLng>> user, std::shared_ptr<Requirement::KatoMethodRequirement const> requirement, std::shared_ptr<Time::TimeSlotManager> time_manager)
		: KatoBachelorMethod(map, user, requirement, time_manager),
		real_user(nullptr), predicted_user(nullptr),Tu(0.0)
	{
		//input_userとreal_userの生成．この2つは変更しない
		input_user = entities->get_user();
		real_user = entities->get_user()->get_real_user();
	}

	///<summary>
	/// デストラクタ
	///</summary>
	KatoMasterMethod::~KatoMasterMethod()
	{
	}
	
	///<summary>
	/// input_user_planからユーザの行動を予測し，そのユーザを返す．
	/// 加藤さん手法の場合は，各要素の生成確率によって割合を決める．
	/// その割合に従って，実際のユーザを作成する
	///</summary>
	std::shared_ptr<Entity::RevisablePauseMobileEntity<Geography::LatLng>> KatoMasterMethod::copy_predicted_user_plan(std::shared_ptr<Entity::PauseMobileEntity<Geography::LatLng>> input_user)
	{
		//std::shared_ptr<std::vector<std::shared_ptr<Geography::LatLng>>> input_user_positions = input_user->get_trajectory()->get_positions();
		//std::shared_ptr<std::vector<std::shared_ptr<Geography::LatLng>>> predicted_user_positions = predicted_user->get_trajectory()->get_positions();

		//std::copy(input_user_positions->begin(), input_user_positions->end(), predicted_user_positions->begin());
		
		//とりあえず今だけこの形で
		predicted_user = entities->get_user();
		return predicted_user;
	}


	///<summary>
	/// 速度の最大変化値と最小変化値を計算する
	/// pair(Max, min)
	///</summary>
	std::pair<double, double> KatoMasterMethod::calc_max_variable_speed(double speed)
	{
		double max_speed = requirement->average_speed_of_dummy + requirement->speed_range_of_dummy;
		double min_speed = requirement->average_speed_of_dummy - requirement->speed_range_of_dummy;

		return std::make_pair(MAX_VARIATION_OF_SPEED*(max_speed - speed), MAX_VARIATION_OF_SPEED*(speed - min_speed));
	}

	///<summary>
	/// 停止時間の最大変化値と最小変化値を計算する
	/// pair(Max, min)
	///</summary>
	std::pair<double, double> KatoMasterMethod::calc_max_variable_pause_time(double pause_time)
	{
		return std::make_pair(MAX_VARIATION_OF_PAUSE_TIME*(requirement->max_pause_time - pause_time), MAX_VARIATION_OF_PAUSE_TIME*(pause_time - requirement->min_pause_time));
	}


		
	///<summary>
	/// ユーザの行動プランに含まれる停止地点に向かっているかどうかをチェック
	///</summary>
	bool KatoMasterMethod::check_going_same_poi_as_plan()
	{
		return true;
	}

	///<summary>
	/// ユーザの行動プラン変更の判断
	/// real_userとpredict_userの違いで見る
	/// ユーザがプラン通りに行動している場合はNO_CHANGEをリターン
	///</summary>
	KatoMasterMethod::ChangeParameter KatoMasterMethod::check_user_plan(int now_phase)
	{
		//止まっている→停止時間の変更or停止地点の変更
		//もし片方でも停止していたら→停止時間変更の判断
		if (predicted_user->check_pause_condition(now_phase) || real_user->check_pause_condition(now_phase)) {
			return check_user_pause_time(now_phase);
		}
		//動いていたら，
		else {
			//もし線形補間した線の上に位置していたら
			if (check_on_the_path(now_phase)) {
				return check_user_speed(now_phase);
			}
			else {
				return check_user_path(now_phase);
			}
		}
	}
	
		///<summary>
		/// ユーザの行動プラン変更の判断
		/// real_userとpredicted_userの違いで見る
		/// pause_timeのチェック
		///</summary>
		KatoMasterMethod::ChangeParameter KatoMasterMethod::check_user_pause_time(int now_phase)
		{
			//もしプラン通り停止していたら(両者の停止フラグが１)，NO_CHANGE
			if (predicted_user->check_pause_condition(now_phase) && real_user->check_pause_condition(now_phase)) {
				return NO_CHANGE;
			}
			else if(predicted_user->check_pause_condition(now_phase) || real_user->check_pause_condition(now_phase)){
				//もしrealのほうが停止フラグが0で(出発していて)，predictが1なら,予定より早く出発
				if (predicted_user->check_pause_condition(now_phase) == true && real_user->check_pause_condition(now_phase) == false) {
					//change_timeの差分を求める
					Tu -= requirement->service_interval;
					return SHORTER_PAUSE_TIME;
				}
				else {
					//change_timeの差分を求める
					Tu += requirement->service_interval;
					return LONGER_PAUSE_TIME;
				}
			}
			//両方フラグが0なら(速度変更からジャンプして飛んできた場合)，速度変更でなく，rest_time分をずれを検知
			else {
				double real_user_dist = Geography::GeoCalculation::lambert_distance(*real_user->read_position_of_phase(now_phase - 1), *real_user->read_position_of_phase(now_phase));
				double predicted_user_dist = Geography::GeoCalculation::lambert_distance(*predicted_user->read_position_of_phase(now_phase - 1), *predicted_user->read_position_of_phase(now_phase));
				//移動距離がrealのほうが大きいなら
				if (real_user_dist > predicted_user_dist) {
					//change_timeの差分を求める
					Tu -= (real_user_dist - predicted_user_dist) / predicted_user->get_starting_speed();
					return SHORTER_PAUSE_TIME;
				}
				else {
					//change_timeの差分を求める
					Tu -= (predicted_user_dist - real_user_dist) / predicted_user->get_starting_speed();
					return LONGER_PAUSE_TIME;
				}
			}
		}
	

		///<summary>
		/// ユーザの行動プラン変更の判断
		/// real_userとpredicted_userの違いで見る
		/// 速度のチェック
		///</summary>
		KatoMasterMethod::ChangeParameter KatoMasterMethod::check_user_speed(int now_phase)
		{
			Geography::LatLng previous_real_user_position = *real_user->read_node_pos_info_of_phase(now_phase - 1).second;
			Geography::LatLng previous_predicted_user_position = *predicted_user->read_node_pos_info_of_phase(now_phase - 1).second;
			Geography::LatLng now_real_user_position = *real_user->read_node_pos_info_of_phase(now_phase).second;
			Geography::LatLng now_predicted_user_position = *predicted_user->read_node_pos_info_of_phase(now_phase).second;
			
			double real_user_dist = Geography::GeoCalculation::lambert_distance(*real_user->read_position_of_phase(now_phase - 1), *real_user->read_position_of_phase(now_phase));
			double predicted_user_dist = Geography::GeoCalculation::lambert_distance(*predicted_user->read_position_of_phase(now_phase - 1), *predicted_user->read_position_of_phase(now_phase));

			double real_user_rest_time = real_user->get_rest_pause_time_when_departing_using_pause_phase(now_phase - 1);
			double predicted_uesr_rest_time = predicted_user->get_rest_pause_time_when_departing_using_pause_phase(now_phase - 1);

			double real_user_speed = real_user_dist / (requirement->service_interval - real_user_rest_time);
			double predicted_user_speed = predicted_user_dist / (requirement->service_interval - real_user_rest_time);

			
			//もし同じ場所にいたら，NO_CHANGE
			if (now_predicted_user_position == now_real_user_position) {
				Tu = 0.0;
				return NO_CHANGE;
			}
			else {
				//もしrest_time分を考慮して，逆算して，速度が同じなら，停止時間の変更と判断
				if (real_user_speed == predicted_user_speed) {
					return check_user_pause_time(now_phase);
				}
				else {
					//移動距離がrealのほうが大きいなら
					if (real_user_dist > predicted_user_dist) {
						//change_timeの差分を求める
						//ここで，どれくらい速度が大きくなったかも求めてしまう．
						//今はmapを参照しているけど，計算量に関わってきそうだから要検討
						double real_speed = real_user_dist / requirement->service_interval;
						Tu += map->calc_necessary_time(real_user->read_node_pos_info_of_phase(now_phase).first, real_user->get_poi().first, real_speed);
						return FASTER_SPEED;
					}
					else {
						//change_timeの差分を求める
						//ここで，どれくらい速度が小さくなったかも求めてしまう．	
						double real_speed = real_user_dist / requirement->service_interval;
						Tu -= map->calc_necessary_time(real_user->read_node_pos_info_of_phase(now_phase).first, real_user->get_poi().first, real_speed);;
						return SLOER_SPEED;
					}
				}
			}
		}


		///<summary>
		/// ユーザの行動プラン変更の判断
		/// real_userとpredicted_userの違いで見る
		/// pathのチェック
		///</summary>
		KatoMasterMethod::ChangeParameter KatoMasterMethod::check_user_path(int now_phase)
		{
			//change_timeを求める
			//Tu = 
			return PATH;
		}


		///<summary>
		/// ユーザの行動プラン変更の判断
		/// real_userとpredicted_userの違いで見る
		/// 停止位置のチェック
		///</summary>
		KatoMasterMethod::ChangeParameter KatoMasterMethod::check_user_position(int now_phase)
		{
			return VISIT_POI;
		}

		///<summary>
		/// ダミーがパス上に存在するかどうかをチェック
		/// predicted_userの現停止POIから次の停止POIまでの経路を取ってきて，その直線上に乗っているかどうかをチェック
		///</summary>
		bool KatoMasterMethod::check_on_the_path(int phase_id)
		{
			Graph::MapNodeIndicator source;
			Graph::MapNodeIndicator destination;

			std::shared_ptr<std::vector<std::shared_ptr<Geography::LatLng>>> path_between_pois = predicted_user->get_trajectory()->get_positions();
			std::vector<std::shared_ptr<Geography::LatLng>>::iterator path_iter = path_between_pois->begin();//pathを検索するためのindex
						
			Graph::MapNodeIndicator nearest_position = source;

			return true;
		}



	///<summary>
	/// ユーザの次の停止地点の到着時間を予測する
	///</summary>
	void KatoMasterMethod::update_user_plan(ChangeParameter check_parameter, int phase_id)
	{
		switch (check_parameter) {
		case LONGER_PAUSE_TIME:
			modification_of_user_trajectory_when_LONGER_PAUSE_TIME(check_parameter, phase_id);
			break;
		case SHORTER_PAUSE_TIME:
			modification_of_user_trajectory_when_SHORTER_PAUSE_TIME(check_parameter, phase_id);
			break;
		case PATH:
			modification_of_user_trajectory_when_PATH(check_parameter, phase_id);
			break;
		case FASTER_SPEED:
			modification_of_user_trajectory_when_FASTER_SPEED(check_parameter, phase_id);
			break;
		case SLOER_SPEED:
			modification_of_user_trajectory_when_SLOER_SPEED(check_parameter, phase_id);
			break;
		case VISIT_POI:
			modification_of_user_trajectory_when_VISIT_POI(check_parameter, phase_id);
			break;
		}
	}

	///<summary>
	/// ユーザの停止時間が予定より長い時のpredicted_userの修正
	///</summary>
	void KatoMasterMethod::modification_of_user_trajectory_when_LONGER_PAUSE_TIME(ChangeParameter check_parameter, int phase_id)
	{
		if (Tu == requirement->service_interval) {
			//trajectoryをずらすことで対応
			predicted_user->get_trajectory()->insert_positions_to_trajectory(phase_id, 1);
			
			//停止時間の修正を行う．
			predicted_user->revise_now_pause_time(phase_id, Tu);
		}
		else {
			//停止時間の修正を行う．
			predicted_user->revise_now_pause_time(phase_id, Tu);

			//経路を再計算
			//recalculation_path(phase_id);
		}
	}


	///<summary>
	/// ユーザの停止時間が予定より短い時のpredicted_userの修正
	///</summary>
	void KatoMasterMethod::modification_of_user_trajectory_when_SHORTER_PAUSE_TIME(ChangeParameter check_parameter, int phase_id)
	{
		if (Tu == requirement->service_interval) {
			//trajectoryをずらすことで対応
			predicted_user->get_trajectory()->delete_positions_to_trajectory(phase_id, 1);

			//停止時間の修正を行う．
			predicted_user->revise_now_pause_time(phase_id, Tu);
		}
		else {
			//経路を再計算
		}
	}

	///<summary>
	/// ユーザのパスの修正
	///</summary>
	void KatoMasterMethod::modification_of_user_trajectory_when_PATH(ChangeParameter check_parameter, int phase_id)
	{

	}

	///<summary>
	/// ユーザの移動速度が予定より早い時の修正
	///</summary>
	void KatoMasterMethod::modification_of_user_trajectory_when_FASTER_SPEED(ChangeParameter check_parameter, int phase_id)
	{
		double real_user_speed = 0.0;

	}

	///<summary>
	/// ユーザの移動速度が予定より遅い時の修正
	///</summary>
	void KatoMasterMethod::modification_of_user_trajectory_when_SLOER_SPEED(ChangeParameter check_parameter, int phase_id)
	{

	}


	///<summary>
	/// ユーザの停止位置の修正
	///</summary>
	void KatoMasterMethod::modification_of_user_trajectory_when_VISIT_POI(ChangeParameter check_parameter, int phase_id)
	{

	}



	///<summary>
	/// ダミーの行動プランを修正する
	///</summary>
	void KatoMasterMethod::revise_dummy_trajectory(int phase_id)
	{
		//poiに止まっている時
		if (revising_dummy->check_pause_condition(phase_id)) {
			//全ての停止地点の到着時間を変更し，Tu分変更させる．
			revise_dummy_pause_time(phase_id);
			if (Tu != 0.0) revise_dummy_path(phase_id);
			if (Tu != 0.0) revise_dummy_speed(phase_id);
			if (Tu != 0.0) revise_dummy_visit_poi(phase_id);
		}
		//現在移動中で，プランと同じpoiに向かっている場合，向かっているpoiに停止中だとして考える
		else {
			//全ての停止地点の到着時間を変更し，Tu分変更させる．
			revise_dummy_pause_time(phase_id);
			revise_dummy_path(phase_id);
			revise_dummy_speed(phase_id);
			if (Tu != 0) revise_dummy_visit_poi(phase_id);

		}

		//visited_poi情報の更新
		update_visited_pois_info_of_dummy();
	}


	///<summary>
	/// ダミーの停止時間の修正
	/// 許容範囲まで修正を試みる
	/// もし調整しきれなかったら，次のpathの調整をする
	///</summary>
	void KatoMasterMethod::revise_dummy_pause_time(int phase_id)
	{
		int changed_poi_num_id = 1;//停止時間が変更されたpoiの数を記録するためのid

		//変更前のtrajectoryを保持


		//全停止時間分，ダミーの停止時間を修正する．
		for (int i = revising_dummy->get_visited_pois_info_list_id(); i <= revising_dummy->get_visited_pois_num(); i++, changed_poi_num_id++)
		{
			//停止中or向かっているPOIの停止時間を取得
			double pause_time = revising_dummy->get_any_poi_pause_time(i);
			double revise_phase = changed_poi_num_id == 1 ? phase_id : revising_dummy->get_any_arrive_phase(i);

			//最大変化量
			double max_variable_value = calc_max_variable_pause_time(pause_time).first;
			double min_variable_value = calc_max_variable_pause_time(pause_time).second;

			//変更時間．初期値は，変更分Tu
			double change_time = Tu;

			//修正幅 ＜ 最大変化量　を満たし，かつ，修正幅 < 最大停止時間とする．
			if (Tu > 0) {
				if (Tu > max_variable_value) change_time = max_variable_value;
				if (max_variable_value + pause_time > requirement->max_pause_time) change_time = requirement->max_pause_time - pause_time;
			}
			else {
				if (std::abs(Tu) < min_variable_value) change_time = min_variable_value;
				if (min_variable_value + pause_time < requirement->min_pause_time) change_time = pause_time - requirement->min_pause_time;
			}

			double new_pause_time = change_time + revising_dummy->get_pause_time();
			
			//change_timeを現在の残り停止時間に記録
			revising_dummy->add_now_pause_time(revise_phase, change_time);
			//停止時間を登録
			revising_dummy->revise_pause_time(new_pause_time);
			
			Tu -= change_time;
			if (Tu == 0.0) break;
		}

		//停止時間が変更されたPOIの数だけ，次の経路を再計算
		for (int k = revising_dummy->get_visited_pois_info_list_id(); k < changed_poi_num_id; k++) {
			recalculation_path(revising_dummy->get_any_poi(k).first, revising_dummy->get_any_poi(k +1).first, phase_id);
		}

		//変更されなかった分はコピーで対応

		
	}
	

	///<summary>
	/// ダミーの移動経路の修正
	///</summary>
	void KatoMasterMethod::revise_dummy_path(int phase_id)
	{
		
	}


	///<summary>
	/// ダミーの行動速度の修正
	///</summary>
	void KatoMasterMethod::revise_dummy_speed(int phase_id)
	{
		int changed_poi_num_id = 1;//停止時間が変更されたpoiの数を記録するためのid

		//全停止時間分，ダミーの停止時間を修正する．
		for (int i = revising_dummy->get_visited_pois_info_list_id(); i <= revising_dummy->get_visited_pois_num(); i++, changed_poi_num_id++)
		{
			int changed_poi_num_id = 1;//停止時間が変更されたpoiの数を記録するためのid
						
			//停止中or向かっているPOIの停止時間を取得
			double distance = map->shortest_distance(revising_dummy->get_any_poi(i).first, revising_dummy->get_any_poi(i + 1).first);
			int end_pause_phase_of_now_poi = revising_dummy->get_pause_phases().back() + 1;
			double new_speed = distance / (time_manager->time_of_phase(revising_dummy->get_any_arrive_phase(i + 1)) + Tu - time_manager->time_of_phase(end_pause_phase_of_now_poi));
			double prev_speed = revising_dummy->get_starting_speed();
				
			//速度の変化分
			double variable_speed = new_speed - prev_speed;

			//最大変化量・最小変化量
			double max_variable_speed = calc_max_variable_speed(new_speed).first;
			double min_variable_speed = calc_max_variable_speed(new_speed).second;
			//最大最小速度
			double max_speed = requirement->average_speed_of_dummy + 0.5 * requirement->speed_range_of_dummy;
			double min_speed = requirement->average_speed_of_dummy - 0.5 * requirement->speed_range_of_dummy;

			//変更速度．初期値は変更分
			double change_speed = new_speed;

			//修正幅 ＜ 最大変化量　を満たし，かつ，修正幅 < 最大速度とする．
			if (variable_speed > 0) {
				if (variable_speed > max_variable_speed) change_speed = new_speed + max_variable_speed;
				if (change_speed > max_speed) change_speed = max_speed;
			}
			else {
				if (std::abs(variable_speed) < min_variable_speed) change_speed = new_speed + min_variable_speed;
				if (change_speed < min_speed) change_speed = min_speed;
			}
			//change_timeを現在の残り停止時間に記録
			revising_dummy->revise_starting_speed(change_speed);
			//移動速度を登録
			revising_dummy->revise_now_speed(phase_id, change_speed);
			change_speed -= new_speed;
			if (change_speed == 0.0) break;		
		}

		//移動速度が変更されたPOIの数だけ，次の経路を再計算
		for (int k = revising_dummy->get_visited_pois_info_list_id(); k < changed_poi_num_id; k++) {
			recalculation_path(revising_dummy->get_any_poi(k).first, revising_dummy->get_any_poi(k + 1).first, phase_id);
		}
	}


	///<summary>
	/// ダミーの停止位置の修正
	///</summary>
	void KatoMasterMethod::revise_dummy_visit_poi(int phase_id)
	{

	}
	

	///<summary>
	/// ダミーの停止位置の修正
	/// 1:現在の残り停止時間を基に，停止phaseを埋める
	/// 2:
	///</summary>
	void KatoMasterMethod::recalculation_path(const Graph::MapNodeIndicator& source, const Graph::MapNodeIndicator& destination, int phase_id)
	{
		double rest_pause_time = revising_dummy->get_now_pause_time(phase_id);
		lldiv_t variable_of_converted_pause_time_to_phase = std::lldiv(rest_pause_time, requirement->service_interval);


		//現在phaseから，now_puase_timeが0以下になるまで，停止情報を登録
		for (int i = 0; i < variable_of_converted_pause_time_to_phase.quot; i++)
		{
			phase_id++;
			rest_pause_time -= requirement->service_interval;
			revising_dummy->set_now_pause_time(phase_id, rest_pause_time);
			revising_dummy->set_position_of_phase(phase_id, source, map->get_static_poi(source.id())->data->get_position());
		}

		std::vector<Graph::MapNodeIndicator> shortests_path_between_pois = map->get_shortest_path(source, destination);
		std::vector<Graph::MapNodeIndicator>::iterator path_iter = shortests_path_between_pois.begin();//pathを検索するためのindex

		//sourceからの距離
		//最初だけ停止時間をphaseに換算した時の余りをtimeとし，それ以外はservice_intervalをtimeとして，現在地から求めたい地点のdistanceを計算
		//速度はphaseで埋める前を参照しなければならないことに注意
		double now_time = requirement->service_interval - revising_dummy->get_now_pause_time(phase_id);
		double pause_position_speed = revising_dummy->get_starting_speed_using_pause_phase(phase_id);

		double total_time_from_source_to_destination = map->calc_necessary_time(source, destination, pause_position_speed);
		Graph::MapNodeIndicator nearest_position = source;
		//pathを作成．場所は一番近いintersection同士で線形補間する．MapNodeIndicatorのTypeはINVALIDとする．
		while (now_time < total_time_from_source_to_destination)
		{
			//最初は停止時間をphaseに換算したときの余り分をdistanceとして，最短路の中で一番近いintersectionを探し，線形補間する．
			//double total_path_length = map->shortest_distance(source, *path_iter);
			while (now_time > map->calc_necessary_time(source, *path_iter, pause_position_speed))
			{
				nearest_position = *path_iter;
				path_iter++;
			}

			double distance = now_time * pause_position_speed;
			double distance_between_nearest_intersection_and_arrive_position = distance - map->shortest_distance(source, nearest_position);
			Geography::LatLng nearest_latlng
				= nearest_position.type() == Graph::NodeType::POI ? map->get_static_poi(nearest_position.id())->data->get_position() : *map->get_static_node(nearest_position.id())->data;
			Geography::LatLng next_nearest_latlang
				= (*path_iter).type() == Graph::NodeType::POI ? map->get_static_poi((*path_iter).id())->data->get_position() : *map->get_static_node((*path_iter).id())->data;
			double angle = Geography::GeoCalculation::lambert_azimuth_angle(nearest_latlng, next_nearest_latlang);

			Geography::LatLng arrive_position = Geography::GeoCalculation::calc_translated_point(nearest_latlng, distance_between_nearest_intersection_and_arrive_position, angle);

			if (phase_id == time_manager->phase_count() - 1) return;//残りのpathを決める時の終了条件
			(phase_id)++;
			revising_dummy->set_position_of_phase(phase_id, Graph::MapNodeIndicator(Graph::NodeType::OTHERS, Graph::NodeType::OTHERS), arrive_position);
			revising_dummy->set_now_speed(phase_id, pause_position_speed);

			now_time += requirement->service_interval;
		}

		//destinationのところまで補完できたら，rest_timeを保持しておく！
		double time_between_arrive_position_and_dest_position = now_time - total_time_from_source_to_destination;
		double dest_rest_time
			= time_between_arrive_position_and_dest_position == requirement->service_interval ? 0 : time_between_arrive_position_and_dest_position;
		//目的地の登録
		//speedは別途設定のため不要
		(phase_id)++;
		revising_dummy->set_position_of_phase(phase_id, destination, map->get_static_poi(destination.id())->data->get_position());

	}

	///<summary>
	/// ダミーの訪問POI情報を更新する
	///</summary>
	void KatoMasterMethod::update_visited_pois_info_of_dummy()
	{
		
	}

	
	///<summary>
	/// 初期化
	///</summary>
	void KatoMasterMethod::initialize()
	{
		//ユーザの動きの変更→新しく作る．
		predicted_user = copy_predicted_user_plan(input_user);
	}


	///<summary>
	/// ここが提案手法の核になる部分.ダミーの行動を修正する
	///</summary>
	void KatoMasterMethod::revise_dummy_positions()
	{
		for (size_t dummy_id = 1; dummy_id <= entities->get_dummy_count(); dummy_id++)
		{
			revising_dummy = entities->get_dummy_by_id(dummy_id);
			for (int phase_id = 0; phase_id < time_manager->phase_count(); phase_id++)
			{
				//visited_pois_info_list_idの更新
				entities->for_each_dummy([=](entity_id dummy_id, std::shared_ptr<Entity::RevisablePauseMobileEntity<Geography::LatLng>>& dummy)
				{
					if ((phase_id + 1) == dummy->get_pause_phases().back()) dummy->increment_visited_pois_info_list_id();
				});
				if (check_going_same_poi_as_plan()) {
					if (check_user_plan(phase_id) != NO_CHANGE) {
						revise_dummy_trajectory(phase_id);//dummyの行動プランの更新
						update_user_plan(check_user_plan(phase_id), phase_id);//次の停止地点の到着時間を予測し，ユーザの行動プランを更新
					}
				}
			}
		}
	}

	void KatoMasterMethod::run()
	{
		//ここで実行時間の計測を開始
		timer->start();

		//初期化.行動変更後のユーザの生成
		initialize();

		//ここが実行部分(各時刻のダミー位置を計算する)(加藤さん卒論手法)[Kato 13]
		decide_dummy_positions();

		//ここでユーザの行動の予測やダミーの行動を修正する(加藤さん修論手法)[Kato 14]
		//revise_dummy_positions();

		//ここで計測を終了
		timer->end();

		//終了処理
		terminate();
	}

}
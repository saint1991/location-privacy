#include "stdafx.h"
#include "KatoMasterMethod.h"

namespace Method
{

	///<summary>
	/// コンストラクタ
	/// これにSimulatorで作成した各種入力への参照を渡す
	///</summary>
	KatoMasterMethod::KatoMasterMethod(std::shared_ptr<Map::BasicDbMap const> map, std::shared_ptr<Entity::PauseMobileEntity<Geography::LatLng>> user, std::shared_ptr<Requirement::KatoMethodRequirement const> requirement, std::shared_ptr<Time::TimeSlotManager> time_manager)
		:KatoBachelorMethod(map, user, requirement, time_manager),
		predicted_user(nullptr)
	{
	}

	///<summary>
	/// デストラクタ
	///</summary>
	KatoMasterMethod::~KatoMasterMethod()
	{
	}
	

	///<summary>
	/// ダミーの行動プランを修正する
	///</summary>
	void KatoMasterMethod::revise_dummy_movement_plan(int phase_id)
	{

	}


	///<summary>
	/// ダミーの停止時間の修正
	///</summary>
	void KatoMasterMethod::revise_dummy_pause_time(int phase_id)
	{
		/*
		//前の値の保持
		time_t previous_pause_time = creating_dummy->get_pause_time(phase_id);
		time_t previous_arrive_time = time_manager->time_of_phase(phase_id + 1);

		if (std::abs(time_to_change) > requirement->max_variation_of_pause_time)
		{
			time_t new_pause_time = time_to_change > 0 ? previous_pause_time + requirement->max_variation_of_pause_time : previous_pause_time - requirement->max_variation_of_pause_time;
			creating_dummy->set_pause_time(phase_id, new_pause_time);
		}
		else
		{
			creating_dummy->set_pause_time(phase_id, previous_pause_time + time_to_change);
		}

		if (requirement->max_pause_time < time_manager->time_of_phase(phase_id)) creating_dummy->set_pause_time(phase_id, requirement->max_pause_time);
		if (requirement->min_pause_time > time_manager->time_of_phase(phase_id)) creating_dummy->set_pause_time(phase_id, requirement->min_pause_time);

		//停止時間の変化量を求める
		time_t variation_of_pause_time = creating_dummy->get_pause_time(phase_id) - previous_pause_time;

		for (int i = phase_id + 1; i <= time_manager->phase_count(); i++)
		{
			//revise_time_manager->time_of_phase(i) += variation_of_pause_time;
		}
		if (time_manager->time_of_phase(phase_id + 1) == previous_arrive_time + time_to_change) return;

		time_to_change -= variation_of_pause_time;

		*/
	}

	
	///<summary>
	/// ダミーの移動経路の修正
	///</summary>
	void KatoMasterMethod::revise_dummy_trajectory(int phase_id)
	{
	/*
		time_t previous_arrive_time = time_manager->time_of_phase(phase_id + 1);
		time_t tempT = 10000000000000000000;

		while (全てのトラジェクトリをチェックし終えるまで)
		{
		phase_idのときの存在地点;
		time_manager->time_of_phase(phase_id + 1) = time_manager->time_of_phase(phase_id) + creating_dummy->get_pause_time(phase_id) + distance_of_Tri / creating_dummy->get_speed(phase_id);

		}
	*/
	}


	///<summary>
	/// ダミーの行動速度の修正
	///</summary>
	void KatoMasterMethod::revise_dummy_speed(int phase_id)
	{
		/*
		time_t previous_arrive_time = time_manager->time_of_phase(phase_id + 1);
		double previous_speed = creating_dummy->get_speed(phase_id);
		double distance = map->shortest_distance(creating_dummy->read_node_pos_info_of_phase(phase_id).first, creating_dummy->read_node_pos_info_of_phase(phase_id).first);
		time_t time = distance / (time_manager->time_of_phase(phase_id + 1) + time_to_change - time_manager->time_of_phase(phase_id));
		creating_dummy->set_speed(phase_id, time);

		if (std::abs(creating_dummy->get_speed(phase_id) - previous_speed) > requirement->max_variation_of_speed)
		{
			double new_speed = creating_dummy->get_speed(phase_id) - previous_speed > 0 ? previous_speed + requirement->max_variation_of_speed : previous_speed - requirement->max_variation_of_speed;
			creating_dummy->set_pause_time(phase_id, new_speed);
		}
		double max_speed = requirement->average_speed_of_dummy + 0.5* requirement->speed_range_of_dummy;
		double min_speed = requirement->average_speed_of_dummy - 0.5* requirement->speed_range_of_dummy;
		if (max_speed < creating_dummy->get_speed(phase_id)) creating_dummy->set_speed(phase_id, max_speed);
		if (min_speed > creating_dummy->get_speed(phase_id)) creating_dummy->set_speed(phase_id, min_speed);

		//time_manager->time_of_phase(phase_id + 1) = time_manager->time_of_phase(phase_id) + creating_dummy->get_pause_time(phase_id) + (time_t)(distance / creating_dummy->get_speed(phase_id));
		time_t variation_of_arrive_time = time_manager->time_of_phase(phase_id + 1) - previous_arrive_time;

		for (int i = phase_id + 1; i <= time_manager->phase_count(); i++)
		{
			//revise_time_manager->time_of_phase(i) += variation_of_pause_time;
		}
		if (time_manager->time_of_phase(phase_id + 1) == previous_arrive_time + time_to_change) return;
		*/
	}


	///<summary>
	/// ダミーの停止位置の修正
	///</summary>
	void KatoMasterMethod::revise_dummy_pause_position(int phase_id)
	{

	}

	///<summary>
	/// ユーザの行動プランに含まれる停止地点に向かっているかどうかをチェック
	///</summary>
	bool KatoMasterMethod::check_going_pause_position_in_plan()
	{
		return true;
	}

	///<summary>
	/// ユーザの行動プラン変更の判断
	/// real_userとpredict_userの違いで見る
	/// ユーザがプラン通りに行動している場合は0をリターン
	///</summary>
	int KatoMasterMethod::check_user_plan()
	{
		if (!check_user_pause_time()) { return 1; }
		else if (!check_user_speed()) { return 2; }
		else if (!check_user_path()) { return 3; }
		else if (!check_user_position()) { return 4; }
		else { return 0; }
	}

	///<summary>
	/// ユーザの行動プラン変更の判断
	/// real_userとpredict_userの違いで見る
	/// pause_timeのチェック
	///</summary>
	bool KatoMasterMethod::check_user_pause_time()
	{
		return true;
	}

	///<summary>
	/// ユーザの行動プラン変更の判断
	/// real_userとpredict_userの違いで見る
	/// 速度のチェック
	///</summary>
	bool KatoMasterMethod::check_user_speed()
	{
		return true;
	}

	///<summary>
	/// ユーザの行動プラン変更の判断
	/// real_userとpredict_userの違いで見る
	/// pathのチェック
	///</summary>
	bool KatoMasterMethod::check_user_path()
	{
		return true;
	}


	///<summary>
	/// ユーザの行動プラン変更の判断
	/// real_userとpredict_userの違いで見る
	/// 停止位置のチェック
	///</summary>
	bool KatoMasterMethod::check_user_position()
	{
		return true;
	}

	///<summary>
	/// input_user_planからユーザの行動を予測し，そのユーザを返す．
	///</summary>
	std::shared_ptr<Entity::PauseMobileEntity<Geography::LatLng>> KatoMasterMethod::predict_user_plan(std::shared_ptr<Entity::PauseMobileEntity<Geography::LatLng>> input_user_plan)
	{


		return nullptr;
	}


	///<summary>
	/// ユーザの次の停止地点の到着時間を予測する
	///</summary>
	void KatoMasterMethod::predict_user_next_pause_position_time(int check_num)
	{

	}
	///<summary>
	/// ユーザの行動プランをアップデートする
	///</summary>
	void KatoMasterMethod::update_user_plan()
	{

	}



	///<summary>
	/// 初期化 (今回は特にやることはない)
	///</summary>
	void KatoMasterMethod::initialize()
	{
		//ユーザの動きの変更→新しく作る．
	}


	///<summary>
	/// ここが提案手法の核になる部分.ダミーの行動を修正する
	///</summary>
	void KatoMasterMethod::revise_dummy_positions()
	{
		/*
		time_t time_to_change = 0;// (修正後の)time_manager->time_of_phase(phase_id) - (修正前の)time_manager->time_of_phase(phase_id);
		predict_user = predict_user_plan(entities->get_user());

		for (int phase_id = 0; phase_id < time_manager->phase_count(); phase_id++)
		{
		if (check_going_pause_position_in_plan()) {
		if (check_user_plan() != 0) {
		predict_user_next_pause_position_time(check_user_plan());//次の停止地点の到着時間を予測
		revise_dummy_movement_plan(phase_id);
		update_user_plan();
		}
		}
		}
		*/

	}

	/*
	///<summary>
	/// 決定した位置を基にMTC等各種評価値を算出する
	///</summary>
	void KatoMasterMethod::evaluate()
	{

	}


	///<summary>
	/// 結果のファイルへのエクスポート
	///</summary>
	void KatoMasterMethod::export_results()
	{

	}


	///<summary>
	/// 終了処理 (今回はスマートポインタを利用しているので，特にやることはない)
	///</summary>
	void KatoMasterMethod::terminate()
	{

	}
	*/

	void KatoMasterMethod::run()
	{
		//ここで実行時間の計測を開始
		timer->start();

		//初期化
		initialize();

		//ここが実行部分(各時刻のダミー位置を計算する)(加藤さん卒論手法)[Kato 13]
		decide_dummy_positions();

		//ここでユーザの行動の予測やダミーの行動を修正する(加藤さん修論手法)[Kato 14]
		revise_dummy_positions();

		//ここで計測を終了
		timer->end();

		//終了処理
		terminate();
	}

}
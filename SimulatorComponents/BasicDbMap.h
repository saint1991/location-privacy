#ifdef SIMULATORCOMPONENTS_EXPORTS
#define BASIC_DB_MAP_API __declspec(dllexport)
#else
#define BASIC_DB_MAP_API __declspec(dllimport)
#endif

#pragma once
#include "Map.h"
#include "BasicMapNode.h"
#include "BasicPoi.h"
#include "BasicRoad.h"
#include "DbNodeCollectionFactory.h"
#include "DbPoiCollectionFactory.h"
#include "WarshallFloyd.h"

namespace Map
{

	///<summary>
	/// DBから読み出して基本的なMapを構成するためのクラス
	/// Tableのフォーマットはそれぞれ
	/// nodes (id, latitude, longitude)
	/// node_connections (id1, id2, distance)で
	/// ※node_connectionsに格納されているコネクションを双方向で構成する
	/// pois (id, latitude, longitude, category_id, category_name, venue_name)
	/// poi_connections (id, to1, to2, distance1, distance2)
	/// を想定
	///</summary>
	class BASIC_DB_MAP_API BasicDbMap : public Graph::Map<BasicMapNode, BasicPoi, BasicRoad>
	{

	protected:
		std::string setting_file_path;
		std::string db_name;
		std::string node_table;
		std::string node_connection_table;
		std::string poi_table;
		std::string poi_connection_table;

		void build_map();

	public:
		BasicDbMap (
			const std::string& setting_file_path,
			const std::string& db_name,
			const std::string& node_table = "nodes",
			const std::string& node_connection_table = "node_connections",
			const std::string& poi_table = "pois",
			const std::string& poi_connection_table = "poi_connections"
		);
		virtual ~BasicDbMap();

		std::vector<std::shared_ptr<BasicPoi const>> find_pois_within_boundary(const Graph::box& boundary, const std::string& category_id) const;
		std::vector<std::shared_ptr<BasicPoi const>> find_pois_within_boundary(const Graph::Rectangle& boundary, const std::string& category_id) const;
	};

}


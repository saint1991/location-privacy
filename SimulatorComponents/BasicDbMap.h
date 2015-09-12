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
	/// DB����ǂݏo���Ċ�{�I��Map���\�����邽�߂̃N���X
	/// Table�̃t�H�[�}�b�g�͂��ꂼ��
	/// nodes (id, latitude, longitude)
	/// node_connections (id1, id2, distance)��
	/// ��node_connections�Ɋi�[����Ă���R�l�N�V������o�����ō\������
	/// pois (id, latitude, longitude, category_id, category_name, venue_name)
	/// poi_connections (id, to1, to2, distance1, distance2)
	/// ��z��
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
	};

}

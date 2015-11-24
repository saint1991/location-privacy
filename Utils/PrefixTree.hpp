
namespace Graph
{

	///<summary>
	/// コンストラクタ
	///</summary>
	template <typename NODE, typename NODE_DATA, typename EDGE>
	PrefixTree<NODE, NODE_DATA, EDGE>::PrefixTree() : node_collection(std::make_shared<Collection::IdentifiableCollection<node_id, NODE>>())
	{
	}


	///<summary>
	///  コピーコンストラクタ
	///</summary>
	template <typename NODE, typename NODE_DATA, typename EDGE>
	PrefixTree<NODE, NODE_DATA, EDGE>::PrefixTree(const PrefixTree& t) : node_collection(std::make_shared<Collection::IdentifiableCollection<node_id, NODE>>())
	{
		t.node_collection->foreach([&](std::shared_ptr<NODE const> node) {
			node_collection->add(std::make_shared<NODE>(*node));
		});
		root_node = std::make_shared<NODE>(*t.root_node);
	}


	///<summary>
	/// デストラクタ
	///</summary>
	template <typename NODE, typename NODE_DATA, typename EDGE>
	PrefixTree<NODE, NODE_DATA, EDGE>::~PrefixTree()
	{

	}


	///<summary>
	/// ルートノードを設定して木を初期化します
	/// ルートノードの親ノードはID-1としている
	///</summary>
	template <typename NODE, typename NODE_DATA, typename EDGE>
	void PrefixTree<NODE, NODE_DATA, EDGE>::initialize(std::shared_ptr<NODE_DATA> root_data)
	{
		node_id current_id = node_collection->size();
		std::shared_ptr<NODE> node = std::make_shared<NODE>(current_id, -1, 0, root_data);
		root_node = node;
		node_collection->add(node);
	}


	///<summary>
	/// rootノードを指している状態でイテレータを取得する
	///</summary>
	template <typename NODE, typename NODE_DATA, typename EDGE>
	template <typename ITER_TYPE>
	ITER_TYPE PrefixTree<NODE, NODE_DATA, EDGE>::root()
	{
		ITER_TYPE ret(root_node->get_id(), node_collection);
		return ret;
	}

	///<summary>
	/// rootノードを指している状態でイテレータを取得する
	///</summary>
	template <typename NODE, typename NODE_DATA, typename EDGE>
	template <typename CONST_ITER_TYPE>
	CONST_ITER_TYPE PrefixTree<NODE, NODE_DATA, EDGE>::const_root() const
	{
		CONST_ITER_TYPE ret(root_node->get_id(), node_collection);
		return ret;
	}


	///<summary>
	/// 終端ノード(nullptr)を指している状態でイテレータを取得する
	///</summary>
	template <typename NODE, typename NODE_DATA, typename EDGE>
	template <typename ITER_TYPE>
	ITER_TYPE PrefixTree<NODE, NODE_DATA, EDGE>::end()
	{
		ITER_TYPE ret(INVALID, nullptr);
		return ret;
	}


	///<summary>
	/// 終端ノード(nullptr)を指している状態でイテレータを取得する
	///</summary>
	template <typename NODE, typename NODE_DATA, typename EDGE>
	template <typename CONST_ITER_TYPE>
	CONST_ITER_TYPE PrefixTree<NODE, NODE_DATA, EDGE>::const_end() const
	{
		CONST_ITER_TYPE ret(INVALID, nullptr);
		return ret;
	}

	///<summary>
	/// 指定したIDを持つノードを指して状態でイテレータを取得する
	///</summary>
	template <typename NODE, typename NODE_DATA, typename EDGE>
	template <typename ITER_TYPE>
	ITER_TYPE PrefixTree<NODE, NODE_DATA, EDGE>::get_iter_by_id(node_id id)
	{
		ITER_TYPE ret(id, node_collection);
		return ret;
	}

	/*	
	///<summary>
	/// イテレータが指している要素の子要素としてデータがnode_dataのノードを挿入しedgeを張る
	/// Insertしたノードを返します
	/// 追加に失敗した場合はnullptrを返します
	///</summary>
	template <typename NODE, typename NODE_DATA, typename EDGE>
	BaseIterator<NODE, NODE_DATA, EDGE> PrefixTree<NODE, NODE_DATA, EDGE>::insert(base_iterator iter, EDGE edge, NODE_DATA node_data)
	{
		std::shared_ptr<NODE> target = *iter;
		if (target == nullptr) return base_iterator(INVALID, nullptr);
		bool result = target->connect_to(std::make_shared<EDGE>(edge));

		node_id current_id = node_collection->size();
		std::shared_ptr<NODE> node = std::make_shared<NODE>(current_id, node_data);
		result = node_collection->add(node) && result;
		return result ? base_iterator(current_id, node_collection) : base_iterator(INVALID, nullptr);
	}
	*/
}
#pragma once
#include "stdafx.h"
#include "PrefixTree.h"
#include "BaseConstIterator.h"

namespace Graph
{

	template <typename NODE, typename NODE_DATA, typename EDGE>
	class BaseIterator : public BaseConstIterator<NODE, NODE_DATA, EDGE>
	{		
	public:
		BaseIterator(node_id id, std::shared_ptr<Collection::IdentifiableCollection<node_id, NODE>> node_collection);
		virtual ~BaseIterator();
		std::shared_ptr<NODE> operator*();
		std::shared_ptr<NODE> operator->();
		BaseIterator<NODE, NODE_DATA, EDGE> find_child_if(const std::function<bool(std::shared_ptr<NODE const>)>& compare);
	};
}

#include "BaseIterator.hpp"
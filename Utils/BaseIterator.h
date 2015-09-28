#pragma once
#include "stdafx.h"
#include "PrefixTree.h"

namespace Graph
{

	template <typename NODE, typename NODE_DATA, typename EDGE>
	class BaseIterator : public std::iterator<std::forward_iterator_tag, NODE>
	{	
	protected:
		std::shared_ptr<NODE> node;
		
	public:
		BaseIterator();
		BaseIterator(std::shared_ptr<NODE> node);
		virtual ~BaseIterator();
		std::shared_ptr<NODE> operator*();
		std::shared_ptr<NODE> operator->();
		BaseIterator<NODE, NODE_DATA, EDGE> find_child_if(const std::function<bool(std::shared_ptr<NODE const>)>& compare);
		bool operator==(const BaseIterator<NODE, NODE_DATA, EDGE>& iter) const;
		bool operator!=(const BaseIterator<NODE, NODE_DATA, EDGE>& iter) const;
	};
}

#include "BaseIterator.hpp"
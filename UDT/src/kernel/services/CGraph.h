/*
 * CGraph.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 16.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CROUTEGRAPH_H_
#define CROUTEGRAPH_H_

namespace NUDT
{

class CRouteGraph
{
public:
	static const NSHARE::CText NAME;
	typedef NSHARE::uuid_t node_t;
	typedef int latency_t;
	typedef std::map<node_t, latency_t> vertexs_t;
	typedef std::vector<node_t> vertexs_array_t;
	struct path_t:vertexs_array_t 
	{
		;
	};
	CRouteGraph();
	 ~CRouteGraph();

	void MAddNode(node_t const& name, const vertexs_t& edges);
	vertexs_t const& MGetNode(node_t const& name);
	vertexs_t MRemoveNode(node_t const& name);
	bool MRemoveEdge(node_t const& name);
	bool MIsEnge(node_t const& name,node_t const& edge) const;

	//void MRemoveNode(node_t const& name, const vertexs_array_t& edges);

	bool MIsVertex(node_t const& name) const;

	void MReset();
	void MResetCache();
	path_t MShortestPath(node_t const& start,
			node_t  const& finish) const;
	NSHARE::CConfig MSerialize() const;
private:

	typedef std::map<node_t, vertexs_t> adjacency_list_t;
	typedef std::map<node_t,latency_t> fast_ways_t;
	typedef std::map<node_t,node_t> roads_t;
	struct path_info
	{
		fast_ways_t FMinDistance;
		roads_t FPrevious;
	};
	typedef std::map<node_t, path_info> pathes_t;


	void MDijkstraComputePathsFor(node_t source) const;

	adjacency_list_t FVertices;
	mutable pathes_t FPathes;
};

} /* namespace NUDT */
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::CRouteGraph::path_t const& aVal)
{
	for (NUDT::CRouteGraph::path_t::const_iterator _it = aVal.begin();
			_it != aVal.end();)
	{
		aStream << _it->MToString();
		if (++_it != aVal.end())
			aStream << " => ";
	}
	return aStream;
}
}
#endif /* CROUTEGRAPH_H_ */

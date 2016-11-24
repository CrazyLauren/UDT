/*
 * CGraph.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 16.02.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <set>
#include <shared_types.h>//msvc bug fix std::vector<uuid_t> is defined in shared_types too
#include "CGraph.h"

namespace NUDT
{
const NSHARE::CText CRouteGraph::NAME = "roads";
const CRouteGraph::latency_t max_weight = std::numeric_limits<
		CRouteGraph::latency_t>::max();

CRouteGraph::CRouteGraph()
{

}

CRouteGraph::~CRouteGraph()
{
}
//name -master node, edges -slave node
void CRouteGraph::MAddNode(node_t const& name, const vertexs_t& edges)
{
	adjacency_list_t::iterator _it = FVertices.find(name);
	LOG_IF(INFO,_it != FVertices.end()) << name << " is exist.";
	VLOG(2) << "Add vertex for " << name;

	//neighbors_t _val;
	for (vertexs_t::const_iterator _jt = edges.begin(); _jt != edges.end();
			++_jt)
	{
		CHECK_GE(_jt->second,0);
		if (FVertices.find(_jt->first) == FVertices.end())
		{
			VLOG(2) << "Insert imidiatle " << _jt->first;
			FVertices.insert(std::make_pair(_jt->first, vertexs_t()));
		}
	}
	FVertices[name].insert(edges.begin(),edges.end());
	MResetCache();
}
CRouteGraph::vertexs_t const& CRouteGraph::MGetNode(node_t const& name)
{
	adjacency_list_t::iterator _it = FVertices.find(name);
	LOG_IF(FATAL,_it == FVertices.end()) << name << " is not exist.";
	return _it->second;
}
void CRouteGraph::MDijkstraComputePathsFor(node_t source) const
{
	VLOG(2) << "Source: " << source;
	path_info _info;
	_info.FMinDistance[source] = 0;

	std::set<std::pair<latency_t, node_t> > vertex_queue;
	vertex_queue.insert(std::make_pair(_info.FMinDistance[source], source));

	while (!vertex_queue.empty())
	{
		latency_t const _dist = vertex_queue.begin()->first;
		node_t const _u = vertex_queue.begin()->second;
		vertex_queue.erase(vertex_queue.begin());

		VLOG(2) << "Exist u :" << _u;
		// Visit each edge exiting u
		adjacency_list_t::const_iterator _neighbors = FVertices.find(_u);
		for (vertexs_t::const_iterator neighbor_iter =
				_neighbors->second.begin();
				neighbor_iter != _neighbors->second.end(); ++neighbor_iter)
		{
			node_t const _v = neighbor_iter->first;
			latency_t const _weight = neighbor_iter->second;
			latency_t const _distance_through_u = _dist + _weight;
			CHECK_GE(_distance_through_u,0);
			CHECK_GE(_weight,0);
			VLOG(2) << "Distance through:" << _distance_through_u<<" v "<<_v<<" weight="<<_weight;

			std::map<node_t, latency_t>::iterator _it = _info.FMinDistance.find(
					_v);
			if ((_it == _info.FMinDistance.end())
					|| (_distance_through_u < _it->second))
			{
				if (_it != _info.FMinDistance.end())
				{
					VLOG(2) << "Erase distance " << _it->first << " lat="
										<< _it->second;
					vertex_queue.erase(std::make_pair(_it->second, _v));
				}

				_it = _info.FMinDistance.insert(_it,
						std::make_pair(_v, _distance_through_u));
				VLOG(2) << "inserting previous for " << _v << " as " << _u;
				_info.FPrevious[_v] = _u;
				vertex_queue.insert(std::make_pair(_it->second, _v));
			}

		}
	}
	FPathes[source] = _info;
}
CRouteGraph::vertexs_t CRouteGraph::MRemoveNode(node_t const& name)
{
	VLOG(2) << "Remove node " << name;
	adjacency_list_t::iterator _it = FVertices.find(name);
	LOG_IF(INFO,_it == FVertices.end()) << "Vertex " << name
												<< " is not exist.";
	vertexs_t _vertex;
	if (_it != FVertices.end())
	{
		_vertex.swap(_it->second);

		//remove all edge
		{
			adjacency_list_t::iterator _jt = FVertices.begin(), _jt_end =
					FVertices.end();

			for (; _jt != _jt_end; ++_jt)
			{
				_jt->second.erase(_it->first);
			}
		}
		FVertices.erase(_it);
		MResetCache();
	}
	return _vertex;
}
void CRouteGraph::MReset()
{
	FVertices.clear();
	FPathes.clear();
}
void CRouteGraph::MResetCache()
{
	FPathes.clear();
}

CRouteGraph::path_t CRouteGraph::MShortestPath(node_t const& start,
		node_t finish) const
{
	VLOG(2) << "Shortest path from " << start << " to " << finish;

	DCHECK(FVertices.find(start) != FVertices.end());
	DCHECK(FVertices.find(finish) != FVertices.end());

	path_t _path;
	pathes_t::const_iterator _it = FPathes.find(start);
	if (_it == FPathes.end())
	{
		MDijkstraComputePathsFor(start);
		_it = FPathes.find(start);
	}
	DCHECK(_it != FPathes.end());

	if (_it == FPathes.end())
		return _path;

	VLOG(4) << MSerialize().MToJSON(true);

	std::map<node_t, node_t> const& _prev = _it->second.FPrevious;
	std::map<node_t, node_t>::const_iterator _jt = _prev.find(finish);

	node_t _val(finish);
	for (; _jt != _prev.end() &&_val!= start;)
	{
		VLOG(2) << "Push " << _val;
		_path.push_back(_val);
		_val = _jt->second;
		VLOG(2) << "Next " << _val;
		_jt = _prev.find(_val);
	}
	std::reverse(_path.begin(), _path.end());
	return _path;
}
bool CRouteGraph::MIsEnge(node_t const& name, node_t const& edge) const
{
	adjacency_list_t::const_iterator _it = FVertices.find(name);
	DCHECK(_it!=FVertices.end());
	if (_it == FVertices.end())
	{
		VLOG(2)<<"vertex "<<name<<" is not exist";
		return false;
	}
	return _it->second.find(edge)!= _it->second.end();
}
bool CRouteGraph::MIsVertex(node_t const& name) const
{
	return FVertices.find(name) != FVertices.end();
}
NSHARE::CConfig CRouteGraph::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	adjacency_list_t::const_iterator _it = FVertices.begin(), _it_end =
			FVertices.end();

	for (; _it != _it_end; ++_it)
	{
		NSHARE::CConfig _info("road");
		_info.MAdd("from",_it->first);
		{
			vertexs_t::const_iterator _kt = _it->second.begin(), _kt_end =
					_it->second.end();
			for (; _kt != _kt_end; ++_kt)
			{
				NSHARE::CConfig _way("way");
				_way.MAdd("to",_kt->first);
				_way.MAdd("lat",_kt->second);
				_info.MAdd(_way);
			}
		}
		{
			pathes_t::const_iterator _jt = FPathes.find(_it->first);
			if (_jt != FPathes.end())
			{
				{
					fast_ways_t::const_iterator _rt =
							_jt->second.FMinDistance.begin(), _rt_end =
							_jt->second.FMinDistance.end();
					for (; _rt != _rt_end; ++_rt)
					{
						NSHARE::CConfig _way("time");
						_way.MAdd("to", _rt->first);
						_way.MAdd("lat", _rt->second);
						_info.MAdd(_way);
					}
				}
				{
					roads_t::const_iterator _rt = _jt->second.FPrevious.begin(),
							_rt_end = _jt->second.FPrevious.end();
					for (; _rt != _rt_end; ++_rt)
					{
						NSHARE::CConfig _way("go");
						_way.MAdd("to", _rt->first);
						_way.MAdd("from", _rt->second);
						_info.MAdd(_way);
					}
				}
			}

		}
		_conf.MAdd(_info);
	}
	return _conf;
}
} /* namespace NUDT */

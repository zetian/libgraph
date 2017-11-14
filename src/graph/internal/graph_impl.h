/* 
 * graph_impl.h
 * 
 * Created on: Nov 13, 2017 23:08
 *  Description:
 *  	1. A visualized illustration of the graph structure
 *
 *  	Graph "G":
 *  		Vertex "V1" - Edge "V1_E1", which connects "V1" to "Vx1"
 *  				    - Edge "V1_E2", which connects "V1" to "Vx2"
 *  				  			...
 *  		Vertex "V2" - Edge "V2_E1", which connects "V2" to "Vx3"
 *  				    - Edge "V2_E2", which connects "V2" to "Vx4"
 *  				  			...
 *  			...
 *
 *  		Vertex "Vx"			...
 *		2. Refer to documentation for instructions of how to use this library
 * 
 * Copyright (c) 2017 Ruixiang Du (rdu)
 */

#ifndef GRAPH_IMPL_H
#define GRAPH_IMPL_H

#include <map>
#include <vector>
#include <cstdint>
#include <type_traits>

#include "graph/internal/bds_base.h"
#include "graph/internal/vertex_impl.h"

namespace librav {

/****************************************************************************/
/*								 Graph										*/
/****************************************************************************/
/// A graph data structure template.
template<typename BundledStructType>
class Graph
{
public:
	/// Graph constructor.
	Graph(){};
	/// Graph destructor. Graph class is only responsible for the memory recycling of Vertex and Edge
	/// objects. The node, such as a quadtree node or a square cell, which each vertex is associated
	///  with needs to be recycled separately, for example by the quadtree/square_grid class.
	~Graph(){
		for(auto it = vertex_map_.begin(); it != vertex_map_.end(); it++)
			delete it->second;
	};

private:
	std::map<uint64_t, Vertex<BundledStructType>*> vertex_map_;

	friend class AStar;

private:
	/// This function checks if a vertex already exists in the graph.
	///	If yes, the functions returns the pointer of the existing vertex,
	///	otherwise it creates a new vertex.
	template<class T = BundledStructType, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
	Vertex<BundledStructType>* GetVertex(BundledStructType vertex_node)
	{
		auto it = vertex_map_.find((uint64_t)(vertex_node.data_id_));

		if(it == vertex_map_.end())
		{
			Vertex<BundledStructType>* new_vertex = new Vertex<BundledStructType>(vertex_node);
			//vertex_map_[vertex_node.data_id_] = new_vertex;
			vertex_map_.insert(std::make_pair(vertex_node.data_id_, new_vertex));
			return new_vertex;
		}

		return it->second;
	}

	template<class T = BundledStructType, typename std::enable_if<std::is_pointer<T>::value>::type* = nullptr>
	Vertex<BundledStructType>* GetVertex(BundledStructType vertex_node)
	{
		auto it = vertex_map_.find((uint64_t)(vertex_node->data_id_));

		if(it == vertex_map_.end())
		{
			Vertex<BundledStructType>* new_vertex = new Vertex<BundledStructType>(vertex_node);
			//vertex_map_[vertex_node->data_id_] = new_vertex;
			vertex_map_.insert(std::make_pair(vertex_node->data_id_, new_vertex));
			return new_vertex;
		}

		return it->second;
	}

	/// This function creates a vertex in the graph that associates with the given node.
	/// The set of functions AddVertex() are only supposed to be used with incremental a* search.
	template<class T = BundledStructType, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
	Vertex<BundledStructType>* AddVertex(BundledStructType vertex_node)
	{
		Vertex<BundledStructType>* new_vertex = new Vertex<BundledStructType>(vertex_node);
		//vertex_map_[vertex_node.data_id_] = new_vertex;
		vertex_map_.insert(std::make_pair(vertex_node.data_id_, new_vertex));
		return new_vertex;
	}

	template<class T = BundledStructType, typename std::enable_if<std::is_pointer<T>::value>::type* = nullptr>
	Vertex<BundledStructType>* AddVertex(BundledStructType vertex_node)
	{
		Vertex<BundledStructType>* new_vertex = new Vertex<BundledStructType>(vertex_node);
		//vertex_map_[vertex_node->data_id_] = new_vertex;
		vertex_map_.insert(std::make_pair(vertex_node->data_id_, new_vertex));
		return new_vertex;
	}

	/// This function checks if a vertex exists in the graph.
	///	If yes, the functions returns the pointer of the existing vertex,
	///	otherwise it returns nullptr.
	template<class T = BundledStructType, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
	Vertex<BundledStructType>* SearchVertex(BundledStructType vertex_node)
	{
		auto it = vertex_map_.find((uint64_t)(vertex_node.data_id_));

		if(it == vertex_map_.end())
			return nullptr;
		else
			return it->second;
	}

	template<class T = BundledStructType, typename std::enable_if<std::is_pointer<T>::value>::type* = nullptr>
	Vertex<BundledStructType>* SearchVertex(BundledStructType vertex_node)
	{
		auto it = vertex_map_.find((uint64_t)(vertex_node->data_id_));

		if(it == vertex_map_.end())
			return nullptr;
		else
			return it->second;
	}

	/// This function is used to reset the vertices for a new search
	void ResetGraphVertices()
	{
		for(const auto& vertex_pair: vertex_map_)
			vertex_pair.second->ClearVertexSearchInfo();
	};

public:
	/// This function removes all edges and vertices in the graph
	void ClearGraph()
	{
		for(auto it = vertex_map_.begin(); it != vertex_map_.end(); it++)
			delete it->second;
		vertex_map_.clear();
	}

	/// This function is used to create a graph by adding edges connecting two nodes
	void AddEdge(BundledStructType src_node, BundledStructType dst_node, double cost)
	{
		Vertex<BundledStructType>* src_vertex = GetVertex(src_node);
		Vertex<BundledStructType>* dst_vertex = GetVertex(dst_node);

		if(src_vertex->CheckNeighbour(dst_vertex))
			return;

		// store information for deleting vertex
		dst_vertex->associated_vertices_.push_back(src_vertex);

		Edge<Vertex<BundledStructType>*> new_edge(src_vertex, dst_vertex, cost);
		src_vertex->edges_.push_back(new_edge);
	};

	/// This function is used to remove the edge from src_node to dst_node.
	bool RemoveEdge(BundledStructType src_node, BundledStructType dst_node)
	{
		Vertex<BundledStructType>* src_vertex = SearchVertex(src_node);
		Vertex<BundledStructType>* dst_vertex = SearchVertex(dst_node);

		if((src_vertex != nullptr) && (dst_vertex != nullptr))
		{
			bool found_edge = false;
			auto idx = src_vertex->edges_.end();

			for(auto it = src_vertex->edges_.begin(); it != src_vertex->edges_.end(); it++)
			{
				if((*it).dst_ == dst_vertex)
				{
					idx = it;
					found_edge = true;
				}
			}

			if(found_edge)
				src_vertex->edges_.erase(idx);

			return found_edge;
		}
		else
			return false;
	};

	/// This function checks if a vertex exists in the graph and remove it if presents.
	template<class T = BundledStructType, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
	void RemoveVertex(BundledStructType vertex_node)
	{
		auto it = vertex_map_.find((uint64_t)(vertex_node.data_id_));

		// unknown vertex, no need to remove
		if(it == vertex_map_.end())
			return;

		for(auto& asv : it->second->associated_vertices_)
			for(auto eit = asv->edges_.begin(); eit != asv->edges_.end(); eit++)
			{
				if((*eit).dst_ == it->second)
				{
					asv->edges_.erase(eit);
					break;
				}
			}

		auto vptr = it->second;
		vertex_map_.erase(it);
		delete vptr;
	}

	template<class T = BundledStructType, typename std::enable_if<std::is_pointer<T>::value>::type* = nullptr>
	void RemoveVertex(BundledStructType vertex_node)
	{
		auto it = vertex_map_.find((uint64_t)(vertex_node->data_id_));

		// unknown vertex, no need to remove
		if(it == vertex_map_.end())
			return;

		for(auto& asv : it->second->associated_vertices_)
			for(auto eit = asv->edges_.begin(); eit != asv->edges_.end(); eit++)
			{
				if((*eit).dst_ == it->second)
				{
					asv->edges_.erase(eit);
					break;
				}
			}

		auto vptr = it->second;
		vertex_map_.erase(it);
		delete vptr;
	}

	/// This functions is used to access all vertices of a graph
	std::vector<Vertex<BundledStructType>*> GetGraphVertices() const
	{
		std::vector<Vertex<BundledStructType>*> vertices;

		for(auto it = vertex_map_.begin(); it != vertex_map_.end(); it++)
		{
			vertices.push_back(it->second);
		}

		return vertices;
	};

	/// This functions is used to access all edges of a graph
	std::vector<Edge<Vertex<BundledStructType>*>> GetGraphEdges() const
	{
		std::vector<Edge<Vertex<BundledStructType>*>> edges;

		for(auto it = vertex_map_.begin(); it != vertex_map_.end(); it++)
		{
			Vertex<BundledStructType>* vertex = it->second;
			for(auto ite = vertex->edges_.begin(); ite != vertex->edges_.end(); ite++) {
				edges.push_back(*ite);
			}
		}

		return edges;
	};

	/// This functions is used to access all edges of a graph
	std::vector<Edge<Vertex<BundledStructType>*>> GetGraphUndirectedEdges() const
	{
		std::vector<Edge<Vertex<BundledStructType>*>> edges;

		for(auto it = vertex_map_.begin(); it != vertex_map_.end(); it++)
		{
			Vertex<BundledStructType>* vertex = it->second;

			for(auto ite = vertex->edges_.begin(); ite != vertex->edges_.end(); ite++) {
				bool edge_existed = false;

				for(auto& itedge : edges)
				{
					if(itedge -= (*ite)) {
						edge_existed = true;
						break;
					}
				}

				if(!edge_existed)
					edges.push_back(*ite);
			}
		}

		return edges;
	};

	/// This function return the vertex with specified id
	Vertex<BundledStructType>* GetVertexFromID(uint64_t vertex_id)
	{
		auto it = vertex_map_.find(vertex_id);

		if(it != vertex_map_.end())
			return (*it).second;
		else
			return nullptr;
	}
};

}

#endif /* GRAPH_IMPL_H */
/* 
 * graph.hpp
 * 
 * Created on: Dec 9, 2015
 * Description: 
 * 
 * Copyright (c) 2017 Ruixiang Du (rdu)
 */

#ifndef GRAPH_HPP
#define GRAPH_HPP

#define USE_UNORDERED_MAP

#ifndef USE_UNORDERED_MAP
#include <map>
#else
#include <unordered_map>
#endif
#include <vector>
#include <cstdint>
#include <type_traits>

#include "graph/edge.hpp"
#include "graph/vertex.hpp"
#include "graph/details/helper_func.hpp"

namespace librav
{

// Only types ended with "_t" should be used in user applications
template <typename StateType, typename TransitionType = double>
class Graph_t;

template <typename StateType, typename TransitionType = double>
class Vertex_t;

template <typename StateType, typename TransitionType = double>
using Edge_t = Edge<Vertex_t<StateType, TransitionType> *, TransitionType>;

template <typename StateType, typename TransitionType = double>
using Path_t = std::vector<Vertex_t<StateType, TransitionType> *>;

/// A graph data structure template.
template <typename StateType, typename TransitionType>
class Graph_t
{
public:
  /// Graph_t constructor.
  /// StateType must provide a function with the signature: int64_t GetUniqueID() const
  Graph_t()
  {
    typedef typename std::remove_const<typename std::remove_reference<typename std::remove_pointer<StateType>::type>::type>::type TestType;
    static_assert(HasIDGenFunc<TestType>::value, "function required in StateType: int64_t GetUniqueID() const");
  }

  /// Graph_t destructor. Graph_t class is only responsible for the memory recycling of Vertex_t and Edge_t
  /// objects. The node, such as a quadtree node or a square cell, which each vertex is associated
  ///  with needs to be recycled separately, for example by the quadtree/square_grid class.
  ~Graph_t();

  typedef Vertex_t<StateType, TransitionType> VertexType;
  typedef Edge_t<StateType, TransitionType> EdgeType;
  typedef std::vector<Vertex_t<StateType, TransitionType> *> PathType;

public:
  /// This function creates a vertex in the graph that associates with the given node.
  template <class T = StateType, typename std::enable_if<std::is_pointer<T>::value>::type * = nullptr>
  VertexType *AddVertex(T vertex_node);

  /// This function checks if a vertex exists in the graph and remove it if presents.
  template <class T = StateType, typename std::enable_if<!std::is_pointer<T>::value>::type * = nullptr>
  void RemoveVertex(T vertex_node);

  /* Directed Graph */
  /// This function is used to create a graph by adding edges connecting two nodes
  void AddEdge(StateType src_node, StateType dst_node, TransitionType cost);

  /// This function is used to remove the edge from src_node to dst_node.
  bool RemoveEdge(StateType src_node, StateType dst_node);

  /// This functions is used to access all vertices of a graph
  std::vector<VertexType *> GetGraphVertices() const;

  /// This functions is used to access all edges of a graph
  std::vector<EdgeType> GetGraphEdges() const;

  /* Undirected Graph */
  /// This function is used to create a graph by adding edges connecting two nodes
  void AddUndirectedEdge(StateType src_node, StateType dst_node, TransitionType cost);

  /// This function is used to remove the edge from src_node to dst_node.
  bool RemoveUndirectedEdge(StateType src_node, StateType dst_node);

  /// This functions is used to access all edges of a graph
  std::vector<EdgeType> GetGraphUndirectedEdges() const;

  /* Utility functions */
  /// This function removes all edges and vertices in the graph
  void ClearGraph();

  /// This function return the vertex with specified id
  VertexType *GetVertexFromID(uint64_t vertex_id);

public:
  /* Same functions for pointer type State node */
  template <class T = StateType, typename std::enable_if<!std::is_pointer<T>::value>::type * = nullptr>
  VertexType *AddVertex(T vertex_node);

  template <class T = StateType, typename std::enable_if<std::is_pointer<T>::value>::type * = nullptr>
  void RemoveVertex(T vertex_node);

private:
#ifndef USE_UNORDERED_MAP
  typedef std::map<uint64_t, VertexType *> VertexMapType;
  typedef VertexMapType::iterator VertexMapTypeIterator;

  std::map<uint64_t, VertexType *> vertex_map_;
#else
  typedef std::unordered_map<uint64_t, VertexType *> VertexMapType;
  typedef typename VertexMapType::iterator VertexMapTypeIterator;

  std::unordered_map<uint64_t, VertexType *> vertex_map_;
#endif

  friend class AStar;
  friend class Dijkstra;

  /// This function is used to reset states of all vertice for a new search
  void ResetGraphVertices();

  /// This function checks if a vertex already exists in the graph.
  ///	If yes, the functions returns the pointer of the existing vertex,
  ///	otherwise it creates a new vertex.
  template <class T = StateType, typename std::enable_if<!std::is_pointer<T>::value>::type * = nullptr>
  VertexType *GetVertex(T vertex_node);

  template <class T = StateType, typename std::enable_if<std::is_pointer<T>::value>::type * = nullptr>
  VertexType *GetVertex(T vertex_node);

  /// This function checks if a vertex exists in the graph.
  ///	If yes, the functions returns the pointer of the existing vertex,
  ///	otherwise it returns nullptr.
  template <class T = StateType, typename std::enable_if<!std::is_pointer<T>::value>::type * = nullptr>
  VertexType *SearchVertex(T vertex_node);

  template <class T = StateType, typename std::enable_if<std::is_pointer<T>::value>::type * = nullptr>
  VertexType *SearchVertex(T vertex_node);

public:
	// define vertex iterator for easy access
  // Reference:
  //  [1] https://stackoverflow.com/questions/1443793/iterate-keys-in-a-c-map
  //  [2] https://stackoverflow.com/questions/1443793/iterate-keys-in-a-c-map/35262398#35262398
  class vertex_iterator : public VertexMapTypeIterator
  {
  public:
    vertex_iterator() : VertexMapTypeIterator(){};
    vertex_iterator(VertexMapTypeIterator s) : VertexMapTypeIterator(s){};
    VertexType *operator->() { return (VertexType *const) (VertexMapTypeIterator::operator->()->second); }
    VertexType &operator*() { return *(VertexMapTypeIterator::operator*().second); }
  };

  vertex_iterator vertex_begin() { return vertex_iterator(vertex_map_.begin()); }
  vertex_iterator vertex_end() { return vertex_iterator(vertex_map_.end()); }
};
}

#include "graph/details/graph_impl.hpp"

#endif /* GRAPH_HPP */

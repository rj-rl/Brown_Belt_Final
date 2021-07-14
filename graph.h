#pragma once

#include <cstdlib>
#include <deque>
#include <vector>
#include <string_view>

template <typename It>
class Range {
public:
    using ValueType = typename std::iterator_traits<It>::value_type;

    Range(It begin, It end) : begin_(begin), end_(end) {}
    It begin() const { return begin_; }
    It end() const { return end_; }

private:
    It begin_;
    It end_;
};

//======================================= Graph ================================================//

namespace Graph {

using VertexId = size_t;
using EdgeId = size_t;

template <typename Weight>
struct Edge {
    VertexId         from   = 0;
    VertexId         to     = 0;
    Weight           weight = 0;
    std::string_view bus_id;
    size_t           span_count = 0;
};

template <typename Weight>
class DirectedWeightedGraph {
private:
    using IncidenceList = std::vector<EdgeId>;
    using IncidentEdgesRange = Range<typename IncidenceList::const_iterator>;

public:
    DirectedWeightedGraph(size_t vertex_count);
    EdgeId AddEdge(const Edge<Weight>& edge);

    size_t GetVertexCount() const;
    size_t GetEdgeCount() const;
    const Edge<Weight>& GetEdge(EdgeId edge_id) const;
    IncidentEdgesRange GetIncidentEdges(VertexId vertex) const;

private:
    std::vector<Edge<Weight>> edges_;
    std::vector<IncidenceList> incidence_lists_;
};

//===================================== Graph Impl =============================================//

template <typename Weight>
DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertex_count) 
    : incidence_lists_(vertex_count) {}

template <typename Weight>
EdgeId DirectedWeightedGraph<Weight>::AddEdge(const Edge<Weight>& edge)
{
    edges_.push_back(edge);
    const EdgeId id = edges_.size() - 1;
    incidence_lists_[edge.from].push_back(id);
    return id;
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::GetVertexCount() const
{
    return incidence_lists_.size();
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::GetEdgeCount() const
{
    return edges_.size();
}

template <typename Weight>
const Edge<Weight>& DirectedWeightedGraph<Weight>::GetEdge(EdgeId edge_id) const
{
    return edges_[edge_id];
}

template <typename Weight>
typename DirectedWeightedGraph<Weight>::IncidentEdgesRange
DirectedWeightedGraph<Weight>::GetIncidentEdges(VertexId vertex) const
{
    const auto& edges = incidence_lists_[vertex];
    return {std::begin(edges), std::end(edges)};
}

}   // ns ::Graph

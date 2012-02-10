#include <boost/config.hpp>
#include <iostream>                         // for std::cout
#include <utility>                          // for std::pair
#include <algorithm>                        // for std::for_each
#include <boost/utility.hpp>                // for boost::tie
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

#include <map> // store the page rank of each node

using namespace boost;

// global update map
std::map<char, float> update_map;

// PR(A) = 1-d + d * sum { PR(N) / out_degree(N) } for each in edge NA
template <class Graph> struct exercise_vertex {
  exercise_vertex(Graph& g_, const char name_[], std::map<char, float>& node_rank_map) 
      : g(g_),name(name_), rank_map(node_rank_map)  { }
  typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
  void operator()(const Vertex& v) const
  {
    using namespace boost;

    typename property_map<Graph, vertex_index_t>::type
      vertex_id = get(vertex_index, g);
    
    std::cout << "vertex: " << name[get(vertex_id, v)] << std::endl;
    
    const float dump_factor = 0.85;
    float total_rank = 1 - dump_factor;
    float part_rank = 0;

    // Write out the incoming edges, for each in-edge, compute the contribution of
    // page rank value to current node
    std::cout << "\tin-edges: ";
	typename graph_traits<Graph>::edge_descriptor e;
    typename graph_traits<Graph>::in_edge_iterator in_i, in_end;
    for (boost::tie(in_i, in_end) = in_edges(v, g); in_i != in_end; ++in_i)
    {
      e = *in_i;
      Vertex src = source(e, g), targ = target(e, g);
      char src_name = name[get(vertex_id, src)];
      float rank = rank_map[src_name];

      // divide the rank by the out edges num
      rank = rank / float(out_degree(src, g));
      part_rank += rank;

      std::cout << "(" << name[get(vertex_id, src)]
                << "," << name[get(vertex_id, targ)] << ") ";
    }
    std::cout << std::endl;
    total_rank += part_rank * dump_factor;

    // update
    update_map[name[get(vertex_id, v)]] = total_rank;
    std::cout << "update rank value for node: " << name[get(vertex_id, v)] << ", and value:" << total_rank << std::endl; 
  }
  Graph& g;
  const char *name;
  std::map<char, float>& rank_map;
};


int main(int,char*[])
{
  // create a typedef for the Graph type
  typedef adjacency_list<vecS, vecS, bidirectionalS,
     no_property, property<edge_weight_t, float> > Graph;

  // Make convenient labels for the vertices
  enum { A, B, C, D, E, N };
  const int num_vertices = N;
  const char name[] = "ABCDE";
  

  // writing out the edges in the graph
  typedef std::pair<int,int> Edge;
  Edge edge_array[] =
  { Edge(A,B), Edge(A,D), Edge(C,A), Edge(D,C),
    Edge(C,E), Edge(B,D), Edge(D,E), };
  const int num_edges = sizeof(edge_array)/sizeof(edge_array[0]);

  // average transmission delay (in milliseconds) for each connection
  float transmission_delay[] = { 1.2, 4.5, 2.6, 0.4, 5.2, 1.8, 3.3, 9.1 };

  // declare a graph object, adding the edges and edge properties
#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
  // VC++ can't handle the iterator constructor
  Graph g(num_vertices);
  property_map<Graph, edge_weight_t>::type weightmap = get(edge_weight, g);
  for (std::size_t j = 0; j < num_edges; ++j) {
    graph_traits<Graph>::edge_descriptor e; bool inserted;
    boost::tie(e, inserted) = add_edge(edge_array[j].first, edge_array[j].second, g);
    weightmap[e] = transmission_delay[j];
  }
#else
  Graph g(edge_array, edge_array + num_edges,
          transmission_delay, num_vertices);
#endif

  boost::property_map<Graph, vertex_index_t>::type
    vertex_id = get(vertex_index, g);
  boost::property_map<Graph, edge_weight_t>::type
    trans_delay = get(edge_weight, g);

  std::cout << "vertices(g) = ";
  typedef graph_traits<Graph>::vertex_iterator vertex_iter;
  std::pair<vertex_iter, vertex_iter> vp;
  for (vp = vertices(g); vp.first != vp.second; ++vp.first)
    std::cout << name[get(vertex_id, *vp.first)] <<  " ";
  std::cout << std::endl;

  std::cout << "edges(g) = ";
  graph_traits<Graph>::edge_iterator ei, ei_end;
  for (boost::tie(ei,ei_end) = edges(g); ei != ei_end; ++ei)
    std::cout << "(" << name[get(vertex_id, source(*ei, g))]
              << "," << name[get(vertex_id, target(*ei, g))] << ") ";
  std::cout << std::endl;

  // Initialization
  std::map<char, float> node_rank_map;
  const char* p = name;
  while (*p != '\0') {
      node_rank_map[*p] = 1.0 / float(N);
      p++;
  }

  const int max_iter = 20;

  for (int i=0; i<max_iter; ++i) {
      std::cout << "=====The " << i << " th iter=======" << std::endl;

      std::for_each(vertices(g).first, vertices(g).second,
              exercise_vertex<Graph>(g, name, node_rank_map));

      // print rank value:
      std::map<char, float>::iterator it(node_rank_map.begin());
      for(; it != node_rank_map.end(); ++it) {
          std::cout << it->first << "--->" << it->second << std::endl;
      }

      node_rank_map.swap(update_map);
      update_map.clear();
      
  }

  return 0;
}

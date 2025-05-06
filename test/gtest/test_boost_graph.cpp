#include "erl_common/eigen.hpp"
#include "erl_common/test_helper.hpp"

#include <boost/graph/adjacency_list.hpp>

TEST(Boost, AdjacencyList) {

    Eigen::Matrix2Xd vertices = Eigen::Matrix2Xd::Random(2, 10);
    using Graph = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::undirectedS,
        boost::no_property,
        boost::property<boost::edge_weight_t, double>>;
    Graph graph(10);
    for (long i = 0; i < vertices.cols(); ++i) {
        for (long j = 0; j < vertices.cols(); ++j) {
            // check if the edge exists
            boost::graph_traits<Graph>::edge_descriptor edge;
            bool exists;
            std::tie(edge, exists) = boost::edge(i, j, graph);
            if (exists) { continue; }

            if (i != j) {
                const double dist = (vertices.col(i) - vertices.col(j)).norm();
                boost::add_edge(i, j, dist, graph);
            }
        }
    }

    std::cout << erl::common::EigenToNumPyFmtString(vertices) << std::endl;

    boost::graph_traits<Graph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(graph); ei != ei_end; ++ei) {
        const long u = boost::source(*ei, graph);
        const long v = boost::target(*ei, graph);
        const double weight = boost::get(boost::edge_weight, graph, *ei);
        const double weight2 =
            boost::get(boost::edge_weight, graph, boost::edge(v, u, graph).first);
        EXPECT_EQ(weight, weight2);
        const double dist = (vertices.col(u) - vertices.col(v)).norm();
        std::cout << fmt::format(
                         "{}({}) -> {}({}), weight: {}, dist: {}",
                         u,
                         erl::common::EigenToNumPyFmtString(vertices.col(u).transpose()),
                         v,
                         erl::common::EigenToNumPyFmtString(vertices.col(v).transpose()),
                         weight,
                         dist)
                  << std::endl;
    }

    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = boost::vertices(graph); vi != vi_end; ++vi) {
        boost::graph_traits<Graph>::adjacency_iterator ai, ai_end;
        for (boost::tie(ai, ai_end) = boost::adjacent_vertices(*vi, graph); ai != ai_end; ++ai) {
            const long u = *vi;
            const long v = *ai;
            const double dist = (vertices.col(u) - vertices.col(v)).norm();
            std::cout << fmt::format(
                             "{}({}) -> {}({}), dist: {}",
                             u,
                             erl::common::EigenToNumPyFmtString(vertices.col(u).transpose()),
                             v,
                             erl::common::EigenToNumPyFmtString(vertices.col(v).transpose()),
                             dist)
                      << std::endl;
        }
    }
}

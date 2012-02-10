This is a simple implementation of pageRank algorithm in c++ with boost graph library.

The general pageRank algorithm can be described as following:
for each node in the web graph, the page rank value of each node PR(A) can be computed by:

PR(A) = 1 - p + p * sum { PR(N) / |out_degree of N| }

in which, p denotes a dump factor, its value is 0.85 as normal.
N has outlink to A.



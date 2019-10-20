#pragma once
#include <vector>
class Graph
{
public:
	typedef int node_id;
	typedef double ftype; // flow type

	Graph();
	~Graph();
private:
	struct node;
	struct arc;
	
	ftype flow_;
	std::vector<node*> nodes_list_;
	int nodes_num_ = 0;

	struct node
	{
		node_id id_;
		arc* first_ = NULL;
		bool is_visited_ = false;

		node* parent_ = NULL; // Record its parent in BFS path.
		arc* p_arc = NULL; // Record the arc that starts from its parent to itself.
		ftype termi_cap_;
	};

	struct arc
	{
		arc* next_ = NULL; //Next arc started from the same node.
		arc* rev_ = NULL;
		node* dst_ = NULL; // The node this arc heads to.
		ftype res_cap_; // capcity in residual graph.
		ftype cap_;
	};

	void init_bfs();
	node* bfs_in_residual_graph( );
	void clear_flow(node* v);
	void init_maxflow();

	void delete_arcs(node* v);

	arc* add_edge(node_id i, node_id j, ftype capcity);

public:
	void add_nodes(int nodes_num);
	void add_terminal_weight(node_id i, ftype w_src, ftype w_sink);
	void add_weight(node_id i, node_id j, ftype cap, ftype rev_cap);
	void print_graph();

public:
	ftype maxflow();
	void get_min_cutted_sets(std::vector<node_id> &source_set, std::vector<node_id> &sink_set);
};


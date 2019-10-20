#include "Graph.h"
#include <queue>
#include <iostream>

Graph::Graph()
{
	flow_ = 0;
}

Graph::~Graph()
{
	for (int i = 0; i < nodes_list_.size(); i++)
		delete_arcs(nodes_list_[i]);

	/*if (source_)
		delete source_;
	if (sink_)
		delete sink_;*/
	for (int i = 0; i < nodes_list_.size(); i++)
		if (nodes_list_[i])
			delete nodes_list_[i];
}

void Graph::delete_arcs(node* v)
{
	arc* a = v->first_;

	while (a)
	{
		arc* p = a->next_;
		delete a;
		a = p;
	}
}

void Graph::add_nodes(int nodes_num)
{
	for (int i = 0; i < nodes_num; i++)
	{
		nodes_list_.push_back(new node());
		nodes_list_[nodes_num_ + i]->id_ = nodes_num_ + i;
	}
	nodes_num_ += nodes_num;
}

Graph::arc* Graph::add_edge(node_id i, node_id j, ftype capcity)
{
	node *u, *v;

	/*if (i == -1)
		u = source_;
	else if (i == -2)
		u = sink_;
	else*/
		u = nodes_list_[i];

	/*if (j == -1)
		v = source_;
	else if (j == -2)
		v = sink_;
	else*/
		v = nodes_list_[j];

	arc* a;
	arc* p = u->first_;
	if (p==NULL || v->id_ < p->dst_->id_)
	{
		a = new arc();
		a->dst_ = v;
		a->cap_ = capcity;
		a->next_ = p;
		u->first_ = a;
		return a;
	}
	else
	{
		int count = 0;
		while (p->next_)
		{
			int next_id = p->next_->dst_->id_;
			if (v->id_ == next_id)
			{
				p->next_->cap_ += capcity;
				return p->next_;
			}
			
			if (next_id > v->id_)
			{
				break;
			}
			p = p->next_;
			//std::cout << count << "\n";
			count++;
		}
		a = new arc();
		a->dst_ = v;
		a->cap_ = capcity;
		a->next_ = p->next_;
		p->next_ = a;

	}
	

	return a;
}

void Graph::add_terminal_weight(node_id i, ftype w_src, ftype w_sink)
{
	/*if (abs(w_src - w_sink) < 1e-3)
		return;
	arc* a, * rev_a;
	if (w_src > w_sink)
	{
		a = add_edge(-1, i, w_src - w_sink);
		rev_a = add_edge(i, -1, 0);
	}
	else
	{
		a = add_edge(i, -2, w_sink - w_src);
		rev_a = add_edge(-2, i, 0);
	}
	flow_ += w_src < w_sink ? w_src : w_sink;
	a->rev_ = rev_a;
	rev_a->rev_ = a;*/
	flow_ += w_src < w_sink ? w_src : w_sink;
	nodes_list_[i]->termi_cap_ = w_src - w_sink;
	nodes_list_[i]->parent_ = NULL;
}

void Graph::add_weight(node_id i, node_id j, ftype cap, ftype rev_cap)
{
	arc* a, * rev_a;
	a = add_edge(i, j, cap);
	rev_a = add_edge(j, i, rev_cap);
	a->rev_ = rev_a;
	rev_a->rev_ = a;
}

void Graph::init_bfs()
{
	/*source_->is_visited_ = false;
	sink_->is_visited_ = false;*/
	for (int i = 0; i < nodes_list_.size(); i++)
	{
		nodes_list_[i]->is_visited_ = false;
	}
}

Graph::node* Graph::bfs_in_residual_graph()
{
	/* Set all nodes unvisited. */
	init_bfs();

	/* Start BFS */
	std::queue<node*> Q;
	/*Q.push(source_);*/
	for (int i = 0; i < nodes_num_; i++)
	{
		if (nodes_list_[i]->termi_cap_ > 0)
		{
			Q.push(nodes_list_[i]);
			nodes_list_[i]->is_visited_ = true;
		}
			
	}
	
	node* curr_node = NULL;
	while (!Q.empty())
	{
		curr_node = Q.front();
		curr_node->is_visited_ = true;
		if (curr_node->termi_cap_ < 0)
			break;
		Q.pop();
		for (arc* a = curr_node->first_; a; a = a->next_)
		{
			if ((!a->dst_->is_visited_) && a->res_cap_ > 0)
			{
				Q.push(a->dst_);
				a->dst_->is_visited_ = true;
				a->dst_->parent_ = curr_node;
				a->dst_->p_arc = a;
			}
		}
	}
	/* If sink can be reached by BFS starting from source, then return true, esle false. */
	if (curr_node == NULL)
		return NULL;
	if (curr_node->termi_cap_ < 0)
		return curr_node;
	else
		return NULL;
	
}

void Graph::clear_flow(node* v)
{
	arc* a = v->first_;
	while (a)
	{
		a->res_cap_ = a->cap_;
		a = a->next_;
	}
}

void Graph::init_maxflow()
{
	for (int i = 0; i < nodes_list_.size(); i++)
	{
		clear_flow(nodes_list_[i]);
	}
}

Graph::ftype Graph::maxflow()
{
	init_maxflow();

	ftype max_flow = flow_;

	node* last_node;
	node* curr_node;
	while (last_node = bfs_in_residual_graph())
	{
		/* Get the path from soure to sink and the minimum residual capcity along the path. */
		ftype path_flow = -last_node->termi_cap_;
		for (curr_node = last_node; curr_node->parent_; curr_node = curr_node->parent_)
			path_flow = std::min<ftype>(path_flow, curr_node->p_arc->res_cap_);
		path_flow = std::min(path_flow, curr_node->termi_cap_);
		max_flow += path_flow;

		/* Update residual capcities of edges and reverse edges along the path. */
		last_node->termi_cap_ = - (-last_node->termi_cap_ - path_flow);
		for (curr_node = last_node; curr_node->parent_; curr_node = curr_node->parent_)
		{
			curr_node->p_arc->res_cap_ -= path_flow; // c_f[p->curr]
			curr_node->p_arc->rev_->res_cap_ += path_flow; // c_f[curr->p]
		}
		curr_node->termi_cap_ -= path_flow;
	}

	return max_flow;
}

void Graph::get_min_cutted_sets(std::vector<node_id>& source_set, std::vector<node_id>& sink_set)
{
	for (node_id i = 0; i < nodes_list_.size(); i++)
	{
		if (nodes_list_[i]->is_visited_)
			source_set.push_back(i);
		else
			sink_set.push_back(i);
	}
}

void Graph::print_graph()
{
	for (int i = 0; i < nodes_num_; i++)
	{
		node* u = nodes_list_[i];
		for (arc* a = u->first_; a; a = a->next_)
		{
			std::cout << u->id_ << "-" << a->cap_ - a->res_cap_ << "->" << a->dst_->id_ << "\n";
		}
	}
}
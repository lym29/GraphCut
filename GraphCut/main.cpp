#include "Graph.h"
#include <iostream>
#include <fstream>
#include "UI.h"

int main()
{
	UI my_ui;
	std::ifstream myReadFile;
	myReadFile.open("./img_name.txt");
	std::string img_name;
	myReadFile >> img_name;

	if (my_ui.input_img(img_name))
		return my_ui.show();
	else
		return -1;
}

void testmaxflow()
{
	Graph* my_graph = new Graph();
	my_graph->add_nodes(2);
	my_graph->add_terminal_weight(0, 3, 2);
	my_graph->add_terminal_weight(1, 2, 3);

	my_graph->add_weight(0, 1, 10, 5);

	my_graph->print_graph();
	Graph::ftype maxflow = my_graph->maxflow();
	std::cout << maxflow << std::endl;
	my_graph->print_graph();
	delete my_graph;
}
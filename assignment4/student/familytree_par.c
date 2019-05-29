#include "familytree.h"

int traverse_rec(tree *node) {
	if (node == NULL) return 0;

	int father_iq, mother_iq;

	#pragma omp task
	father_iq = traverse_rec(node->father);

	#pragma omp task
	mother_iq = traverse_rec(node->mother);

	#pragma omp task
	{
		node->IQ = compute_IQ(node->data, father_iq, mother_iq);
		genius[node->id] = node->IQ;
	}
	
    return node->IQ;
}

int traverse(tree *node, int numThreads) {
	
	if (node == NULL) return 0;

    #pragma omp parallel single nowait firstprivate(node) num_threads(numThreads)
	traverse_rec(node);

	return node->IQ;
}

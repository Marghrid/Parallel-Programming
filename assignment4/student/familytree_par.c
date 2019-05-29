#include "familytree.h"

int traverse(tree *node, int numThreads) {
	
	if (node == NULL) return 0;

    #pragma omp parallel num_threads(numThreads)
    {
		#pragma omp single
		{
			int father_iq, mother_iq;

    		#pragma omp task
			father_iq = traverse(node->father, numThreads);

			#pragma omp task
			mother_iq = traverse(node->mother, numThreads);

			#pragma omp task
			node->IQ = compute_IQ(node->data, father_iq, mother_iq);
		}
	}
	genius[node->id] = node->IQ;
	return node->IQ;
}

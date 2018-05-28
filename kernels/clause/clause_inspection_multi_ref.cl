__kernel void clause_inspection_ref(const int M,
                      const int Length,
											const __global int* Clauses,
                      const __global int* Assignments,
                      const __global int* Target,
                      __global int* C) {
    
    const int i = get_global_id(0);
    if (i < M) {
	    C[i] = Target[i];
	  }
}

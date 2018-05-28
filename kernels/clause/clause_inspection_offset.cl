__kernel void clause_inspection(const int M,
                      const __global int* Clauses,
                      const __global int* Assignments,
                      const __global int* Offsets,
                      const __global int* Target,
                      __global int* C) {

  enum Assignment {TRUE = 1, FALSE = 0, UNDEF = 2};
  enum Result {SAT = 1, CONFLICT = 2, UNIT = 3, WASTE = 4, UNRES = 5};

  INIT_LOCAL_assignments(Assignments, Local_Assignments);
  INIT_LOCAL_clauses_offsets(Offsets, Local_Clauses_Offsets);

  const int i = get_global_id(0);
  if (i >= M) {
    return;
  }

  const int N = clauses_offsets_get(Local_Clauses_Offsets, i + 1) - clauses_offsets_get(Local_Clauses_Offsets, i);
  
  int result = UNRES;
  int count = 0;
  for (int k = 0; k < N; k++) {
    int lit = _2D_clauses_get(Clauses, i, k, Offsets);
    int val = assignments_get(Local_Assignments, lit);
    if (val == UNDEF) {
      count++;
    } else if (val == TRUE) {
      C[i] = SAT;
      // printf("%d %d\n", i, SAT);
      return;
    }
  }

  // Return clause state based on count
  if (count == N) {
    result = WASTE;
  } else if (count == 0) {
    result = CONFLICT;
  } else if (count == 1) {
    result = UNIT;
  } else {
    result = UNRES;
  }

  // printf("%d %d\n", i, result);

  C[i] = result;
}

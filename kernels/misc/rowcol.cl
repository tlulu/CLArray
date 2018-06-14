__kernel void misc(const int M, const int N,
                      const __global int* A,
                      __global int* C) {
  
  const int i = get_global_id(0);

  if (i >= M) return;

  for (int j = 0; j < N; j++) {
    C[i * N + j] = _2D_A_get(A, i, j, null);
  }
}

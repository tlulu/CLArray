__kernel void hadamard(const int M, const int N,
                      const __global int* A,
                      const __global int* B,
                      __global int* C) {

  INIT_LOCAL_A(A, Local_A);
  INIT_LOCAL_B(B, Local_B);
  
  const int i = get_global_id(0);
  const int j = get_global_id(1);

  if (i >= M) return;
  if (j >= N) return;

  int a = _2D_A_get(Local_A, i, j);
  int b = _2D_B_get(Local_B, i, j);

  // Output 2D array is in row major format.
  C[i * N + j] = a * b;
}

__kernel void multiplication(const int M, const int K, const int N,
                      const __global int* A,
                      const __global int* B,
                      __global int* C) {

  INIT_LOCAL_A(A, Local_A);
  INIT_LOCAL_B(B, Local_B);
  
  const int i = get_global_id(0);
  const int j = get_global_id(1);

  if (i >= M) return;
  if (j >= N) return;

  int acc = 0;
  for (int k = 0; k < K; k++) {
    int a = _2D_A_get(Local_A, i, k);
    int b = _2D_B_get(Local_B, k, j);
    acc += a * b;
  }

  // Output 2D array is in row major format.
  C[i * N + j] = acc;
}

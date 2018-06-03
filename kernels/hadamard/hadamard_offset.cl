__kernel void hadamard(const int M,
                      const __global int* A,
                      const __global int* B,
                      const __global int* Offsets,
                      __global int* C) {

  INIT_LOCAL_A(A, Local_A);
  INIT_LOCAL_B(B, Local_B);
  INIT_LOCAL_A_offsets(Offsets, Local_A_Offsets);
  
  const int i = get_global_id(0);
  const int j = get_global_id(1);

  const int N = A_offsets_get(Local_A_Offsets, i + 1) - A_offsets_get(Local_A_Offsets, i);

  if (i >= M) return;
  if (j >= N) return;

  int a = _2D_A_get(Local_A, i, j, Local_A_Offsets);
  int b = _2D_B_get(Local_B, i, j);

  C[i * N + j] = a * b;
}

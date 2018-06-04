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

  const int start = A_offsets_get(Local_A_Offsets, i);
  const int next = A_offsets_get(Local_A_Offsets, i + 1);
  const int N = next - start;

  if (i >= M) return;
  if (j >= N) return;

  const int a = _2D_A_get(Local_A, i, j, Local_A_Offsets);
  const int b = _2D_B_get(Local_B, i, j);

  // Output 2D array is in offset format.
  C[start + j] = a * b;
}

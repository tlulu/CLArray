__kernel void misc(const int M, const int A_Size,
                      const __global int* A,
                      __global int* C) {

  INIT_LOCAL_A(A, Local_A);
  
  const int i = get_global_id(0);

  if (i >= M) return;

  int acc = 0;
  for (int j = 0; j < A_Size; j++) {
    int e = A_get(Local_A, j);
    acc += e;
  }

  C[i] = acc;
}

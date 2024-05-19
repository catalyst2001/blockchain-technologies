#include <stdio.h>
#include <vector>
#include <omp.h>

void print_mat(std::vector<std::vector<double>>& matrix)
{
  for (int i = 0; i < matrix.size(); ++i) {
    for (int j = 0; j < matrix[i].size(); ++j) {
      printf("%.02lf ", matrix[i][j]);
    }
    printf("\n");
  }
}

void forward_elimination(std::vector<std::vector<double>>& matrix, int n)
{
  for (int k = 0; k < n - 1; ++k) {
#pragma omp parallel for
    for (int i = k + 1; i < n; ++i) {
      double factor = matrix[i][k] / matrix[k][k];
      for (int j = k; j < n + 1; ++j) {
        matrix[i][j] -= factor * matrix[k][j];
      }
    }
  }
}

std::vector<double> back_substitution(std::vector<std::vector<double>>& matrix, int n)
{
  std::vector<double> solution(n);
  solution[n - 1] = matrix[n - 1][n] / matrix[n - 1][n - 1];
  for (int i = n - 2; i >= 0; --i) {
    double sum = 0.0;
#pragma omp parallel for reduction(+:sum)
    for (int j = i + 1; j < n; ++j) {
      sum += matrix[i][j] * solution[j];
    }
    solution[i] = (matrix[i][n] - sum) / matrix[i][i];
  }
  return solution;
}

// 2x + 3y - z = 1
// 4x + y + 2z = 3
// x + 2y - 3z = 4
int main()
{
  std::vector<std::vector<double>> matrix = {
      {2, 3, -1, 1},
      {4, 1, 2, 3},
      {1, 2, -3, 4}
  };

  int n = matrix.size();

  printf("original matrix:\n");
  print_mat(matrix);

  forward_elimination(matrix, n);
  std::vector<double> solution = back_substitution(matrix, n);

  printf("\nSolution:\n");
  for (int i = 0; i < n; i++)
    printf("x%d = %.2lf\n", i + 1, solution[i]);

  return 0;
}

#if 1
#else
int main()
{
  omp_set_num_threads(8);
#pragma omp parallel
  {
    printf("Hello world from thread %d out of %d\n", omp_get_thread_num(), omp_get_num_threads());
  }
  return 0;
}
#endif
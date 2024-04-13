#include <iostream>
#include <vector>
#include <omp.h>

// Функция для вывода матрицы
void printMatrix(std::vector<std::vector<double>>& matrix) {
  for (int i = 0; i < matrix.size(); ++i) {
    for (int j = 0; j < matrix[i].size(); ++j) {
      std::cout << matrix[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

// Функция для выполнения прямого хода метода Гаусса
void forwardElimination(std::vector<std::vector<double>>& matrix, int n) {
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

// Функция для выполнения обратного хода метода Гаусса
std::vector<double> backSubstitution(std::vector<std::vector<double>>& matrix, int n) {
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

int main() {
  // Пример системы линейных уравнений:
  // 2x + 3y - z = 1
  // 4x + y + 2z = 3
  // x + 2y - 3z = 4

  std::vector<std::vector<double>> matrix = {
      {2, 3, -1, 1},
      {4, 1, 2, 3},
      {4, 1, 2, 3},
      {1, 2, -3, 4}
  };

  int n = matrix.size();

  std::cout << "Original matrix:" << std::endl;
  printMatrix(matrix);

  forwardElimination(matrix, n);

  std::cout << "\nMatrix after forward elimination:" << std::endl;
  printMatrix(matrix);

  std::vector<double> solution = backSubstitution(matrix, n);

  std::cout << "\nSolution:" << std::endl;
  for (int i = 0; i < n; ++i) {
    std::cout << "x" << i + 1 << " = " << solution[i] << std::endl;
  }

  return 0;
}

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <omp.h>

typedef std::vector<std::vector<int>> matrix;

void ReadMatrix(std::ifstream* input, matrix* matrix_b, int dim) {
    for (int i = 0; i < dim; ++i) {
        (*matrix_b)[i].resize(dim);
        for (int j = 0; j < dim; ++j) {
            (*input) >> (*matrix_b)[i][j];
        }
    }
}

void CalculateRow(const matrix& matrix_a, const matrix& matrix_b, std::vector<int>* row,
                  int row_num, int dim) {
    for (int i = 0; i < dim; ++i) {
        int sum = 0;
        for (int j = 0; j < dim; ++j) {
            sum += matrix_a[row_num][j] * matrix_b[j][i];
        }
        row->push_back(sum);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Please provide the following arguments <input_path> <output_path>\n";
        return 0;
    }
    std::ifstream input(argv[1]);
    int dim = 0;
    input >> dim;
    matrix matrix_a(dim);
    matrix matrix_b(dim);
    matrix matrix_res(dim);
    ReadMatrix(&input, &matrix_a, dim);
    ReadMatrix(&input, &matrix_b, dim);
    input.close();
    int thread_num;
    std::cout << "Please enter the desired number of threads: ";
    std::cin >> thread_num;
    thread_num--;
    omp_set_dynamic(0);
    omp_set_num_threads(thread_num);
    auto begin = std::chrono::steady_clock::now();
#pragma omp parallel for shared(dim) shared(matrix_a) shared(matrix_b) \
    shared(matrix_res) default(none)
    for (int i = 0; i < dim; ++i) {
        CalculateRow(matrix_a, matrix_b, &(matrix_res[i]), i, dim);
    }
    auto end = std::chrono::steady_clock::now();
    std::ofstream output(argv[2]);
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            output << std::left << std::setw(6) << matrix_res[i][j] << " ";
        }
        output << "\n";
    }
    output.close();
    std::cout << "Done in "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
              << " microseconds";
    return 0;
}

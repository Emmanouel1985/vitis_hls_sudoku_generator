#include <hls_vector.h>
#include <vector>
#include <iostream>

#define DATA_SIZE 4
static constexpr unsigned N = 9;
static constexpr unsigned N2 = N * N;

extern "C" void sudoku_kernel(hls::vector<uint8_t, N2> *sudokusIn, hls::vector<uint8_t, N2> *sudokusOut, int size);

int main(int, char**) {

    std::vector<hls::vector<uint8_t, N2>> sudokus(DATA_SIZE, 
        {   2,3,8,4,1,7,0,5,6,
            1,6,0,5,2,8,7,3,4,
            4,5,7,6,3,0,2,1,8,
            3,8,2,7,4,1,5,6,0,
            6,0,1,8,5,2,3,4,7,
            5,7,4,0,6,3,8,2,1,
            8,2,3,1,7,4,6,0,5,
            0,1,6,2,8,5,4,7,3,
            7,4,5,3,0,6,1,8,2});
    std::vector<hls::vector<uint8_t, N2>> sudokusOut(DATA_SIZE);

    sudoku_kernel(sudokus.data(), sudokusOut.data(), DATA_SIZE);

    for (int ii = 0; ii < N2; ++ii)
    {
        std::cout << int(sudokusOut[0][ii]);
    }
    std::cout << std::endl;

    return 0;
}

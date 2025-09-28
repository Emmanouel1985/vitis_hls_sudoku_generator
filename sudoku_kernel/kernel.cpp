#include <ap_int.h>
#include <hls_np_channel.h>
#include <hls_stream.h>
#include <hls_task.h>
#include <hls_vector.h>
#include <hls_print.h>
#include <stdint.h>
#include <tuple>

#define DATA_SIZE 4096

// TRIPCOUNT identifier
const int c_size = DATA_SIZE;

static constexpr unsigned N = 9;
static constexpr unsigned N2 = N * N;
static constexpr unsigned N_PERMUTATIONS = 2 * 6 * 6 * 6 * 6 * 6 * 6 * 6 * 6;

using vec_t = hls::vector<ap_uint<4>, N2>;

static void proc(hls::stream<hls::vector<uint8_t,N2>> &permutationStream,
                 hls::stream<vec_t> &inStream, hls::stream<vec_t> &outStream) {
  auto sudokuIn = inStream.read();
  auto sudokuOut = sudokuIn;
  for (int ii = 0; ii < N_PERMUTATIONS; ++ii) {
    //if (ii % 1000 == 0) {hls::print("proc %d\n", ii);}
    vec_t sudokuTmp;
    auto permutation = permutationStream.read();
#pragma HLS array_partition variable = permutation type = complete
// #pragma HLS array_partition variable = sudokuTmp type = complete
// #pragma HLS array_partition variable = sudokuIn type = complete
// #pragma HLS array_partition variable = sudokuOut type = complete
    for (int jj = 0; jj < N2; ++jj) {
#pragma HLS UNROLL
      sudokuTmp[jj] = sudokuIn[permutation[jj]];
    }
    ap_uint<4> newLabels[N];
#pragma HLS array_partition variable = newLabels type = complete
    for (int jj = 0; jj < N; ++jj) {
#pragma HLS UNROLL
      newLabels[sudokuTmp[jj]] = jj;
    }
    for (int jj = 0; jj < N2; ++jj) {
#pragma HLS UNROLL
      sudokuTmp[jj] = newLabels[sudokuTmp[jj]];
    }
    sudokuOut = std::min(sudokuOut, sudokuTmp);
  }
  outStream.write(sudokuOut);
}

static void swap(hls::vector<uint8_t, N>& lhs, hls::vector<uint8_t, N>& rhs) {
#pragma HLS INLINE
  const auto tmp = lhs;
  lhs = rhs;
  rhs = tmp;
}

static void generate_permutations(
    hls::stream<hls::vector<uint8_t, N2>, 2> &inStream0, hls::stream<hls::vector<uint8_t, N2>, 2> &inStream1,
    hls::stream<hls::vector<uint8_t, N2>, 2> &inStream2, hls::stream<hls::vector<uint8_t, N2>, 2> &inStream3) {
  for (const auto [b00, b01, b02] :
       {std::make_tuple(false, false, false),
        std::make_tuple(false, false, true),
        std::make_tuple(true, false, false), std::make_tuple(true, false, true),
        std::make_tuple(false, true, false),
        std::make_tuple(true, true, false)}) {
#pragma HLS loop_flatten off
    for (const auto [b10, b11, b12] : {std::make_tuple(false, false, false),
                                       std::make_tuple(false, false, true),
                                       std::make_tuple(true, false, false),
                                       std::make_tuple(true, false, true),
                                       std::make_tuple(false, true, false),
                                       std::make_tuple(true, true, false)}) {
#pragma HLS loop_flatten off
      for (const auto [b20, b21, b22] : {std::make_tuple(false, false, false),
                                         std::make_tuple(false, false, true),
                                         std::make_tuple(true, false, false),
                                         std::make_tuple(true, false, true),
                                         std::make_tuple(false, true, false),
                                         std::make_tuple(true, true, false)}) {
#pragma HLS loop_flatten off
        for (const auto [b0, b1, b2] : {std::make_tuple(false, false, false),
                                        std::make_tuple(false, false, true),
                                        std::make_tuple(true, false, false),
                                        std::make_tuple(true, false, true),
                                        std::make_tuple(false, true, false),
                                        std::make_tuple(true, true, false)}) {
#pragma HLS loop_flatten off
          for (const auto [c00, c01, c02] :
               {std::make_tuple(false, false, false),
                std::make_tuple(false, false, true),
                std::make_tuple(true, false, false),
                std::make_tuple(true, false, true),
                std::make_tuple(false, true, false),
                std::make_tuple(true, true, false)}) {
#pragma HLS loop_flatten off
            for (const auto [c10, c11, c12] :
                 {std::make_tuple(false, false, false),
                  std::make_tuple(false, false, true),
                  std::make_tuple(true, false, false),
                  std::make_tuple(true, false, true),
                  std::make_tuple(false, true, false),
                  std::make_tuple(true, true, false)}) {
#pragma HLS loop_flatten off
              for (const auto [c20, c21, c22] :
                   {std::make_tuple(false, false, false),
                    std::make_tuple(false, false, true),
                    std::make_tuple(true, false, false),
                    std::make_tuple(true, false, true),
                    std::make_tuple(false, true, false),
                    std::make_tuple(true, true, false)}) {
#pragma HLS loop_flatten off
                for (const auto [c0, c1, c2] :
                     {std::make_tuple(false, false, false),
                      std::make_tuple(false, false, true),
                      std::make_tuple(true, false, false),
                      std::make_tuple(true, false, true),
                      std::make_tuple(false, true, false),
                      std::make_tuple(true, true, false)}) {
#pragma HLS loop_flatten off
                  for (bool t : {false, true}) {
#pragma HLS loop_flatten off
                    hls::vector<hls::vector<uint8_t, N>, N> p{
                        {0, 1, 2, 3, 4, 5, 6, 7, 8},
                        {9, 10, 11, 12, 13, 14, 15, 16, 17},
                        {18, 19, 20, 21, 22, 23, 24, 25, 26},
                        {27, 28, 29, 30, 31, 32, 33, 34, 35},
                        {36, 37, 38, 39, 40, 41, 42, 43, 44},
                        {45, 46, 47, 48, 49, 50, 51, 52, 53},
                        {54, 55, 56, 57, 58, 59, 60, 61, 62},
                        {63, 64, 65, 66, 67, 68, 69, 70, 71},
                        {72, 73, 74, 75, 76, 77, 78, 79, 80}};
// #pragma HLS array_partition variable = p type = complete dim = 0
// #pragma HLS disaggregate variable = p
                    if (b00) {
                      swap(p[0], p[1]);
                    }
                    if (b01) {
                      swap(p[0], p[2]);
                    }
                    if (b02) {
                      swap(p[1], p[2]);
                    }
                    if (b10) {
                      swap(p[3], p[4]);
                    }
                    if (b11) {
                      swap(p[3], p[5]);
                    }
                    if (b12) {
                      swap(p[4], p[5]);
                    }
                    if (b20) {
                      swap(p[6], p[7]);
                    }
                    if (b21) {
                      swap(p[6], p[8]);
                    }
                    if (b22) {
                      swap(p[7], p[8]);
                    }
                    if (b0) {
                      swap(p[0], p[3]);
                      swap(p[1], p[4]);
                      swap(p[2], p[5]);
                    }
                    if (b1) {
                      swap(p[0], p[6]);
                      swap(p[1], p[7]);
                      swap(p[2], p[8]);
                    }
                    if (b2) {
                      swap(p[3], p[6]);
                      swap(p[4], p[7]);
                      swap(p[5], p[8]);
                    }
                    p = hls::vector<hls::vector<uint8_t, N>, N>{
                        {p[0][0], p[1][0], p[2][0], p[3][0], p[4][0], p[5][0],p[6][0], p[7][0], p[8][0]},
                        {p[0][1], p[1][1], p[2][1], p[3][1], p[4][1], p[5][1],p[6][1], p[7][1], p[8][1]},
                        {p[0][2], p[1][2], p[2][2], p[3][2], p[4][2],p[5][2], p[6][2], p[7][2], p[8][2]},
                        {p[0][3], p[1][3], p[2][3], p[3][3], p[4][3],p[5][3], p[6][3], p[7][3], p[8][3]},
                        {p[0][4], p[1][4], p[2][4], p[3][4], p[4][4],p[5][4], p[6][4], p[7][4], p[8][4]},
                        {p[0][5], p[1][5], p[2][5], p[3][5], p[4][5],p[5][5], p[6][5], p[7][5], p[8][5]},
                        {p[0][6], p[1][6], p[2][6], p[3][6], p[4][6],p[5][6], p[6][6], p[7][6], p[8][6]},
                        {p[0][7], p[1][7], p[2][7], p[3][7], p[4][7],p[5][7], p[6][7], p[7][7], p[8][7]},
                        {p[0][8], p[1][8], p[2][8], p[3][8], p[4][8],p[5][8], p[6][8], p[7][8], p[8][8]}};
                    if (c00) {
                      swap(p[0], p[1]);
                    }
                    if (c01) {
                      swap(p[0], p[2]);
                    }
                    if (c02) {
                      swap(p[1], p[2]);
                    }
                    if (c10) {
                      swap(p[3], p[4]);
                    }
                    if (c11) {
                      swap(p[3], p[5]);
                    }
                    if (c12) {
                      swap(p[4], p[5]);
                    }
                    if (c20) {
                      swap(p[6], p[7]);
                    }
                    if (c21) {
                      swap(p[6], p[8]);
                    }
                    if (c22) {
                      swap(p[7], p[8]);
                    }
                    if (c0) {
                      swap(p[0], p[3]);
                      swap(p[1], p[4]);
                      swap(p[2], p[5]);
                    }
                    if (c1) {
                      swap(p[0], p[6]);
                      swap(p[1], p[7]);
                      swap(p[2], p[8]);
                    }
                    if (c2) {
                      swap(p[3], p[6]);
                      swap(p[4], p[7]);
                      swap(p[5], p[8]);
                    }
                    if (t)
                    {
                      p = hls::vector<hls::vector<uint8_t, N>, N>{
                          {p[0][0], p[1][0], p[2][0], p[3][0], p[4][0], p[5][0],p[6][0], p[7][0], p[8][0]},
                          {p[0][1], p[1][1], p[2][1], p[3][1], p[4][1], p[5][1],p[6][1], p[7][1], p[8][1]},
                          {p[0][2], p[1][2], p[2][2], p[3][2], p[4][2],p[5][2], p[6][2], p[7][2], p[8][2]},
                          {p[0][3], p[1][3], p[2][3], p[3][3], p[4][3],p[5][3], p[6][3], p[7][3], p[8][3]},
                          {p[0][4], p[1][4], p[2][4], p[3][4], p[4][4],p[5][4], p[6][4], p[7][4], p[8][4]},
                          {p[0][5], p[1][5], p[2][5], p[3][5], p[4][5],p[5][5], p[6][5], p[7][5], p[8][5]},
                          {p[0][6], p[1][6], p[2][6], p[3][6], p[4][6],p[5][6], p[6][6], p[7][6], p[8][6]},
                          {p[0][7], p[1][7], p[2][7], p[3][7], p[4][7],p[5][7], p[6][7], p[7][7], p[8][7]},
                          {p[0][8], p[1][8], p[2][8], p[3][8], p[4][8],p[5][8], p[6][8], p[7][8], p[8][8]}};
                    }
                    hls::vector<uint8_t, N2> pOut{
                        p[0][0],p[0][1],p[0][2],p[0][3],p[0][4],p[0][5],p[0][6],p[0][7],p[0][8],
                        p[1][0],p[1][1],p[1][2],p[1][3],p[1][4],p[1][5],p[1][6],p[1][7],p[1][8],
                        p[2][0],p[2][1],p[2][2],p[2][3],p[2][4],p[2][5],p[2][6],p[2][7],p[2][8],
                        p[3][0],p[3][1],p[3][2],p[3][3],p[3][4],p[3][5],p[3][6],p[3][7],p[3][8],
                        p[4][0],p[4][1],p[4][2],p[4][3],p[4][4],p[4][5],p[4][6],p[4][7],p[4][8],
                        p[5][0],p[5][1],p[5][2],p[5][3],p[5][4],p[5][5],p[5][6],p[5][7],p[5][8],
                        p[6][0],p[6][1],p[6][2],p[6][3],p[6][4],p[6][5],p[6][6],p[6][7],p[6][8],
                        p[7][0],p[7][1],p[7][2],p[7][3],p[7][4],p[7][5],p[7][6],p[7][7],p[7][8],
                        p[8][0],p[8][1],p[8][2],p[8][3],p[8][4],p[8][5],p[8][6],p[8][7],p[8][8]};
#pragma HLS array_partition variable = pOut type = complete
                    inStream0.write(pOut);
                    inStream1.write(pOut);
                    inStream2.write(pOut);
                    inStream3.write(pOut);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

static void read_sudokus(hls::vector<uint8_t, N2> *in,
                         hls::stream<vec_t> &inStream, int size) {
mem_rd:
  for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
    //hls::print("read_sudokus %d\n", i);
    vec_t vec;
    for (int nn = 0; nn < N2; ++nn) {
#pragma HLS UNROLL
      vec[nn] = in[i][nn];
    }
    inStream << vec;
  }
}

static void write_sudokus(hls::vector<uint8_t, N2> *out,
                          hls::stream<vec_t> &inStream, int size) {
mem_rd:
  for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
    //hls::print("write_sudokus %d\n", i);
    auto vec = inStream.read();
    for (int nn = 0; nn < N2; ++nn) {
#pragma HLS UNROLL
      out[i][nn] = vec[nn];
    }
  }
}

extern "C" {
void sudoku_kernel(hls::vector<uint8_t, N2> *sudokusIn,
                   hls::vector<uint8_t, N2> *sudokusOut, int size) {
  hls_thread_local hls::stream<hls::vector<uint8_t, N2>, 2> permutationStream[8];
  hls_thread_local hls::split::round_robin<vec_t, 4, 2> sIn;
  hls_thread_local hls::merge::round_robin<vec_t, 4, 5> sOut;

#pragma HLS array_partition variable = permutationStream type = complete
//#pragma HLS INTERFACE m_axi port = permutations bundle = gmem0
#pragma HLS INTERFACE m_axi port = sudokusIn bundle = gmem0 depth = c_size * 81
#pragma HLS INTERFACE m_axi port = sudokusOut bundle = gmem1 depth = c_size * 81

#pragma HLS dataflow
  read_sudokus(sudokusIn, sIn.in, size);
  // read_permuations<16>(permutations, permutationStream, size);
  hls_thread_local hls::task permutations_task{
      generate_permutations, permutationStream[0],  permutationStream[1],
      permutationStream[2],  permutationStream[3]};
  hls_thread_local hls::task tasks[4]{
      {proc, permutationStream[0], sIn.out[0], sOut.in[0]},
      {proc, permutationStream[1], sIn.out[1], sOut.in[1]},
      {proc, permutationStream[2], sIn.out[2], sOut.in[2]},
      {proc, permutationStream[3], sIn.out[3], sOut.in[3]}};
  write_sudokus(sudokusOut, sOut.out, size);
}
}

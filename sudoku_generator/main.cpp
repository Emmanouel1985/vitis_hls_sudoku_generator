/**
* Copyright (C) 2019-2021 Xilinx, Inc
* SPDX-License-Identifier: MIT
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

#include <algorithm>
#include <array>
#include <iostream>
#include <chrono>
#include <cstring>
#include <string_view>
#include <string>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

#define DATA_SIZE 4096
static constexpr unsigned N = 9;
static constexpr unsigned N2 = N * N;

using sudoku_t = std::array<char,N2>;

int main(int argc, char** argv) {

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    if (argc < 2)
    {
        std::cerr << "no xclbin file provided";
        return EXIT_FAILURE;
    }

    // Read settings
    std::string binaryFile{ argv[1] };
    int device_index = 0;

    std::cout << "Open the device" << device_index << std::endl;
    auto device = xrt::device(device_index);
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    size_t vector_size_bytes = sizeof(sudoku_t) * DATA_SIZE;

    auto krnl = xrt::kernel(device, uuid, "sudoku_kernel");

    std::cout << "Allocate Buffer in Global Memory\n";
    auto bo_in = xrt::bo(device, vector_size_bytes, krnl.group_id(0));
    auto bo_out = xrt::bo(device, vector_size_bytes, krnl.group_id(1));

    // Map the contents of the buffer object into host memory
    auto bo_in_map = bo_in.map<sudoku_t*>();
    auto bo_out_map = bo_out.map<sudoku_t*>();
    std::fill(bo_in_map, bo_in_map + DATA_SIZE, sudoku_t{0});
    std::fill(bo_out_map, bo_out_map + DATA_SIZE, sudoku_t{0});

    // Create the test data
    const auto* refSudoku = "123456789456789123789123456214365897365897214897214365531642978648971532972538641";
    for (int i = 0; i < DATA_SIZE; ++i) {
        std::memcpy(bo_in_map[i].data(),refSudoku,N2);
        std::transform(bo_in_map[i].begin(), bo_in_map[i].end(), bo_in_map[i].begin(),
            [](char c) { return c-'1';} );
    }

    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";

    bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    const auto start{std::chrono::steady_clock::now()};
    auto run = krnl(bo_in, bo_out, DATA_SIZE);
    run.wait();
    const auto finish{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{finish - start};

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    for (int i = 0; i < DATA_SIZE; ++i) {
        std::transform(bo_out_map[i].begin(), bo_out_map[i].end(), bo_out_map[i].begin(),
            [](char c) { return c+'1';} );
    }

    // Validate our results
    if (std::string_view{bo_out_map[0].data(),N2} != refSudoku)
        throw std::runtime_error("Value read back does not match reference");

    std::cout << elapsed_seconds.count() << "s\n";
    std::cout << "TEST PASSED\n";
    return 0;
}

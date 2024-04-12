
# Vigenère Cipher Implementation in C and CUDA

## Introduction
This project contains two implementations of the Vigenère cipher encryption and decryption techniques: one using standard C and another using CUDA for parallel processing. The implementations include functionality to encrypt, decrypt, and crack the Vigenère cipher, along with performance timing to compare the computational speed of serial vs. parallel approaches.

## System Requirements
- **GCC Compiler**: For compiling the C code.
- **NVIDIA CUDA Toolkit**: For compiling and running the CUDA code. Ensure that your system has an NVIDIA GPU that is compatible with the CUDA version installed.

## File Structure
```
    - code.cu          # Main CUDA program file
    - full_C_Code.c         # Main C program file
```

## How to Compile and Run

**C Implementation:**
1. Navigate to the directory
2. Compile the code using GCC:
    ```bash
    gcc full_C_Code.c -o c_code -lm
    ```
3. Run the compiled program:
    ```bash
    ./c_code
    ```

**CUDA Implementation:**
1. Navigate to the directory containing `code.cu`.
2. Compile the code using NVCC from the NVIDIA CUDA Toolkit:
    ```bash
    nvcc code.cu -o code_cu
    ```
3. Run the compiled program:
    ```bash
    ./code_cu
    ```

## Project Purpose
The main objective of this project is to demonstrate the difference in performance between serial and parallel processing for cryptographic operations. By implementing the same logic in both C and CUDA, the project highlights the potential speed-ups achieved through parallel computation on a GPU.

## Notes
- Ensure that the CUDA runtime and drivers are properly installed and configured on your system.
- The source code in `full_C_Code.c` and `code.cu` is heavily commented for ease of understanding and further development.

## Author
Sujeet Amberkar
#include<stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define MAX_LENGTH 10000
#define REPEAT_TIMES 10
#define MAX_KEY 10
#define NUM_LETTERS 26 



__constant__ char d_inputString[MAX_LENGTH]; 
__device__ char d_key[10]; 
__device__ char d_key_calculated[1024]; 


__device__ bool is_upper(char c) {
    return c >= 'A' && c <= 'Z';
}


__global__ void repeatString(char *output, int inputLength){
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int offset = idx * inputLength;
    for(int i = 0; i<inputLength;i++)
        output[offset+i]=d_inputString[i];
}

__global__ void vignere_encrypt_cuda(const char* msg, char* encrypted, int msgLen, int keyLen) {
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx < msgLen) {
        if (is_upper(msg[idx])) {
            int msgIndex = msg[idx] - 'A';
            int keyIndex = d_key[idx % keyLen] - 'A';
            int encryptedIndex = (msgIndex + keyIndex) % NUM_LETTERS;
            encrypted[idx] = 'A' + encryptedIndex;
        } else {
            encrypted[idx] = msg[idx];
        }
    }
}

__global__ void crack_vigenere_kernel(const char* cipher, int cipher_len, int n, char* keys) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        int freq[NUM_LETTERS] = {0};
        for (int j = i; j < cipher_len; j += n) {
            if (is_upper(cipher[j])) {
                freq[cipher[j] - 'A']++;
            }
        }
        
        int max_freq = 0;
        char max_char = 0;
        for (int k = 0; k < NUM_LETTERS; k++) {
            if (freq[k] > max_freq) {
                max_freq = freq[k];
                max_char = k + 'A';
            }
        }
        
        keys[i] = ((max_char - 'E' + NUM_LETTERS) % NUM_LETTERS) + 'A';
    }
}
__global__ void vignere_decrypt_cuda(const char* encrypted, char* decrypted, int msgLen, int keyLen) {
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx < msgLen) {
        if (is_upper(encrypted[idx])) {
            int encryptedIndex = encrypted[idx] - 'A';
            int keyIndex = d_key[idx % keyLen] - 'A'; 
            int msgIndex = (encryptedIndex - keyIndex + NUM_LETTERS) % NUM_LETTERS;
            decrypted[idx] = 'A' + msgIndex;
        } else {
            decrypted[idx] = encrypted[idx];
        }
    }
}


char* only_alphabets(const char* text) {
    char* res = (char*)malloc(strlen(text) + 1);
    if (res == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    int j = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        if (isalpha((unsigned char)text[i])) 
        {
            res[j++] = toupper((unsigned char)text[i]);
        }
    }
    res[j] = '\0'; 

    return res;
}

char* process_string_CUDA(char* temp) {
    temp = only_alphabets(temp);
    char *d_output;
    int inputLength = strlen(temp);

    size_t outputSize = MAX_LENGTH * REPEAT_TIMES; 
    cudaMemcpyToSymbol(d_inputString, temp, inputLength + 1);
    cudaDeviceSynchronize(); 

    cudaMalloc(&d_output, outputSize);
    cudaMemset(d_output, 0, outputSize);
    repeatString<<<REPEAT_TIMES, 1>>>(d_output, inputLength); 
    cudaDeviceSynchronize();

    char *plainTextInput = (char*) malloc(outputSize);
    cudaMemcpy(plainTextInput, d_output, outputSize, cudaMemcpyDeviceToHost);
    cudaFree(d_output);
    free(temp);
    return plainTextInput; 
}

char *cudaEncrypt(char *processedInput,char *key)
{
    char* d_message;
    char* d_encrypted;
    char* d_decrypted;


    int msgLen=strlen(processedInput);
    int keyLen = strlen(key);
    
    cudaMalloc((void**)&d_message, msgLen+1);
    cudaMalloc((void**)&d_encrypted, msgLen+1);
    cudaMalloc((void**)&d_decrypted, msgLen+1); 
    
    cudaMemcpy(d_message, processedInput, msgLen+1, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(d_key, key, keyLen+1); 
    
    int blockSize = 256; 
    int numBlocks = (msgLen + blockSize - 1) / blockSize; 
    
    vignere_encrypt_cuda<<<numBlocks, blockSize>>>(d_message, d_encrypted, msgLen, keyLen);

    char* encrypted = (char*)malloc(msgLen+1);
    cudaMemcpy(encrypted, d_encrypted, msgLen+1, cudaMemcpyDeviceToHost);
    cudaFree(d_message);
    cudaFree(d_encrypted);
    cudaFree(d_decrypted);
    return encrypted;

}
__global__ void calculate_matches(const char *cipher, int length, int *matches) {
    int shift = blockIdx.x * blockDim.x + threadIdx.x;
    if (shift >= length) return;

    int matchCount = 0;
    for (int i = 0; i < length; ++i) {
        if (cipher[i] == cipher[(i - shift + length) % length]) {
            ++matchCount;
        }
    }
    matches[shift] = matchCount;
}

double mean(const int arr[], int size) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum / size;
}
double variance(const int arr[], int size, double mean) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += (arr[i] - mean) * (arr[i] - mean);
    }
    return sum / size;
}
int mode(const int arr[], int size) {
    int maxValue = 0, maxCount = 0;

    for (int i = 0; i < size; ++i) {
        int count = 0;
        for (int j = 0; j < size; ++j) {
            if (arr[j] == arr[i]) ++count;
        }
        
        if (count > maxCount) {
            maxCount = count;
            maxValue = arr[i];
        }
    }
    return maxValue;
}

int guess_key_length_cuda(const char *cipher) {
    int length = strlen(cipher);
    char *d_cipher;
    int *d_matches, *matches;

    cudaMalloc((void **)&d_cipher, length);
    cudaMalloc((void **)&d_matches, length * sizeof(int));

    cudaMemcpy(d_cipher, cipher, length, cudaMemcpyHostToDevice);

    int threadsPerBlock = 256;
    int blocks = (length + threadsPerBlock - 1) / threadsPerBlock;

    calculate_matches<<<blocks, threadsPerBlock>>>(d_cipher, length, d_matches);

    matches = (int *)malloc(length * sizeof(int));
    cudaMemcpy(matches, d_matches, length * sizeof(int), cudaMemcpyDeviceToHost);




     double dat_means = mean(matches + 1, length - 1);
    double dat_std_dev = sqrt(variance(matches + 1, length - 1, dat_means));

    int* peaks = (int*)malloc(length * sizeof(int));
    int peaks_count = 0;
    for (int i = 0; i < length; i++) {
        if (matches[i] >= dat_std_dev + dat_means) {
            peaks[peaks_count++] = i;
        }
    }

    int* peak_diff = (int*)malloc((peaks_count - 1) * sizeof(int));
    for (int i = 0; i < peaks_count - 1; i++) {
        peak_diff[i] = peaks[i + 1] - peaks[i];
    }

    int key_length = mode(peak_diff, peaks_count - 1);



    cudaFree(d_cipher);
    cudaFree(d_matches);
    free(matches); 
    free(peaks);
    free(peak_diff);

    return key_length;
}

char* crack_vigenere_cuda(const char* cipher, int n) {
    int cipher_len = strlen(cipher);
    char* dev_cipher = NULL;
    char* dev_keys = NULL;
    char* keys = (char*)malloc(n + 1);
    
    cudaMalloc((void**)&dev_cipher, cipher_len * sizeof(char));
    cudaMalloc((void**)&dev_keys, n * sizeof(char));
    
    cudaMemcpy(dev_cipher, cipher, cipher_len * sizeof(char), cudaMemcpyHostToDevice);
    
    int threadsPerBlock = 256;
    int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
    crack_vigenere_kernel<<<blocksPerGrid, threadsPerBlock>>>(dev_cipher, cipher_len, n, dev_keys);
    
    cudaMemcpy(keys, dev_keys, n * sizeof(char), cudaMemcpyDeviceToHost);
    keys[n] = '\0';
    
    cudaFree(dev_cipher);
    cudaFree(dev_keys);
    
    return keys;
}
char* cudaDecrypt(char *encrypted, char *key) {
    char* d_encrypted;
    char* d_decrypted;
    int msgLen = strlen(encrypted);
    int keyLen = strlen(key);

    cudaMalloc((void**)&d_encrypted, msgLen + 1);
    cudaMalloc((void**)&d_decrypted, msgLen + 1);

    cudaMemcpy(d_encrypted, encrypted, msgLen + 1, cudaMemcpyHostToDevice);

    cudaMemcpyToSymbol(d_key_calculated, key, keyLen + 1);

    int blockSize = 256;
    int numBlocks = (msgLen + blockSize - 1) / blockSize;
    vignere_decrypt_cuda<<<numBlocks, blockSize>>>(d_encrypted, d_decrypted, msgLen, keyLen);

    cudaDeviceSynchronize();

    char* decrypted = (char*)malloc(msgLen + 1);

    cudaMemcpy(decrypted, d_decrypted, msgLen + 1, cudaMemcpyDeviceToHost);

    cudaFree(d_encrypted);
    cudaFree(d_decrypted);
    return decrypted;
}




int main ()
{
    clock_t start = clock();


    char inputString[MAX_LENGTH]="Standard deviation is calculated as the square root of the variance Alternatively it is calculated by finding the mean of a data set finding the difference of each data point to the mean, squaring the differences adding them together dividing by the number of points in the data set less and finding the square root. Standard deviation is important because it can help users assess risk Consider an investment option with an average annual return of 10 per year However this average was derived from the past three year returns of many values By calculating the standard deviation and understanding your low likelihood of actually averaging 10 in any single given year your better armed to make informed decisions and recognizing underlying risk";
    char key[MAX_KEY]="SUJ";
    char * processedInput = process_string_CUDA(inputString);
    char * encrypted = cudaEncrypt(processedInput,key);
    int n = guess_key_length_cuda(encrypted);
    char* key_calculated = crack_vigenere_cuda(encrypted,n);

    char *calculatedPlainText=cudaDecrypt(encrypted,key_calculated);
    printf("%s",key_calculated);
    clock_t end = clock();

    // Calculate the time spent
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time spent: %f seconds\n", time_spent);


return 0;
}

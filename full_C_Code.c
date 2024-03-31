#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include <ctype.h> 
#include <math.h>
#include <time.h>
#define NUM_LETTERS 26 

char* only_alphabets(const char* text) {
    char* res = malloc(strlen(text) + 1);
    if (res == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    int j = 0; 
    for (int i = 0; text[i] != '\0'; i++) {
        if (isalpha((unsigned char)text[i])) {
            res[j++] = text[i]; 
        }
    }
    res[j] = '\0'; 

    return res;
}
typedef struct {
    int value;
    int index;
} ElementIndexPair;

ElementIndexPair* zip_index(int* arr, int size, int* outSize) {
    ElementIndexPair* result = (ElementIndexPair*)malloc(size * sizeof(ElementIndexPair));
    if (result == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < size; i++) {
        result[i].value = arr[i];
        result[i].index = i;
    }

    *outSize = size;

    return result;
}

void get_char_freq(const char* text, int freq[26]) {
    for (int i = 0; i < 26; i++) {
        freq[i] = 0;
    }

    while (*text) {
        if (isalpha((unsigned char)*text)) { 
            int index = toupper((unsigned char)*text) - 'A';
            freq[index]++;
        }
        text++;
    }
}

void vignere_encrypt(const char* msg, const char* key, char* encrypted) {
    int msgLen = strlen(msg);
    int keyLen = strlen(key);

    for (int i = 0; i < msgLen; i++) {
        if (isupper(msg[i])) {
            int msgIndex = msg[i] - 'A';
            int keyIndex = key[i % keyLen] - 'A';
            int encryptedIndex = (msgIndex + keyIndex) % NUM_LETTERS;
            encrypted[i] = 'A' + encryptedIndex;
        } else {
            encrypted[i] = msg[i];
        }
    }
    encrypted[msgLen] = '\0'; 
}
void vignere_decrypt(const char* encrypted_msg, const char* key, char* decrypted) {
    int msgLen = strlen(encrypted_msg);
    int keyLen = strlen(key);

    for (int i = 0; i < msgLen; i++) {
        if (isupper(encrypted_msg[i])) {
            int msgIndex = encrypted_msg[i] - 'A';
            int keyIndex = key[i % keyLen] - 'A';
            int decryptedIndex = (msgIndex - keyIndex + NUM_LETTERS) % NUM_LETTERS;
            decrypted[i] = 'A' + decryptedIndex;
        } else {
            decrypted[i] = encrypted_msg[i];
        }
    }
    decrypted[msgLen] = '\0'; 
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

int guess_key_length(const char* cipher) {
    int length = strlen(cipher);
    int* res = (int*)malloc(length * sizeof(int));
    for (int shift = 0; shift < length; shift++) {
        int matches = 0;
        for (int i = 0; i < length; i++) {
            if (cipher[i] == cipher[(i - shift + length) % length]) {
                matches++;
            }
        }
        res[shift] = matches;
    }
    double dat_means = mean(res + 1, length - 1);
    double dat_std_dev = sqrt(variance(res + 1, length - 1, dat_means));
    int* peaks = (int*)malloc(length * sizeof(int));
    int peaks_count = 0;
    for (int i = 0; i < length; i++) {
        if (res[i] >= dat_std_dev + dat_means) {
            peaks[peaks_count++] = i;
        }
    }
    int* peak_diff = (int*)malloc((peaks_count - 1) * sizeof(int));
    for (int i = 0; i < peaks_count - 1; i++) {
        peak_diff[i] = peaks[i + 1] - peaks[i];
    }

    int key_length = mode(peak_diff, peaks_count - 1);

    free(res);
    free(peaks);
    free(peak_diff);
    return key_length;
}
char* crack_vigenere(const char* cipher) {
    int n_key = guess_key_length(cipher);
    int cipher_len = strlen(cipher);
    char* keys = (char*)malloc(n_key + 1);

    for (int i = 0; i < n_key; i++) {
        int freq[NUM_LETTERS] = {0};
        for (int j = i; j < cipher_len; j += n_key) {
            if (isupper(cipher[j])) {
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
    keys[n_key] = '\0'; 

    return keys;
}
int main() {
    clock_t start = clock();

    char* message = "Standard deviation is calculated as the square root of the variance. Alternatively, it is calculated by finding the mean of a data set, finding the difference of each data point to the mean, squaring the differences, adding them together, dividing by the number of points in the data set less 1, and finding the square root. Standard deviation is important because it can help users assess risk. Consider an investment option with an average annual return of 10 per year. However, this average was derived from the past three year returns of 50, -15, and -5. By calculating the standard deviation and understanding your low likelihood of actually averaging 10 in any single given year, you're better armed to make informed decisions and recognizing underlying risk";
    size_t messageLen = strlen(message);
    char* repeatedMessage = (char*)malloc(messageLen * 10 + 1);
    for (int i = 0; i < 10; i++) {
        strcat(repeatedMessage, message);
    }

    char* filteredMessage = only_alphabets(repeatedMessage);
    for (int i = 0; filteredMessage[i]; i++) {
        filteredMessage[i] = toupper(filteredMessage[i]);
    }

    const char* key = "SUJ";
    char* encrypted = (char*)malloc(strlen(filteredMessage) + 1); 

    vignere_encrypt(filteredMessage, key, encrypted);
    char* guessedKey = crack_vigenere(encrypted);
    printf("%s\n", guessedKey);

    free(repeatedMessage); 
    free(filteredMessage);
    free(encrypted);
    free(guessedKey);
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Time spent: %f seconds\n", time_spent);

    return 0;
    return 0;
}
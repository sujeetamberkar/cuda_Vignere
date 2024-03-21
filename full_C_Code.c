#include <stdio.h>
#import<string.h>
#include <stdlib.h>
#include <ctype.h> // For isalpha()
#include <math.h>

#define NUM_LETTERS 26 // Number of letters in the English alphabet

// Function to filter only alphabet characters from a string
char* only_alphabets(const char* text) {
    // Allocate memory for the result, assuming all characters might be alphabets
    // +1 for the null terminator
    char* res = malloc(strlen(text) + 1);
    if (res == NULL) {
        // Handle memory allocation failure
        printf("Memory allocation failed.\n");
        exit(1);
    }

    int j = 0; // Index for the result string
    for (int i = 0; text[i] != '\0'; i++) {
        if (isalpha((unsigned char)text[i])) { // Check if the character is an alphabet
            res[j++] = text[i]; // Append the alphabet character to the result
        }
    }
    res[j] = '\0'; // Null-terminate the result string

    return res;
}
// Define a structure to hold an element and its index
typedef struct {
    int value;
    int index;
} ElementIndexPair;

// Function to create an array of ElementIndexPair from an array of integers
ElementIndexPair* zip_index(int* arr, int size, int* outSize) {
    // Allocate memory for the output array
    ElementIndexPair* result = (ElementIndexPair*)malloc(size * sizeof(ElementIndexPair));
    if (result == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // Populate the result array with elements and their indices
    for (int i = 0; i < size; i++) {
        result[i].value = arr[i];
        result[i].index = i;
    }

    // Set the output size
    *outSize = size;

    return result;
}

void get_char_freq(const char* text, int freq[26]) {
    // Initialize frequencies to 0 for all alphabet letters
    for (int i = 0; i < 26; i++) {
        freq[i] = 0;
    }

    // Go through each character in the text
    while (*text) {
        if (isalpha((unsigned char)*text)) { // Check if the character is an alphabet
            // Convert to uppercase and update its frequency
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
            // Assuming both msg and key are uppercase
            int msgIndex = msg[i] - 'A';
            int keyIndex = key[i % keyLen] - 'A';
            int encryptedIndex = (msgIndex + keyIndex) % NUM_LETTERS;
            encrypted[i] = 'A' + encryptedIndex;
        } else {
            // Copy the character as is if it's not an uppercase letter
            encrypted[i] = msg[i];
        }
    }
    encrypted[msgLen] = '\0'; // Null-terminate the encrypted string
}
void vignere_decrypt(const char* encrypted_msg, const char* key, char* decrypted) {
    int msgLen = strlen(encrypted_msg);
    int keyLen = strlen(key);

    for (int i = 0; i < msgLen; i++) {
        if (isupper(encrypted_msg[i])) {
            // Assuming both encrypted_msg and key are uppercase
            int msgIndex = encrypted_msg[i] - 'A';
            int keyIndex = key[i % keyLen] - 'A';
            int decryptedIndex = (msgIndex - keyIndex + NUM_LETTERS) % NUM_LETTERS;
            decrypted[i] = 'A' + decryptedIndex;
        } else {
            // Copy the character as is if it's not an uppercase letter
            decrypted[i] = encrypted_msg[i];
        }
    }
    decrypted[msgLen] = '\0'; // Null-terminate the decrypted string
}
// Function to calculate the mean of an array
double mean(const int arr[], int size) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum / size;
}

// Function to calculate the variance of an array
double variance(const int arr[], int size, double mean) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += (arr[i] - mean) * (arr[i] - mean);
    }
    return sum / size;
}

// Function to find the mode of an array
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

// Function to guess the key length of a Vigenère cipher
int guess_key_length(const char* cipher) {
    int length = strlen(cipher);
    int* res = (int*)malloc(length * sizeof(int));

    // Count matching characters for each possible shift
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

    // Find peaks
    int* peaks = (int*)malloc(length * sizeof(int));
    int peaks_count = 0;
    for (int i = 0; i < length; i++) {
        if (res[i] >= dat_std_dev + dat_means) {
            peaks[peaks_count++] = i;
        }
    }

    // Calculate differences between peaks
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
// Cracks the Vigenère cipher to guess the key
char* crack_vigenere(const char* cipher) {
    int n_key = guess_key_length(cipher);
    int cipher_len = strlen(cipher);
    char* keys = (char*)malloc(n_key + 1);

    for (int i = 0; i < n_key; i++) {
        int freq[NUM_LETTERS] = {0};
        // Populate frequency array for this segment
        for (int j = i; j < cipher_len; j += n_key) {
            if (isupper(cipher[j])) {
                freq[cipher[j] - 'A']++;
            }
        }
        
        // Find the most frequent character
        int max_freq = 0;
        char max_char = 0;
        for (int k = 0; k < NUM_LETTERS; k++) {
            if (freq[k] > max_freq) {
                max_freq = freq[k];
                max_char = k + 'A';
            }
        }
        
        // Assuming the most frequent character corresponds to 'E'
        keys[i] = ((max_char - 'E' + NUM_LETTERS) % NUM_LETTERS) + 'A';
    }
    keys[n_key] = '\0'; // Null-terminate the key string

    return keys;
}





int main() {

    // char* message = "Standard deviation is calculated as the square root of the variance. Alternatively, it is calculated by finding the mean of a data set, finding the difference of each data point to the mean, squaring the differences, adding them together, dividing by the number of points in the data set less 1, and finding the square root. Standard deviation is important because it can help users assess risk. Consider an investment option with an average annual return of 10 per year. However, this average was derived from the past three year returns of 50, -15, and -5. By calculating the standard deviation and understanding your low likelihood of actually averaging 10 in any single given year, you're better armed to make informed decisions and recognizing underlying risk";
    char* message = "ab";

    // Replicate message 10 tismes
    size_t messageLen = strlen(message);
    char* repeatedMessage = (char*)malloc(messageLen * 10 + 1);
    for (int i = 0; i < 10; i++) {
        strcat(repeatedMessage, message);
    }
    printf("%s",repeatedMessage);

    // // Filter to only alphabets and convert to uppercase
    // char* filteredMessage = only_alphabets(repeatedMessage);
    // for (int i = 0; filteredMessage[i]; i++) {
    //     filteredMessage[i] = toupper(filteredMessage[i]);
    // }

    // const char* key = "ABC";
    // char* encrypted = (char*)malloc(strlen(filteredMessage) + 1); // Allocate memory for the encrypted message

    // vignere_encrypt(filteredMessage, key, encrypted);
    // // printf("%s\n", encrypted);

    // char* guessedKey = crack_vigenere(encrypted);
    // printf("%s\n", guessedKey);
    // printf("The cracked key is %s\n", guessedKey);

    // // Free allocated memory
    // free(repeatedMessage); // Free memory of the repeated message
    // free(filteredMessage);
    // free(encrypted);
    // free(guessedKey);

    // return 0;
}
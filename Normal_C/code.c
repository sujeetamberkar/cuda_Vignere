#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define NUM_ASCII_CHARS 26
#define BASE_CHAR 'A'
# define MAX_CIPHER_LENGTH 3000

void only_alphabets_and_upper(const char *input, char *output) {
    while (*input != '\0') {
        if (isalpha((unsigned char)*input)) {
            *output++ = toupper((unsigned char)*input);
        }
        input++;
    }
    *output = '\0'; // Null-terminate the output string.
}
void vignere_encrypt(const char *msg, const char *key, char *result) {
    int msg_len = strlen(msg);
    int key_len = strlen(key);
    for (int i = 0; i < msg_len; i++) {
        int msg_index = msg[i] - 'A';
        int key_index = key[i % key_len] - 'A';
        result[i] = (msg_index + key_index) % NUM_ASCII_CHARS + 'A';
    }
    result[msg_len] = '\0'; // Null-terminate the result string.
}

void vignere_decrypt(const char *cipher, const char *key, char *decrypted) {
    int cipher_len = strlen(cipher);
    int key_len = strlen(key);
    for (int i = 0; i < cipher_len; i++) {
        int cipher_char_index = cipher[i] - BASE_CHAR;
        int key_char_index = key[i % key_len] - BASE_CHAR;
        // Perform the decryption step and ensure the result is positive before modding
        int decrypted_char_index = (cipher_char_index - key_char_index + NUM_ASCII_CHARS) % NUM_ASCII_CHARS;
        decrypted[i] = BASE_CHAR + decrypted_char_index;
    }
    decrypted[cipher_len] = '\0'; // Null-terminate the decrypted string
}
double calculate_mean(int arr[], int len) {
    double sum = 0;
    for (int i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum / len;
}

double calculate_std_deviation(int arr[], int len, double mean) {
    double sum = 0;
    for (int i = 0; i < len; i++) {
        sum += (arr[i] - mean) * (arr[i] - mean);
    }
    return sqrt(sum / (len - 1));
}

int calculate_mode(int arr[], int len) {
    int maxValue = 0, maxCount = 0, i, j;

    for (i = 0; i < len; ++i) {
        int count = 0;
        for (j = 0; j < len; ++j) {
            if (arr[j] == arr[i])
                ++count;
        }

        if (count > maxCount) {
            maxCount = count;
            maxValue = arr[i];
        }
    }

    return maxValue;
}

int guess_key_length(const char *cipher) {
    int len = strlen(cipher);
    int res[MAX_CIPHER_LENGTH] = {0};
    int peak_diffs[MAX_CIPHER_LENGTH] = {0};
    int pd_index = 0;

    for (int shift = 0; shift < len; shift++) {
        int matches = 0;
        for (int i = 0; i < len; i++) {
            if (cipher[i] == cipher[(i - shift + len) % len]) {
                matches++;
            }
        }
        res[shift] = matches;
    }

    double mean = calculate_mean(&res[1], len - 1); // Exclude the first element
    double std_dev = calculate_std_deviation(&res[1], len - 1, mean);

    for (int i = 1; i < len; i++) { // Start from 1 to exclude the full match
        if (res[i] > mean + std_dev) {
            peak_diffs[pd_index++] = i;
        }
    }

    int peak_diffs_filtered[MAX_CIPHER_LENGTH] = {0};
    for (int i = 0; i < pd_index - 1; i++) {
        peak_diffs_filtered[i] = peak_diffs[i + 1] - peak_diffs[i];
    }

    return calculate_mode(peak_diffs_filtered, pd_index - 1);
}
void calculate_frequency(const char *text, int text_length, int *freq) {
    for (int i = 0; i < NUM_ASCII_CHARS; i++) {
        freq[i] = 0; // Initialize frequencies
    }

    for (int i = 0; i < text_length; i++) {
        if (isalpha(text[i])) {
            freq[toupper(text[i]) - 'A']++;
        }
    }
}


// Function to find the key using frequency analysis
void find_key(const char *cipher, int key_length, char *key) {
    int segment_length = strlen(cipher) / key_length;
    int freq[NUM_ASCII_CHARS];
    char segment[MAX_CIPHER_LENGTH];

    for (int i = 0; i < key_length; i++) {
        int segment_index = 0;

        // Collect every Nth letter for frequency analysis
        for (int j = i; j < strlen(cipher); j += key_length) {
            segment[segment_index++] = cipher[j];
        }

        calculate_frequency(segment, segment_index, freq);

        // Find the most frequent letter in this segment
        int max_freq = 0, letter_index = 0;
        for (int k = 0; k < NUM_ASCII_CHARS; k++) {
            if (freq[k] > max_freq) {
                max_freq = freq[k];
                letter_index = k;
            }
        }

        // Assuming the most frequent letter corresponds to 'E'
        int key_letter_index = (letter_index + NUM_ASCII_CHARS - ('E' - 'A')) % NUM_ASCII_CHARS;
        key[i] = key_letter_index + 'A';
    }

    key[key_length] = '\0'; // Null-terminate the key string
}
// Function to calculate the frequency of each letter in a segment


int main()
{
    const char *original_message ="The Vigenère cipher is a method of encrypting alphabetic text by using a simple form of polyalphabetic substitution. A polyalphabetic cipher is any cipher based on substitution, using multiple substitution alphabets. The encryption of the original text is done using the Vigenère square or Vigenère table. The table consists of the alphabet written out 26 times in different rows, each alphabet shifted cyclically to the left compared to the previous alphabet, corresponding to the 26 possible Caesar ciphers. At different points in the encryption process, the cipher uses a different alphabet from one of the rows. The key determines the number of positions each letter is shifted. The Vigenère cipher is thus considered to be a more secure form of the Caesar cipher, which uses a shift of a single alphabet. The use of a key and the polyalphabetic nature of the encryption process allow the Vigenère cipher to be more secure against simple frequency analysis attacks. For a long time, this cipher was regarded as unbreakable due to its complexity and the vast number of permutations. It was finally broken in the 19th century with the development of more sophisticated cryptanalysis techniques. However, the Vigenère cipher remains an important part of the history of cryptography and an interesting study in the evolution of encryption methods. Despite its vulnerabilities to modern analysis techniques, it offers a fascinating glimpse into the past efforts to secure communication and the ingenious methods developed to achieve this goal. The study of such ciphers not only enriches our understanding of historical cryptography but also provides valuable lessons on the importance of adapting and evolving encryption methods to counteract advancements in cryptanalysis. As we continue to develop and refine our cryptographic techniques, the lessons learned from the Vigenère cipher and other historical methods will undoubtedly continue to inform our approaches to securing information in the digital age.";
    char filtered_message[3000];
    char encrypted_message[3000];
    char answer[3000]; 
    const char *key = "HOLLY";


    // printf("%lu",strlen(original_message));
    only_alphabets_and_upper(original_message, filtered_message);
    // printf("%s",filtered_message);

    vignere_encrypt(filtered_message, key, encrypted_message);
    // printf("%s",encrypted_message);


    int key_length = guess_key_length(encrypted_message);
    // printf("%d",key_length);


    char calculatedKey [100];
    find_key(encrypted_message, key_length, calculatedKey);
    // printf("%s",calculatedKey);



    
    vignere_decrypt(encrypted_message, calculatedKey,answer);
    printf("%s",answer);





    return 0;   
}
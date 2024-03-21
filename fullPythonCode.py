import math
import statistics as st

NUM_ASCII_CHARS = 26 # 26 for alphabets and 128 for ascii
BASE_CHAR = ord('A') # ord('A') for alphabets and 0 for ascii 

def only_alphabets(text: str):
    res = ''
    for c in text:
        if c in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ': res += c
    return res

def zip_index(arr: list):
    return [(arr[i], i) for i in range(len(arr))]

def get_char_freq(text):
    text = text.upper()
    freq = {}
    for c in text: freq[c] = freq.get(c, 0) + 1
    return freq

##################################################################################################
## Vignere Cipher

def vignere_encrypt(msg:  str, key: str):
    assert len(key) <= len(msg)
    return ''.join([chr(BASE_CHAR + (((ord(msg[i]) - BASE_CHAR) + (ord(key[i % len(key)]) - BASE_CHAR) ) % NUM_ASCII_CHARS)) for i in range(len(msg))])

def vignere_decrypt(msg:  str, key: str):
    assert len(key) <= len(msg)
    return ''.join([chr(BASE_CHAR + (((ord(msg[i]) - BASE_CHAR) - (ord(key[i % len(key)]) - BASE_CHAR) ) % NUM_ASCII_CHARS)) for i in range(len(msg))])

def guess_key_length(cipher: str):
    res = []
    for shift in range(len(cipher)):
        matches = 0
        for i in range(len(cipher)):
            matches += cipher[i] == cipher[(i - shift + len(cipher)) % len(cipher)]
        res.append(matches)

    dat_means = st.mean(res[1:])
    dat_std_dev   = math.sqrt(st.variance(res[1:]))

    peaks = []
    for i in range(len(res)):
        if res[i] >= dat_std_dev + dat_means: peaks.append(i)
    peak_diff = []
    for i in range(len(peaks) - 1):
        peak_diff.append(peaks[i+1] - peaks[i])

    return st.mode(peak_diff)

def crack_vignere(cipher: str) -> str:
    n_key = guess_key_length(cipher)
    divisions = ['' for i in range(n_key)]
    for i in range(len(cipher)):
        divisions[i % n_key] += cipher[i]
    
    keys = ''
    for i in range(n_key):
        freq = sorted(get_char_freq(divisions[i]).items(), key = lambda x: x[1], reverse = True)
        keys += chr(BASE_CHAR + ((ord(freq[0][0]) - ord('E')) % NUM_ASCII_CHARS) )
    return keys
    
##################################################################################################

def main():
    # print(vignere_encrypt("HELLO WORLD","KEY"))
    message = "Standard deviation is calculated as the square root of the variance. Alternatively, it is calculated by finding the mean of a data set, finding the difference of each data point to the mean, squaring the differences, adding them together, dividing by the number of points in the data set less 1, and finding the square root. Standard deviation is important because it can help users assess risk. Consider an investment option with an average annual return of 10 per year. However, this average was derived from the past three year returns of 50, -15, and -5. By calculating the standard deviation and understanding your low likelihood of actually averaging 10 in any single given year, you're better armed to make informed decisions and recognizing underlying risk "
    message = only_alphabets(message * 10).upper()
    key = "SUJ"
    
    crypted = vignere_encrypt(message, key)
    print(crypted)

    guess = crack_vignere(crypted)
    print(guess)
    print(f'The cracked key is {guess}')

if __name__ == '__main__': main()
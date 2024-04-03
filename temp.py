import math
import statistics as st
import matplotlib.pyplot as plt

NUM_ASCII_CHARS = 26 # 26 for alphabets and 128 for ascii
BASE_CHAR = ord('A') # ord('A') for alphabets and 0 for ascii 

def only_alphabets(text: str):
    res = ''
    for c in text:
        if c in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ': res += c
    return res

def zip_index(arr: list):
    return [(arr[i], i) for i in range(len(arr))]

def vignere_encrypt(msg:  str, key: str):
    assert len(key) <= len(msg)
    return ''.join([chr( ((ord(msg[i]) + ord(key[i % len(key)])) % NUM_ASCII_CHARS) + BASE_CHAR ) for i in range(len(msg))])

def vignere_decrypt(msg:  str, key: str):
    assert len(key) <= len(msg)
    return ''.join([chr( ((ord(msg[i]) - ord(key[i % len(key)])) % NUM_ASCII_CHARS) + BASE_CHAR ) for i in range(len(msg))])

def guess_key_length(cipher: str):
    res = []
    for shift in range(len(cipher)):
        matches = 0
        for i in range(len(cipher)):
            matches += cipher[i] == cipher[(i - shift + len(cipher)) % len(cipher)]
        res.append(matches)

    dat_means = st.mean(res)
    dat_var   = st.variance(res)
    sq = math.sqrt(dat_var)
    vars = []
    """
    for m in res:
        vars.append(())
    """

    plt.bar(list(range(len(res))), res, width=0.3)
    plt.grid(True)
    plt.show()
    peaks = list(zip(mods, list(range(len(mods)))))
    peaks.sort(key=lambda x: x[0], reverse=True)
    return peaks[0][1]


def main():
    message = """The Vigenère cipher (French pronunciation: ​[viʒnɛːʁ])
      is a method of encrypting alphabetic text by using a series of 
      interwoven Caesar ciphers, based on the letters of a keyword. 
      It employs a form of polyalphabetic substitution.[1][2] First 
      described by Giovan Battista Bellaso in 1553, the cipher is easy
      to understand and implement, but it resisted all attempts to break
        it until 1863, three centuries later. This earned it the 
        description le chiffrage indéchiffrable (French for 'the 
        indecipherable cipher'). Many people have tried to implement 
        encryption schemes that are essentially Vigenère ciphers.[3] In 
        1863, Friedrich Kasiski was the first to publish a general method 
        of deciphering Vigenère ciphers."""
    
    message = only_alphabets(message).upper()
    key = "KEYS"
    crypted = vignere_encrypt(message, key)
    guess_key_length(crypted)

    decrypted = vignere_decrypt(crypted, key)
    print(f'Message: "{message}"\n\nCrypted: "{crypted}"\n\nDecrypted: "{decrypted}"')

if __name__ == '__main__': main()
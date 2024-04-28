# Advance Encrytion Standard (AES)
Implementation of AES 128 Bit Encryption, Decryption and Key Expansion Algorithm in C++

## Structure

### Key Expansion
- The Key Expansion class expands the key into 11 round keys to be used through out the encryption and decryption process.
- Key generation is usually scheduled but in this implementation, round keys are generated and stored globally at the start.
- The Encryption and Decryption classes then utilize these keys in ascending and descending order respectively.

![image](https://github.com/MuhammadHabibKhan/aes/assets/92048010/bcdb9d1e-979f-4dbb-a0e4-b3054c038c0a)


### Encryption & Decyption
- Encryption and Decryption classes contain seperate functions for each step of the algorithm as depicted in the image below.
- The functions createPlainMatrix() in Encryption and createCipherMatrix() in Decryption class outputs the matrix after the XOR operation with the initial round key.
- The initial round key for the first is W(0-3) and W(40-43) for the latter.

![image](https://github.com/MuhammadHabibKhan/aes/assets/92048010/f962b6a9-3163-481d-9434-68e254977267)


### AES Class
- Glues together the Key Expansion, Encryption and Decryption class and their functionality.
- It implements the algoirthm by executing each step for the required amount of rounds.
- Creates 16-byte packets of input data as well.
- Padding bits are added in the Encryption class.

## Resources
- You may verify the code's integrity and working through [this](https://legacy.cryptool.org/en/cto/aes-step-by-step) website.
- This [animation](https://legacy.cryptool.org/en/cto/aes-animation) of AES may help you understand the alogrithm better.

## Note
- The code is set to work on 128 bit AES alogrithm however certain elements of the code were designed to allow for easier incorporation of higher order bit AES.
- Needless to say, a good chunk code will need to be changed to work on 192 or 256 bit other than the simple increase in number of rounds.
- The material on the specifics of 192 or 256 bit AES was lackluster but there is significant changes in the way Key Expansion happens compared to 128-bit.
- No header file is bad I know know, will add later.

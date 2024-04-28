# Advance Encrytion Standard (AES)
Implementation of AES 128 Bit Encryption, Decryption and Key Expansion Algorithm in C++

## Code Structure

### Key Expansion Class
- Key Expansion class expands the key into 11 round keys
- Round keys are generated and stored globally in a vector instead of being scheduled
- Encryption & Decryption class utilizes them in ascending and descending order respectively


&nbsp; ![image](https://github.com/MuhammadHabibKhan/aes/assets/92048010/bcdb9d1e-979f-4dbb-a0e4-b3054c038c0a)


### Encryption & Decyption Class
- Encryption and Decryption class contain seperate functions for each step of the algorithm
- These steps are as depicted in the image below
- Output of plain and cipher text matrix displayed after XOR operation with initial round key


<!--- ![image](https://github.com/MuhammadHabibKhan/aes/assets/92048010/f962b6a9-3163-481d-9434-68e254977267)  -->
![image](https://github.com/MuhammadHabibKhan/aes/assets/92048010/c4cceeb5-82dc-4d16-86de-44aefc818ee8)


### AES Class
- Glues together the KeyExpansion, Encryption and Decryption class and their functionalities
- Creates 16-byte packets of input data
- Padding bits are added in the Encryption class
- Implements the algorithm by executing each step for the required amount of rounds

## Resources
- You may verify the code's integrity and working through [this](https://legacy.cryptool.org/en/cto/aes-step-by-step) website
- This [animation](https://legacy.cryptool.org/en/cto/aes-animation) of AES may help you understand the alogrithm better

## Note
- The code is set to work on 128-bit AES alogrithm
- However certain elements of the code were designed to allow for easier incorporation of higher-order bit AES
- Needless to say, simply increasing the number of rounds won't cut it
- A good chunk code will change to work on 192 or 256-bit algorithm
- It includes significant changes in the way Key Expansion occurs compared to 128-bit
- PS: No header file is included (bad habit I know), will add later

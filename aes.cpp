#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>
#include <array>

using namespace std;

vector<array<array<string, 4>, 4>> KEY_POOL; // in binary
vector<array<array<string, 4>, 4>> CIPHER_TEXT;

const vector <string> ROUND_CONSTANT(
    {
        "01000000", "02000000", "04000000", "08000000", "10000000", 
        "20000000", "40000000", "80000000", "1B000000", "36000000"
    }
);

const array<array<string, 16>, 16> S_BOX = {
        {
            {"63", "7c", "77", "7b", "f2", "6b", "6f", "c5", "30", "01", "67", "2b", "fe", "d7", "ab", "76"},
            {"ca", "82", "c9", "7d", "fa", "59", "47", "f0", "ad", "d4", "a2", "af", "9c", "a4", "72", "c0"},
            {"b7", "fd", "93", "26", "36", "3f", "f7", "cc", "34", "a5", "e5", "f1", "71", "d8", "31", "15"},
            {"04", "c7", "23", "c3", "18", "96", "05", "9a", "07", "12", "80", "e2", "eb", "27", "b2", "75"},
            {"09", "83", "2c", "1a", "1b", "6e", "5a", "a0", "52", "3b", "d6", "b3", "29", "e3", "2f", "84"},
            {"53", "d1", "00", "ed", "20", "fc", "b1", "5b", "6a", "cb", "be", "39", "4a", "4c", "58", "cf"},
            {"d0", "ef", "aa", "fb", "43", "4d", "33", "85", "45", "f9", "02", "7f", "50", "3c", "9f", "a8"},
            {"51", "a3", "40", "8f", "92", "9d", "38", "f5", "bc", "b6", "da", "21", "10", "ff", "f3", "d2"},
            {"cd", "0c", "13", "ec", "5f", "97", "44", "17", "c4", "a7", "7e", "3d", "64", "5d", "19", "73"},
            {"60", "81", "4f", "dc", "22", "2a", "90", "88", "46", "ee", "b8", "14", "de", "5e", "0b", "db"},
            {"e0", "32", "3a", "0a", "49", "06", "24", "5c", "c2", "d3", "ac", "62", "91", "95", "e4", "79"},
            {"e7", "c8", "37", "6d", "8d", "d5", "4e", "a9", "6c", "56", "f4", "ea", "65", "7a", "ae", "08"},
            {"ba", "78", "25", "2e", "1c", "a6", "b4", "c6", "e8", "dd", "74", "1f", "4b", "bd", "8b", "8a"},
            {"70", "3e", "b5", "66", "48", "03", "f6", "0e", "61", "35", "57", "b9", "86", "c1", "1d", "9e"},
            {"e1", "f8", "98", "11", "69", "d9", "8e", "94", "9b", "1e", "87", "e9", "ce", "55", "28", "df"},
            {"8c", "a1", "89", "0d", "bf", "e6", "42", "68", "41", "99", "2d", "0f", "b0", "54", "bb", "16"}
        }
};

const array<array<int, 4>, 4> PRE_DEFINED_MATRIX = {
        {
            {2, 3, 1, 1},
            {1, 2, 3, 1},
            {1, 1, 2, 3},
            {3, 1, 1, 2}
        }
};

const array<array<int, 4>, 4> INV_PRE_DEFINED_MATRIX = {
        {
            {14, 11, 13, 9},
            {9, 14, 11, 13},
            {13, 9, 14, 11},
            {11, 13, 9, 14}
        }
};

// conversion functions (replace the manual conversions with these functions later)

string binaryXOR(const string& a, const string& b) {

    // Ensure both strings have the same length
    if (a.length() != b.length()) {
        std::cerr << "Binary strings must have the same length." << std::endl;
        return "";
    }
    // Initialize result string
    string result = "";

    // Perform XOR operation on corresponding bits
    for (size_t i = 0; i < a.length(); ++i) {
        result += (a[i] == b[i]) ? '0' : '1';
    }
    return result;
}

string decimalToHex(unsigned int decimal) {
    stringstream ss;
    ss << hex << decimal;
    return ss.str();
}

string hexToBinary(const string& hexString) {
    string binaryString;
    
    for (char c : hexString) 
    {
        int value;
        if (c >= '0' && c <= '9') {
            value = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            value = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            value = c - 'A' + 10;
        } else {
            return "";
        }
        binaryString += bitset<4>(value).to_string();
    }
    return binaryString;
}

string binaryToHex(const string& binaryString) 
{
    string hexString;
    unsigned int decimalValue = stoul(binaryString, nullptr, 2);
    hexString = decimalToHex(decimalValue);
    return hexString;
}

int galois_multiply(int a, int b) 
{
    unsigned char result = 0;
    unsigned char carry;
    
    for (int i = 0; i < 8; ++i) 
    {
        if (b & 1) 
        {
            result ^= static_cast<unsigned char>(a);
        }
        carry = static_cast<unsigned char>(a) & 0x80; // check if leftmost bit is set
        a <<= 1;
        
        if (carry) 
        {
            a ^= 0x1B; // 0x1B is the irreducible polynomial for AES
        }
        b >>= 1;
    }
    return static_cast<int>(result);
}


class KeyExpansion
{
    public: string key;

    KeyExpansion() {}

    KeyExpansion(string k)
    {
        this->key = k;
    }

    void InitialRoundKey()
    {
        // To be XORed with Plain Text Matrix before any encryption begins

        int index = 0;
        array<array<string, 4>, 4> KEY_MATRIX;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (index < key.length())
                {
                    char key_char = key[index++];
                    int ascii_value = static_cast<int>(key_char);

                    string binary_string_8bit = bitset<8>(ascii_value).to_string();
                    KEY_MATRIX[j][i] = binary_string_8bit; // binary of key char stored column-wise
                }
                else
                {
                    cout << "Error: Key out of bounds" << endl;
                    return;
                }
            }
            // cout << " |" << endl;
        }
        KEY_POOL.push_back(KEY_MATRIX);
    }

    void expandKey()
    {
        // create word array and w'

        for (int k = 1; k < 11; k++) // 11 round keys for 128-bit | 10 here since InitialRoundKey() generates first one
        {
            array<string, 4> WORD_ARRAY;
            array<string, 4> WORD_ARRAY_OUTPUT;
            array<array<string, 4>, 4> KEY_MATRIX;

            for (int i = 0; i < 4; i++)
            {
                string word;

                for (int j = 0; j < 4; j++)
                {
                    word += KEY_POOL[k - 1][j][i];
                }
                WORD_ARRAY[i] = word;
            }
            string w_dash = gFunction(WORD_ARRAY[3], k); // w' calculated using the last word

            // generate new word array

            for (int i = 0; i < 4; i++)
            {
                if (i == 0)
                {
                    string xor_result = binaryXOR(WORD_ARRAY[i], w_dash);
                    WORD_ARRAY_OUTPUT[i] = xor_result;
                }
                else
                {
                    string xor_result = binaryXOR(WORD_ARRAY[i], WORD_ARRAY_OUTPUT[i-1]);
                    WORD_ARRAY_OUTPUT[i] = xor_result;
                }
            }
            // convert word array into 4x4 byte matrix to append into KEY POOL

            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    KEY_MATRIX[j][i] = WORD_ARRAY_OUTPUT[i].substr(j * 8, 8);
                }
            }
            KEY_POOL.push_back(KEY_MATRIX);
        }
    }

    string gFunction(string word, int round)
    {
        array<string, 4> BYTE_ARRAY;
        array<string, 4> BYTE_ARRAY_SHIFT;
        array<string, 4> BYTE_ARRAY_SUB;

        string w_dash;

        for (int i=0; i < 4; i++)
        {
            BYTE_ARRAY[i] =  word.substr(i * 8, 8);
        }

        // 1) One byte circular left shift

        for (int i = 0; i < 4; i++)
        {
            // int index = (i + 1) % 4;
            BYTE_ARRAY_SHIFT[i] = BYTE_ARRAY[(i + 1) % 4];
        }

        // 2) S_BOX substitution
        
        for (int i = 0; i < 4; i++)
        {
            string first_4_bits = BYTE_ARRAY_SHIFT[i].substr(0, 4);
            string last_4_bits = BYTE_ARRAY_SHIFT[i].substr(4, 4);

            int row_index = stoi(first_4_bits, nullptr, 2);
            int col_index = stoi(last_4_bits, nullptr, 2);

            int decimal_value = stoi(S_BOX[row_index][col_index], nullptr, 16); // hex to decimal
            BYTE_ARRAY_SUB[i] = bitset<8>(decimal_value).to_string(); // decimal to binary
        }

        // 3) XORing with Round Constant

        string rc_binary = hexToBinary(ROUND_CONSTANT[round - 1]);
        // cout << "rc binary: " << rc_binary << endl;
        
        for (int i = 0; i < 4; i++)
        {
            string xor_result = binaryXOR(BYTE_ARRAY_SUB[i], rc_binary.substr(i*8, 8));
            w_dash += xor_result;
        }
        return w_dash;
    }

};

class Encryption
{
    public:

    string input_string;

    int PT_MATRIX_INT[4][4];
    string PT_MATRIX_HEX[4][4];

    string STATE_ARRAY_SUB_BYTES[4][4];
    string STATE_ARRAY_SHIFT_ROWS[4][4];
    string STATE_ARRAY_MIX_COLUMN[4][4];

    vector<array<array<string, 4>, 4>> ADD_ROUND_KEY_OUTPUT;

    Encryption() {}

    Encryption(string ip_str)
    {
        this->input_string = ip_str;
    }

    void createPlainMatrix()
    {
        cout << "--------- PLAIN TEXT ---------" << endl;
        
        int index = 0;
        array<array<string, 4>, 4> INITIAL_ROUND_KEY = KEY_POOL[0];

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (index < input_string.length())
                {
                    char input_char = input_string[index++];
                    int ascii_value = static_cast<int>(input_char);

                    string binary_string_8bit = bitset<8>(ascii_value).to_string();

                    // convert binary into integer
                    int int_value = stoi(binary_string_8bit, nullptr, 2);

                    // XORing plain text with Initial Round Key
                    string round_key_binary = INITIAL_ROUND_KEY[j][i];
                    int round_key_decimal = stoi(round_key_binary, nullptr, 2);

                    int result = int_value ^ round_key_decimal;
                    PT_MATRIX_INT[j][i] = result; // input stored column-wise in AES / Block Ciphers

                    // Convert integer to hexadecimal string
                    stringstream ss;
                    ss << hex << result; // set hex as format for string stream before inserting
                    string hex_string = ss.str(); //extract contents from stream as string
                    
                    PT_MATRIX_HEX[j][i] = hex_string;
                }
                else
                {
                    // PKCS#7 Padding => amount of bytes needed for padding are added as padding
                    int padding_value = 16 - input_string.length();
                    int round_key_xor = padding_value ^ stoi(INITIAL_ROUND_KEY[j][i], nullptr, 2);
                    PT_MATRIX_INT[j][i] = round_key_xor;

                    stringstream pv;
                    pv << hex << round_key_xor;
                    string padding_value_hex = pv.str();
                    PT_MATRIX_HEX[j][i] = padding_value_hex;
                }
            }    
        }
        // print 
        for (int i=0; i<4; i++)
        {
            for (int j=0; j<4; j++)
            {
                if (PT_MATRIX_HEX[i][j].length() > 1)
                {
                    cout << " | " << PT_MATRIX_HEX[i][j];
                }
                else
                {
                    cout << " |  " << PT_MATRIX_HEX[i][j];
                }
            }
            cout << " |" << endl;
        }
    }

    void subBytes(int round)
    {
        cout << "--------- SUB BYTES ---------" << endl;

        for (int i=0; i < 4; i++)
        {
            for (int j=0; j < 4; j++)
            {
                string plain_char_hex;

                if (round == 0)
                {
                    plain_char_hex = PT_MATRIX_HEX[i][j]; // use plain-text matrix
                }
                else
                {
                    plain_char_hex = ADD_ROUND_KEY_OUTPUT[round-1][i][j]; // use output from previous rounds if round > 0
                }

                string first_char(1, plain_char_hex[0]);
                string second_char(1, plain_char_hex[1]);

                int row_index = stoi(first_char, nullptr, 16);
                int col_index;

                try
                {
                    col_index = stoi(second_char, nullptr, 16); // if fails then single digit hex
                }
                catch(const exception& e)
                {
                    // swap col and row as first digit is 0 if none present 
                    col_index = row_index;
                    row_index = 0;
                }

                stringstream ss;
                ss << S_BOX[row_index][col_index];
                string sub_byte_hex = ss.str();

                STATE_ARRAY_SUB_BYTES[i][j] = sub_byte_hex;
                cout << " | " << S_BOX[row_index][col_index];
            }
            cout << " |" << endl;
        }
    }

    void shiftRows()
    {
        cout << "--------- SHIFT ROWS ---------" << endl;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                STATE_ARRAY_SHIFT_ROWS[i][j] = STATE_ARRAY_SUB_BYTES[i][(j + i) % 4];

                cout << " | " << STATE_ARRAY_SHIFT_ROWS[i][j];
            }
            cout << " |" << endl;
        }
    }

    void mixColumn()
    {
        cout << "--------- MIX COLUMN ---------" << endl;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                int gf_xor = 0;

                for (int k = 0; k < 4; k++)
                {
                    int pre_defined_matrix_int = PRE_DEFINED_MATRIX[i][k];
                    int state_array_int = stoi(STATE_ARRAY_SHIFT_ROWS[k][j], nullptr, 16); // hex to decimal

                    // int gf_mul = galois_single_multiply(pre_defined_matrix_int, state_array_int, 8);
                    int gf_mul = galois_multiply(pre_defined_matrix_int, state_array_int);

                    if (k != 0)
                    {
                        int temp = gf_xor ^ gf_mul; // xor second value with first stored in variable
                        gf_xor = temp; // replace with newer value
                    }
                    else 
                    {
                        gf_xor = gf_mul; // first only store value
                    }
                }            
                stringstream ss;
                ss << hex << gf_xor;
                string hex_xor = ss.str();

                STATE_ARRAY_MIX_COLUMN[i][j] = hex_xor;
                
                // print formatting
                if (STATE_ARRAY_MIX_COLUMN[i][j].length() > 1)
                {
                    cout << " | " << STATE_ARRAY_MIX_COLUMN[i][j];
                }
                else
                {
                    cout << " |  " << STATE_ARRAY_MIX_COLUMN[i][j];
                }
            }
            cout << " |" << endl;
        }
    }

    void addRoundKey(int round)
    {

        array<array<string, 4>, 4> ROUND_KEY_MATRIX = KEY_POOL[round + 1]; // + 1 since 0th index is initial round key already used
        array<array<string, 4>, 4> OUTPUT;

        cout << "--------- ROUND KEY MATRIX ---------" << endl;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                cout << " | " << hex << stoi(ROUND_KEY_MATRIX[i][j], nullptr, 2) << dec;
            }
            cout << " |" << endl;
        }

        cout << "--------- ROUND KEY OUTPUT---------" << endl;
        
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                int mix_column_decimal = stoi(STATE_ARRAY_MIX_COLUMN[i][j], nullptr, 16);
                string mix_col_binary = bitset<8>(mix_column_decimal).to_string();

                string xor_result = binaryXOR(mix_col_binary, ROUND_KEY_MATRIX[i][j]);
                unsigned long long int xor_decimal = stoull(xor_result, nullptr, 2);

                stringstream ss;
                ss << hex << xor_decimal;
                string hex_string = ss.str();

                OUTPUT[i][j] = hex_string;
                
                if (OUTPUT[i][j].length() > 1)
                {
                    cout << " | " << OUTPUT[i][j];
                }
                else
                {
                    cout << " |  " << OUTPUT[i][j];
                }
            }
            cout << " |" << endl;
        }
        ADD_ROUND_KEY_OUTPUT.push_back(OUTPUT);
    }

    void addLastRoundKey(int round)
    {

        array<array<string, 4>, 4> ROUND_KEY_MATRIX = KEY_POOL[round + 1]; // + 1 since 0th index is initial round key already used
        array<array<string, 4>, 4> OUTPUT;
        
        cout << "--------- ROUND KEY MATRIX ---------" << endl;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                cout << " | " << hex << stoi(ROUND_KEY_MATRIX[i][j], nullptr, 2) << dec;
            }
            cout << " |" << endl;
        }

        cout << "--------- LAST ROUND KEY ---------" << endl;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                int mix_column_decimal = stoi(STATE_ARRAY_SHIFT_ROWS[i][j], nullptr, 16);
                string mix_col_binary = bitset<8>(mix_column_decimal).to_string();

                string xor_result = binaryXOR(mix_col_binary, ROUND_KEY_MATRIX[i][j]);
                unsigned long long int xor_decimal = stoull(xor_result, nullptr, 2);

                stringstream ss;
                ss << hex << xor_decimal;
                string hex_string = ss.str();

                OUTPUT[i][j] = hex_string;
                
                if (OUTPUT[i][j].length() > 1)
                {
                    cout << " | " << OUTPUT[i][j];
                }
                else
                {
                    cout << " |  " << OUTPUT[i][j];
                }
            }
            cout << " |" << endl;
        }
        // ADD_ROUND_KEY_OUTPUT.push_back(OUTPUT);
        CIPHER_TEXT.push_back(OUTPUT); // push last round key output that cotains cipher text to the CIPHER_TEXT vector stored globally
    }

};

class Decryption
{
    public: string decrypted_message; 
    
    string STATE_ARRAY_INV_SUB_BYTES[4][4];
    string STATE_ARRAY_INV_SHIFT_ROWS[4][4];
    string STATE_ARRAY_INV_ROUND_KEY[4][4];

    array<array<string, 4>, 4> CIPHER_TEXT_MATRIX;
    vector<array<array<string, 4>, 4>> INV_MIX_COLUMN_OUTPUT;

    Decryption(int input)
    {
        this->CIPHER_TEXT_MATRIX = CIPHER_TEXT[input]; // From ADD_ROUND_KEY_OUTPUT that contains cipher text
    }

    void createCipherMatrix()
    {
        // adding initial round key
        array<array<string, 4>, 4> INIT_ROUND_KEY = KEY_POOL[KEY_POOL.size() - 1]; // w[40-43]

        cout << "--------- CIPHER TEXT ---------" << endl;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                unsigned int ct_char_decimal = stoul(CIPHER_TEXT_MATRIX[i][j], nullptr, 16);
                string ct_char_binary = bitset<8>(ct_char_decimal).to_string();

                string ct_char_hex = binaryToHex(binaryXOR(ct_char_binary, INIT_ROUND_KEY[i][j]));
                CIPHER_TEXT_MATRIX[i][j] = ct_char_hex;

                cout << " | " << CIPHER_TEXT_MATRIX[i][j];
            }
            cout << " |" << endl;
        }
    }

    void invShiftRows(int round)
    {
        cout << "--------- INVERSE SHIFT ROWS ---------" << endl;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (round == 0)
                {
                    STATE_ARRAY_INV_SHIFT_ROWS[i][(j + i) % 4] = CIPHER_TEXT_MATRIX[i][j];
                }
                else
                {
                    // cout << "inv mix: " << INV_MIX_COLUMN_OUTPUT[round - 1][i][j] << endl;
                    STATE_ARRAY_INV_SHIFT_ROWS[i][(j + i) % 4] = INV_MIX_COLUMN_OUTPUT[round - 1][i][j];
                }
            }
        }
        // print
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (STATE_ARRAY_INV_SHIFT_ROWS[i][j].length() > 1)
                {
                    cout << " | " << STATE_ARRAY_INV_SHIFT_ROWS[i][j];
                }
                else
                {
                    cout << " |  " << STATE_ARRAY_INV_SHIFT_ROWS[i][j];
                }
            }
            cout << " |" << endl;
        }
    }

    void invSubBytes(int round)
    {
        cout << "--------- INVERSE SUB BYTES ---------" << endl;

        for (int i=0; i < 4; i++)
        {
            for (int j=0; j < 4; j++)
            {
                string cipher_char_hex = STATE_ARRAY_INV_SHIFT_ROWS[i][j];
                string cipher_char_hex_updated;

                if (cipher_char_hex.length() == 1)
                {
                    cipher_char_hex_updated += "0";
                    cipher_char_hex_updated += cipher_char_hex;
                    cipher_char_hex = cipher_char_hex_updated;
                }
                string row_col_hex;

                for (int k = 0; k < 16; k++)
                {
                    for (int l = 0; l < 16; l++)
                    {
                        if (S_BOX[k][l] == cipher_char_hex)
                        {
                            // cout << "sbox: " << S_BOX[k][l] << " cipher char: " << cipher_char_hex << endl;
                            string row_hex = decimalToHex(k);
                            string col_hex = decimalToHex(l);
                            // cout << "row: " << row_hex << " col: " << col_hex << endl;
                            row_col_hex += row_hex;
                            row_col_hex += col_hex;
                            STATE_ARRAY_INV_SUB_BYTES[i][j] = row_col_hex;
                            break;
                        }
                    }
                }
                cout << " | " << STATE_ARRAY_INV_SUB_BYTES[i][j];
            }
            cout << " |" << endl;
        }
    }

    void invAddRoundKey(int round)
    {

        array<array<string, 4>, 4> ROUND_KEY_MATRIX = KEY_POOL[(KEY_POOL.size()-round) - 2]; // - 2 since last index is initial round key and size starts from 1 | going in reverse order
        
        cout << "--------- ROUND KEY ---------" << endl;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                cout << " | " << hex << stoi(ROUND_KEY_MATRIX[i][j], nullptr, 2) << dec;
            }
            cout << " |" << endl;
        }

        cout << "--------- INVERSE ROUND KEY ---------" << endl;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                int sub_bytes_decimal = stoi(STATE_ARRAY_INV_SUB_BYTES[i][j], nullptr, 16);
                string sub_bytes_binary = bitset<8>(sub_bytes_decimal).to_string();

                string xor_result = binaryXOR(sub_bytes_binary, ROUND_KEY_MATRIX[i][j]);
                // unsigned long long int xor_decimal = stoull(xor_result, nullptr, 2);
                int xor_decimal = stoi(xor_result, nullptr, 2);

                stringstream ss;
                ss << hex << xor_decimal;
                string hex_string = ss.str();

                STATE_ARRAY_INV_ROUND_KEY[i][j] = hex_string;
                
                if (STATE_ARRAY_INV_ROUND_KEY[i][j].length() > 1)
                {
                    cout << " | " << STATE_ARRAY_INV_ROUND_KEY[i][j];
                }
                else
                {
                    cout << " |  " << STATE_ARRAY_INV_ROUND_KEY[i][j];
                }
            }
            cout << " |" << endl;
        }
    }

    void invMixColumn()
    {
        cout << "--------- INVERSE MIX COLUMN ---------" << endl;

        array<array<string, 4>, 4> OUTPUT;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                int gf_xor = 0;

                for (int k = 0; k < 4; k++)
                {
                    int pre_defined_matrix_int = INV_PRE_DEFINED_MATRIX[i][k];
                    int state_array_int = stoi(STATE_ARRAY_INV_ROUND_KEY[k][j], nullptr, 16); // hex to decimal

                    int gf_mul = galois_multiply(pre_defined_matrix_int, state_array_int);

                    if (k != 0)
                    {
                        int temp = gf_xor ^ gf_mul; // xor second value with first stored in variable
                        gf_xor = temp; // replace with newer value
                    }
                    else 
                    {
                        gf_xor = gf_mul; // first only store value
                    }
                }            
                stringstream ss;
                ss << hex << gf_xor;
                string hex_xor = ss.str();

                OUTPUT[i][j] = hex_xor;
                
                // print formatting
                if (OUTPUT[i][j].length() > 1)
                {
                    cout << " | " << OUTPUT[i][j];
                }
                else
                {
                    cout << " |  " << OUTPUT[i][j];
                }
            }
            cout << " |" << endl;
        }
        INV_MIX_COLUMN_OUTPUT.push_back(OUTPUT);
    }

};

class AES
{
    public: 

    string key;
    string input_text;
    vector <string> INPUT_16_BYTES_ARRAY;

    AES() {}

    AES(string input, string k)
    {
        this->input_text = input;
        this->key = k;

        makeInputPackets();

        KeyExpansion Key(key);
        Key.InitialRoundKey();
        Key.expandKey();
    }

    void makeInputPackets()
    {
        // padding bits added in createPlainMatrix()

        int index = 0;

        while (index < input_text.length())
        {
            string temp_string;

            for (int i = 0; i < 16; i++)
            {
                if (index < input_text.length())
                {
                    temp_string += input_text[index++];
                }
            }
            INPUT_16_BYTES_ARRAY.push_back(temp_string);
        }
    }

    void Encryption_128_bit()
    {
        for (int i = 0; i < INPUT_16_BYTES_ARRAY.size(); i++)
        {
            cout << "\n16 byte input: " << INPUT_16_BYTES_ARRAY[i] << "\n" << endl;
            
            Encryption Encrypt(INPUT_16_BYTES_ARRAY[i]);
            Encrypt.createPlainMatrix();

            for (int round = 0; round < 10; round++)
            {
                cout << "\n******** ENCRYPTION ROUND # " << round+1 << " ********\n" << endl;

                Encrypt.subBytes(round);
                Encrypt.shiftRows();

                if (round < 9)
                {
                    Encrypt.mixColumn();
                    Encrypt.addRoundKey(round);
                }
                else
                {
                    Encrypt.addLastRoundKey(round);
                }
            }
        }
    }

    void Decryption_128_bit()
    {
        for (int i = 0; i < INPUT_16_BYTES_ARRAY.size(); i++)
        {
            Decryption Decrypt(i);
            
            for (int round = 0; round < 10; round++)
            {
                if (round == 0)
                {
                    Decrypt.createCipherMatrix();
                }
                cout << "\n******** DECRYPTION ROUND # " << round+1 << " ********\n" << endl;

                Decrypt.invShiftRows(round);
                Decrypt.invSubBytes(round);
                Decrypt.invAddRoundKey(round);

                if (round < 9)
                {
                    Decrypt.invMixColumn();
                }
            }
            // extract plaintext

            string pt;

            for (int k = 0; k < 4; k++)
            {
                for (int j = 0; j < 4; j++)
                {
                    int pt_ascii = stoul(Decrypt.STATE_ARRAY_INV_ROUND_KEY[j][k], nullptr, 16);
                    pt += static_cast<char>(pt_ascii);
                }
            }
            cout << "\nDecrypted Text: " << pt << endl;
        } 
    }

};

int main()
{
    AES Cipher("muhammadhabibkhamuhammadhabib", "qwertyuiopasdfgh");
    Cipher.Encryption_128_bit();
    Cipher.Decryption_128_bit();

    return 0;
}
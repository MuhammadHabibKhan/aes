#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>
#include <array>

#include "Galois/galois.h"

using namespace std;

vector <array<array<string, 4>, 4>> KEY_POOL; // in binary

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

    for (int i = 0; i < 4; i++)
    {
        array<array<string, 4>, 4> KEY_MATRIX;

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
        KEY_POOL.push_back(KEY_MATRIX);
    }
}

void expandKey()
{
    // create word array and w'

    for (int k = 1; k < 11; k++) // 11 round keys for 128-bit
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
        string w_dash = gFunction(WORD_ARRAY[4], k); // w' calculated using the last word

        // generate new word array

        for (int i = 0; i < 4; i++)
        {
            if (i = 0)
            {
                int first_value = stoi(WORD_ARRAY[i], nullptr, 2);
                int w_dash_int =  stoi(w_dash, nullptr, 2); // for first word, use w' as second value

                int xor_result = first_value ^ w_dash_int;
                WORD_ARRAY_OUTPUT[i] = bitset<8>(xor_result).to_string();

            }
            else
            {
                int first_value = stoi(WORD_ARRAY[i], nullptr, 2);
                int second_value =  stoi(WORD_ARRAY_OUTPUT[i-1], nullptr, 2); // use previous new word as second value

                int xor_result = first_value ^ second_value;
                WORD_ARRAY_OUTPUT[i] = bitset<8>(xor_result).to_string();
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
        BYTE_ARRAY_SHIFT[i] = BYTE_ARRAY[(i - 1) % 4];
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

    for (int i = 0; i < 4; i++)
    {
        int sub_word = stoi(BYTE_ARRAY_SUB[i], nullptr, 2);
        int round_const = stoi(ROUND_CONSTANT[round], nullptr, 16); // round constant originally in hex base 16

        int xor_result = sub_word ^ round_const; // xor 8-bit sections of the word with round constant
        w_dash += bitset<32>(xor_result).to_string(); // append the xored bits in binary to w_dash
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

Encryption() {}

Encryption(string ip_str)
{
    this->input_string = ip_str;
}

void createPlainMatrix()
{
    cout << "--------- PLAIN TEXT ---------" << endl;
    int index = 0;

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
                PT_MATRIX_INT[j][i] = int_value; // input stored column-wise in AES / Block Ciphers

                // Convert integer to hexadecimal string
                stringstream ss;
                ss << hex << int_value; // set hex as format for string stream before inserting
                string hex_string = ss.str(); //extract contents from stream as string
                
                PT_MATRIX_HEX[j][i] = hex_string;
            }
            else
            {
                cout << "Error: Input String out of bounds" << endl;
                return;
            }
        }
    }
    // print column-wise
    
    for (int i=0; i<4; i++)
    {
        for (int j=0l; j<4; j++)
        {
            cout << " | " << PT_MATRIX_HEX[i][j];
        }
        cout << " |" << endl;
    }
}

void subBytes()
{
    cout << "--------- SUB BYTES ---------" << endl;

    for (int i=0; i < 4; i++)
    {
        for (int j=0; j < 4; j++)
        {
            string plain_char_hex = PT_MATRIX_HEX[i][j];
            string first_char(1, plain_char_hex[0]);
            string second_char(1, plain_char_hex[1]);

            int row_index = stoi(first_char, nullptr, 16);
            int col_index = stoi(second_char, nullptr, 16);

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

                int gf_mul = galois_single_multiply(pre_defined_matrix_int, state_array_int, 8);

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
            
            cout << " | " << STATE_ARRAY_MIX_COLUMN[i][j];
        }
        cout << " |" << endl;
    }
}

};

int main()
{
    Encryption AES("muhammadhabibkha");
    AES.createPlainMatrix();
    AES.subBytes();
    AES.shiftRows();
    AES.mixColumn();

    return 0;
}

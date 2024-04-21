#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>
#include <array>

#include "Galois/galois.h"

using namespace std;

const std::array<std::array<std::string, 16>, 16> S_BOX = {
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

const std::array<std::array<int, 4>, 4> PRE_DEFINED_MATRIX = {
        {
            {2, 3, 1, 1},
            {1, 2, 3, 1},
            {1, 1, 2, 3},
            {3, 1, 1, 2}
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
                int int_value = std::stoi(binary_string_8bit, nullptr, 2);
                PT_MATRIX_INT[i][j] = int_value;

                // Convert integer to hexadecimal string
                stringstream ss;
                ss << hex << int_value; // set hex as format for string stream before inserting
                string hex_string = ss.str(); //extract contents from stream as string
                
                PT_MATRIX_HEX[i][j] = hex_string;
                cout << " | " << hex_string;
            }
            else
            {
                cout << "Error: Input String out of bounds" << endl;
                return;
            }
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




};


int main()
{
    // unsigned int x, y;
    // x = 150;
    // y = 158;

    // // unsigned int z = galois_single_multiply(x, y, 8);
    // unsigned int z = galois_single_divide(45, 34, 8);

    // cout << z << endl;

    Encryption AES("muhammadhabibkha");
    AES.createPlainMatrix();
    AES.subBytes();
    AES.shiftRows();

    return 0;
}

#include <iostream>
#include <array>
#include <bitset>

#include "des.hpp"

//! Test with these online tools:
//! https://www.javacardos.com/tools/des-encrypt-decrypt
//! https://emvlab.org/descalc/


int main(int argc, char** arv)
{

    std::bitset<64> plaintext{ 0x123456ABCD132536 }, key{ 0xAABB09182736CCDD };

    auto des{ DES(key) }; 
    auto ciphertext{ des.Encrypt(plaintext) };
    
    /* Print Results */
    std::cout << std::string(80, '-') << "\nplaintext:\n"  << "bit:\t" << plaintext  << "\nhex:\t" << std::hex <<  plaintext.to_ullong() << std::endl;
    std::cout << "ciphertext:\n" << "bit:\t" << ciphertext << "\nhex:\t" << std::hex << ciphertext.to_ullong() << std::endl;

    auto recoveredPlaintext{ des.Decrypt(ciphertext) };
    
    /* Print Results */
    std::cout << std::string(80, '-') << "\nciphertext:\n"  << "bit:\t" << ciphertext  << "\nhex:\t" << std::hex <<  ciphertext.to_ullong() << std::endl;
    std::cout << "recovered plaintext:\n" << "bit:\t" << recoveredPlaintext << "\nhex:\t" << std::hex << recoveredPlaintext.to_ullong() << std::endl;

    return EXIT_SUCCESS;
}

/* OUTPUT
--------------------------------------------------------------------------------
plaintext:
bit:    0001001000110100010101101010101111001101000100110010010100110110
hex:    123456abcd132536
ciphertext:
bit:    1100000010110111101010001101000001011111001110101000001010011100
hex:    c0b7a8d05f3a829c
--------------------------------------------------------------------------------
ciphertext:
bit:    1100000010110111101010001101000001011111001110101000001010011100
hex:    c0b7a8d05f3a829c
recovered plaintext:
bit:    0001001000110100010101101010101111001101000100110010010100110110
hex:    123456abcd132536
*/

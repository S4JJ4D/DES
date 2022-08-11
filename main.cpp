#include <iostream>
#include <array>
#include <bitset>

#include "constants.hpp"
#include "des.hpp"

//! Test with these online tools:
//! https://www.javacardos.com/tools/des-encrypt-decrypt
//! https://emvlab.org/descalc/


int main(int argc, char** arv)
{

    /* DES Encryption */

    std::bitset<64> plaintext{ 0x123456ABCD132536 }, key{ 0xAABB09182736CCDD };

    auto des{ DES(key) }; 
    auto ciphertext{ des.Encrypt(plaintext) };
    std::cout << std::string(80, '-') << "\nplaintext:\n"  << "bit:\t" << plaintext  << "\nhex:\t" << std::hex <<  plaintext.to_ullong() << std::endl;
    std::cout << "ciphertext:\n" << "bit:\t" << ciphertext << "\nhex:\t" << std::hex << ciphertext.to_ullong() << std::endl;

    auto recoveredPlaintext{ des.Decrypt(ciphertext) };
    std::cout << std::string(80, '-') << "\nciphertext:\n"  << "bit:\t" << ciphertext  << "\nhex:\t" << std::hex <<  ciphertext.to_ullong() << std::endl;
    std::cout << "recovered plaintext:\n" << "bit:\t" << recoveredPlaintext << "\nhex:\t" << std::hex << recoveredPlaintext.to_ullong() << std::endl;

    return EXIT_SUCCESS;
}
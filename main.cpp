#include <iostream>
#include <array>
#include <bitset>

#include "des.hpp"

//! You can verify this code with these online tools: (In both cases, remove `0x` prefix when entering the hex values)
//! https://www.javacardos.com/tools/des-encrypt-decrypt
//! https://emvlab.org/descalc/ 


int main(int argc, char** argv)
{

    /* DES */

    std::bitset<64> plaintext{ 0x02468aceeca86420 }, key{ 0x0f1571c947d9e859 };

    Crypto::DES des{ key };
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
bit:    0000001001000110100010101100111011101100101010000110010000100000
hex:    2468aceeca86420
ciphertext:
bit:    1101101000000010110011100011101010001001111011001010110000111011
hex:    da02ce3a89ecac3b
--------------------------------------------------------------------------------
ciphertext:
bit:    1101101000000010110011100011101010001001111011001010110000111011
hex:    da02ce3a89ecac3b
recovered plaintext:
bit:    0000001001000110100010101100111011101100101010000110010000100000
hex:    2468aceeca86420

*/

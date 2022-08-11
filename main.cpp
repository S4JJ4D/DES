#include <iostream>
#include <array>
#include <bitset>

#include "constants.hpp"
#include "des.hpp"

// https://www.javacardos.com/tools/des-encrypt-decrypt
// https://emvlab.org/descalc/


void PrintContainer(auto& container)
{
    for (const auto& element : container)
    {
        std::cout << element << std::endl;
    }
}

int main(int argc, char** arv)
{

    /* DES Encryption */
    std::cout << std::string(80, '-') << std::endl;
    std::bitset<64> plaintext{ 0x123456ABCD132536 };
    std::bitset<64> key      { 0xAABB09182736CCDD };
 
    auto ciphertext{ EncryptDES(plaintext, key) };

    std::cout << std::dec << ciphertext << std::endl;
    std::cout << std::hex << ciphertext.to_ullong() << std::endl;
    std::cout << "C0B7A8D05F3A829C" << std::endl;


    auto ptext{ DecryptDES(ciphertext, key) };

    std::cout << std::dec << ptext << std::endl;
    std::cout << std::hex << ptext.to_ullong() << std::endl;
    std::cout << "123456ABCD132536" << std::endl;

    return 0;
}
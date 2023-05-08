#ifndef DES_HPP_
#define DES_HPP_

#include <bitset>
#include <tuple>
#include <vector>
#include <array>
#include <optional>
#include <algorithm>

#include "constants.hpp"

/**
 * @brief In-Place Left Circular Shift
 * @param inBitset input std::bitset<> object
 * @param nPos number of positions to shift
 * @return *this
 */
template<size_t nBit>
std::bitset<nBit>& LeftRotate(std::bitset<nBit> &inBitset, uint32_t nPos)
{
    return inBitset = ( (inBitset << nPos) | (inBitset >> (nBit - nPos)) );
}


/**
 * @brief Grouping Cryptosystems
*/
namespace Crypto
{

class DES
{
public:

    using Key =          std::bitset<DESC::SIZE::KEY>;
    using SubKey =       std::bitset<DESC::SIZE::SUBKEY>;
    using PureKey =      std::bitset<DESC::SIZE::PUREKEY>;
    using HalfPureKey =  std::bitset<DESC::SIZE::PUREKEY_HALF>;
    using Block =        std::bitset<DESC::SIZE::DES_BLOCKSIZE>;
    using HalfBlock =    std::bitset<DESC::SIZE::DES_HALF_BLOCKSIZE>;
    using SubKeyArray =  std::array<DES::SubKey, DESC::SIZE::ROUND_COUNT>;

    DES(const std::bitset<DESC::SIZE::KEY>& inKey)
        : mKey{ inKey } {}

    /** DES Encryption **/
    Block Encrypt(const Block& plaintext, std::optional<Key> key = std::nullopt, bool printSteps = false);
    /** DES Decryption **/
    Block Decrypt(const Block& ciphertext, std::optional<Key> key = std::nullopt, bool printSteps = false);

    void setKey(const Key& inKey) { mKey = inKey; }
    [[nodiscard]] Key getKey() { return mKey; }

private:
    Key mKey;

    /** SBox Map **/
    std::bitset<DESC::SIZE::SBOX_OUTPUT> SBoxMap(DESC::SBoxType SBoxType, const std::bitset<DESC::SIZE::SBOX_INPUT>& inBitset);

    /** Initial Permutation **/
    Block IPMap(const Block& inBitset);

    /** Initial Permutation - splits the output into two segments **/
    std::tuple<HalfBlock, HalfBlock> IPMapSplitOut(const Block& inBitset);

    /** Final Permutation - Overload (1) **/
    Block FPMap(const Block& inBitset);

    /** Final Permutation - Overload (2) **/
    Block FPMap(const HalfBlock& inL, const HalfBlock& inR);

    /** Permuted Choice 1 (PC-1) **/
    PureKey PC1Map(const Block& inBitset);

    /** Permuted Choice 1 (PC-1) - Splits output to left and right segments **/
    std::tuple<HalfPureKey, HalfPureKey> PC1MapSplitOut(const Block& inBitset);

    /** Permuted Choice 2 (PC-2) **/
    SubKey PC2Map(const HalfPureKey& inL, const HalfPureKey& inR);

    /** Expansion Box **/
    std::bitset<DESC::SIZE::EBOX_OUTPUT> ExpansionFunction(const HalfBlock& inBitset);

    /** 32-bit permutation performed at the final stage of F-function **/
    HalfBlock PMap(const HalfBlock& inBitset);

    /** The Feistel (F) function **/
    HalfBlock FeistelFunction(const HalfBlock& inBitset, const SubKey& subKey);

    /** Feistel Round Overload (1) **/
    Block FeistelRound(const Block& inBitset, const SubKey& subKey);

    /** Feistel Round Overload (2): Operating on Segments **/
    std::tuple<HalfBlock, HalfBlock> FeistelRound(const HalfBlock& inL, const HalfBlock& inR, const SubKey& subKey);

    /** Key Schedule **/
    SubKeyArray KeySchedule(const Key& key);

};

} // ! namespace Crypto

#endif // ! DES_HPP_


/**
 * @class Crypto::DES
 * @brief Date Encryption System Class
 *
 * @details An object of this class represents an instance of a DES crypto-system allowing the user to encrypt and decrypt
 * plaintext and cyphertext data using DES ECB mode. DES processes blocks of data of length 64 bit one at a time.
 * An example of using this class to encrypt/decrypt data is as follows:
 * @code
 * // Defining arbitrary data
 * std::bitset<64> plaintext{ 0x02468aceeca86420 }, key{ 0x0f1571c947d9e859 };
 * // Instantiating the class
 * Crypto::DES des{ key };
 * // Encrypt the plaintext
 * auto ciphertext{ des.Encrypt(plaintext) };
 * // Print Results
 * std::cout << std::string(80, '-') << "\nplaintext:\n" << "bit:\t" << plaintext << "\nhex:\t" << std::hex << plaintext.to_ullong() << std::endl;
 * std::cout << "ciphertext:\n" << "bit:\t" << ciphertext << "\nhex:\t" << std::hex << ciphertext.to_ullong() << std::endl;
 * auto recoveredPlaintext{ des.Decrypt(ciphertext) };
 * // Print Results
 * std::cout << std::string(80, '-') << "\nciphertext:\n"  << "bit:\t" << ciphertext  << "\nhex:\t" << std::hex <<  ciphertext.to_ullong() << std::endl;
 * std::cout << "recovered plaintext:\n" << "bit:\t" << recoveredPlaintext << "\nhex:\t" << std::hex << recoveredPlaintext.to_ullong() << std::endl;
 * @endcode
*/

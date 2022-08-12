#ifndef DES_HPP
#define DES_HPP

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
std::bitset<nBit>& LeftRotate(std::bitset<nBit>& inBitset, uint32_t nPos)
{
    return inBitset = ( (inBitset << nPos) | (inBitset >> (nBit - nPos)) );
}

/**
 * @brief Data Encryption Standard class
 */
class DES
{
public:

    DES(const std::bitset<DESC::SIZE::KEY>& inKey)
    :   mKey{inKey} {}
    
    /** DES Encryption **/
    std::bitset<DESC::SIZE::DES_BLOCKSIZE> Encrypt(const std::bitset<DESC::SIZE::DES_BLOCKSIZE>& plaintext,  std::optional<std::bitset<DESC::SIZE::KEY>> key = std::nullopt, bool printSteps = false);
    /** DES Decryption **/
    std::bitset<DESC::SIZE::DES_BLOCKSIZE> Decrypt(const std::bitset<DESC::SIZE::DES_BLOCKSIZE>& ciphertext, std::optional<std::bitset<DESC::SIZE::KEY>> key = std::nullopt, bool printSteps = false);


    void setKey(const std::bitset<DESC::SIZE::KEY>& inKey) {mKey = inKey;}
    std::bitset<DESC::SIZE::KEY> getKey() {return mKey;}

private:
    
    std::bitset<DESC::SIZE::KEY> mKey;

    /** SBox Map - Overload(1) **/
    uint32_t SBoxMap(DESC::SBoxType SBoxType, uint32_t inputVal);
    /** SBox Map - Overload (2) **/
    std::bitset<DESC::SIZE::SBOX_OUTPUT> SBoxMap(DESC::SBoxType SBoxType, const std::bitset<DESC::SIZE::SBOX_INPUT>& inBitset);

    /** Initial Permutation **/
    std::bitset<DESC::SIZE::IP_OUTPUT> IPMap(const std::bitset<DESC::SIZE::IP_INPUT>& inBitset);
    /** Initial Permutation - splits the output into two segments **/
    std::tuple<std::bitset<DESC::SIZE::IP_SOUTPUT>, std::bitset<DESC::SIZE::IP_SOUTPUT>> IPMapSplitOut(const std::bitset<DESC::SIZE::IP_INPUT>& inBitset);

    /** Final Permutation - Overload (1) **/
    std::bitset<DESC::SIZE::FP_OUTPUT> FPMap(const std::bitset<DESC::SIZE::FP_INPUT>& inBitset);
    /** Final Permutation - Overload (2) **/
    std::bitset<DESC::SIZE::FP_OUTPUT> FPMap(const std::bitset<DESC::SIZE::FP_SINPUT>& inL, const std::bitset<DESC::SIZE::FP_SINPUT>& inR);

    /** Permuted Choice 1 (PC-1) **/
    std::bitset<DESC::SIZE::PC1_OUTPUT> PC1Map(const std::bitset<DESC::SIZE::PC1_INPUT>& inBitset);
    /** Permuted Choice 1 (PC-1) - Splits output to left and right segments **/
    std::tuple<std::bitset<DESC::SIZE::PC1_SOUTPUT>, std::bitset<DESC::SIZE::PC1_SOUTPUT>> PC1MapSplitOut(const std::bitset<DESC::SIZE::PC1_INPUT>& inBitset);

    /** Permuted Choice 2 (PC-2) **/
    std::bitset<DESC::SIZE::PC2_OUTPUT> PC2Map(const std::bitset<DESC::SIZE::PC2_SINPUT>& inL, const std::bitset<DESC::SIZE::PC2_SINPUT>& inR);

    /** Expansion Box **/
    std::bitset<DESC::SIZE::EBOX_OUTPUT> ExpansionFunction(const std::bitset<DESC::SIZE::EBOX_INPUT>& inBitset);

    /** 32-bit permutation performed at the final stage of F-function **/
    std::bitset<DESC::SIZE::P_OUTPUT> PMap(const std::bitset<DESC::SIZE::P_INPUT>& inBitset);

    /** The Feistel (F) function **/
    std::bitset<DESC::SIZE::FFUNC_OUTPUT> FeistelFunction(const std::bitset<DESC::SIZE::FFUNC_INPUT>& inBitset, const std::bitset<DESC::SIZE::SUBKEY>& key);

    /** Feistel Round Overload (1) **/
    std::bitset<DESC::SIZE::FEISTEL_ROUND_OUTPUT> FeistelRound(const std::bitset<DESC::SIZE::FEISTEL_ROUND_INPUT>& inBitset, const std::bitset<DESC::SIZE::SUBKEY>& key);

    /** Feistel Round Overload (2): Operating on Segments **/
    std::tuple<std::bitset<DESC::SIZE::FEISTEL_ROUND_SOUTPUT>, std::bitset<DESC::SIZE::FEISTEL_ROUND_SOUTPUT>>
    FeistelRound(const std::bitset<DESC::SIZE::FEISTEL_ROUND_SINPUT>& inL, const std::bitset<DESC::SIZE::FEISTEL_ROUND_SINPUT>& inR, const std::bitset<DESC::SIZE::SUBKEY>& key);

    /** Key Schedule **/
    std::array<std::bitset<DESC::SIZE::SUBKEY>, DESC::SIZE::ROUND_COUNT> KeySchedule(const std::bitset<DESC::SIZE::KEY>& key);

}; // ! class DES


#endif // ! DES_HPP

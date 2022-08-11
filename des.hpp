#ifndef DES_FCN_HPP
#define DES_FCN_HPP

#include <bitset>
#include <tuple>
#include <vector>
#include <array>
#include <optional>

#include "constants.hpp"

/**
 * @brief In-Place Left Circular Shift
 */
template<size_t nBit>
std::bitset<nBit>& LeftRotate(std::bitset<nBit> &inBitset, unsigned int r)
{
    return inBitset = ( (inBitset << r) | (inBitset >> (nBit - r)) );
}

unsigned int SBoxMap(DES::SBoxType SBoxType, unsigned int inputVal);
std::bitset<DES::SIZE::SBOX_OUTPUT> SBoxMap(DES::SBoxType SBoxType, const std::bitset<DES::SIZE::SBOX_INPUT>& inBitset);


/* Initial Permutation */
std::bitset<DES::SIZE::IP_OUTPUT> IPMap(const std::bitset<DES::SIZE::IP_INPUT>& inBitset);
/* Initial Permutation - split the output into two segments */
std::tuple<std::bitset<DES::SIZE::IP_SOUTPUT>, std::bitset<DES::SIZE::IP_SOUTPUT>> IPMapSplitOut(const std::bitset<DES::SIZE::IP_INPUT>& inBitset);

/* Final Permutation - Overload (1) */
std::bitset<DES::SIZE::FP_OUTPUT> FPMap(const std::bitset<DES::SIZE::FP_INPUT>& inBitset);
/* Final Permutation - Overload (2) */
std::bitset<DES::SIZE::FP_OUTPUT> FPMap(const std::bitset<DES::SIZE::FP_SINPUT>& inL, const std::bitset<DES::SIZE::FP_SINPUT>& inR);

/* Permuted Choice 1 (PC-1) */
std::bitset<DES::SIZE::PC1_OUTPUT> PC1Map(const std::bitset<DES::SIZE::PC1_INPUT>& inBitset);
/* Permuted Choice 1 (PC-1) - Splits output to left and right segments */
std::tuple<std::bitset<DES::SIZE::PC1_SOUTPUT>, std::bitset<DES::SIZE::PC1_SOUTPUT>> PC1MapSplitOut(const std::bitset<DES::SIZE::PC1_INPUT>& inBitset);

/* Permuted Choice 2 (PC-2) */
std::bitset<DES::SIZE::PC2_OUTPUT> PC2Map(const std::bitset<DES::SIZE::PC2_SINPUT>& inL, const std::bitset<DES::SIZE::PC2_SINPUT>& inR);

/* Expansion Box */
std::bitset<DES::SIZE::EBOX_OUTPUT> ExpansionFunction(const std::bitset<DES::SIZE::EBOX_INPUT>& inBitset);

/* 32-bit permutation performed at the final stage of f-function */
std::bitset<DES::SIZE::P_OUTPUT> PMap(const std::bitset<DES::SIZE::P_INPUT>& inBitset);

/* The Feistel (F) function */
std::bitset<DES::SIZE::FFUNC_OUTPUT> FeistelFunction(const std::bitset<DES::SIZE::FFUNC_INPUT>& inBitset, const std::bitset<DES::SIZE::SUBKEY>& key);

/* Feistel Round Overload (1) */
std::bitset<DES::SIZE::FEISTEL_ROUND_OUTPUT> FeistelRound(const std::bitset<DES::SIZE::FEISTEL_ROUND_INPUT>& inBitset, const std::bitset<DES::SIZE::SUBKEY>& key);

/* Feistel Round Overload (2): Operating on Segments*/
std::tuple<std::bitset<DES::SIZE::FEISTEL_ROUND_SOUTPUT>, std::bitset<DES::SIZE::FEISTEL_ROUND_SOUTPUT>>
FeistelRound(const std::bitset<DES::SIZE::FEISTEL_ROUND_SINPUT>& inL, const std::bitset<DES::SIZE::FEISTEL_ROUND_SINPUT>& inR, const std::bitset<DES::SIZE::SUBKEY>& key);

/* Key Schedule */
std::array<std::bitset<DES::SIZE::SUBKEY>, DES::SIZE::ROUND_COUNT> KeySchedule(const std::bitset<DES::SIZE::KEY>& key);

/* DES Encryption */
std::bitset<DES::SIZE::DES_BLOCKSIZE> EncryptDES(const std::bitset<DES::SIZE::DES_BLOCKSIZE>& plaintext, const std::bitset<DES::SIZE::KEY>& key, bool printSteps = false);

/* DES Decryption */
std::bitset<DES::SIZE::DES_BLOCKSIZE> DecryptDES(const std::bitset<DES::SIZE::DES_BLOCKSIZE>& ciphertext, const std::bitset<DES::SIZE::KEY>& key, bool printSteps = false);

#endif

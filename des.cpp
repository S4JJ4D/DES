/**
 * @file des.cc
 * @brief Implementing DES
 * @author S4JJ4D
 */

#include <iostream>

#include "des.hpp"

using namespace Crypto;

// -------------------------------------------------------------------------------------------------------------------
/* S-Box */
std::bitset<DESC::SIZE::SBOX_OUTPUT> DES::SBoxMap(DESC::SBoxType SBoxType, const std::bitset<DESC::SIZE::SBOX_INPUT>& inBitset)
{
    auto inputVal{ inBitset.to_ulong() };

    /* Extract Column-Select */
    auto colSelect{ (inputVal&0b011110) >> 1 };
    /* Extract Row-Select */
    auto tempVal{ (inputVal&100001) };
    auto rowSelect{ ( (tempVal >> 5) & 1 )*2 + (tempVal & 1) };
    /* Due to zero-indexing of arrays, a subtraction of 1 is necessary */
    return DESC::SBoxArray[static_cast<uint32_t>(SBoxType)][rowSelect][colSelect];

}


// -------------------------------------------------------------------------------------------------------------------
/* Initial Permutation */
DES::Block DES::IPMap(const Block& inBitset)
{
    Block outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::IP_INPUT; ++i)
    {
        k = DESC::SIZE::IP_OUTPUT - 1 - i;
        idx = DESC::SIZE::IP_INPUT - DESC::IPArray[i/DESC::IPnRow][i%DESC::IPnRow];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}


// -------------------------------------------------------------------------------------------------------------------
/* Initial Permutation - split the output into two segments */
std::tuple<DES::HalfBlock, DES::HalfBlock> DES::IPMapSplitOut(const Block& inBitset)
{
    HalfBlock leftBitset{}, rightBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::IP_INPUT; ++i)
    {
        k = DESC::SIZE::IP_INPUT - 1 - i;
        idx = DESC::SIZE::IP_INPUT - DESC::IPArray[i/DESC::IPnRow][i%DESC::IPnRow];
        if (i < DESC::SIZE::IP_SOUTPUT)
            leftBitset[k - DESC::SIZE::IP_SOUTPUT] = inBitset[idx];
        else
            rightBitset[k]                         = inBitset[idx];
    }

    return {leftBitset, rightBitset};
}


// -------------------------------------------------------------------------------------------------------------------
/* Final Permutation - Overload (1) */
DES::Block DES::FPMap(const Block& inBitset)
{
    Block outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::FP_OUTPUT; ++i)
    {
        k = DESC::SIZE::FP_OUTPUT - 1 - i;
        idx = DESC::SIZE::FP_INPUT - DESC::FPArray[i/DESC::IPnCol][i%DESC::IPnCol];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}


// -------------------------------------------------------------------------------------------------------------------
/* Final Permutation - Overload (2) */
DES::Block DES::FPMap(const HalfBlock& inL, const HalfBlock& inR)
{
    Block outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::FP_OUTPUT; ++i)
    {
        k = DESC::SIZE::FP_OUTPUT - 1 - i;
        idx = DESC::FPArray[i/DESC::IPnCol][i%DESC::IPnCol];
        outBitset[k] = (idx <= DESC::SIZE::FP_SINPUT) ? inL[DESC::SIZE::FP_SINPUT - idx] : inR[DESC::SIZE::FP_SINPUT - (idx - DESC::SIZE::FP_SINPUT)];
    }

    return outBitset;
}


// -------------------------------------------------------------------------------------------------------------------
/* Permuted choice 1 */
DES::PureKey DES::PC1Map(const Block& inBitset)
{
    PureKey outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::PC1_OUTPUT; ++i)
    {
        k = DESC::SIZE::PC1_OUTPUT - 1 - i;
        idx = DESC::SIZE::PC1_INPUT - DESC::PC1Array[i/DESC::PC1nCol][i%DESC::PC1nCol];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}


// -------------------------------------------------------------------------------------------------------------------
/* Permuted choice 1 - Splits output to left and right segments */
std::tuple<DES::HalfPureKey, DES::HalfPureKey> DES::PC1MapSplitOut(const Block& inBitset)
{
    HalfPureKey leftBitset{}, rightBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::PC1_OUTPUT; ++i)
    {
        k = DESC::SIZE::PC1_OUTPUT - 1 - i;
        idx = DESC::SIZE::PC1_INPUT - DESC::PC1Array[i/DESC::PC1nCol][i%DESC::PC1nCol];
        if (i < DESC::SIZE::PC1_SOUTPUT)
            leftBitset[k - DESC::SIZE::PC1_SOUTPUT] = inBitset[idx];
        else
            rightBitset[k]                          = inBitset[idx];
    }

    return {leftBitset, rightBitset};
}


// -------------------------------------------------------------------------------------------------------------------
/* Permuted Choice 2 (PC-2) */
DES::SubKey DES::PC2Map(const HalfPureKey& inL, const HalfPureKey& inR)
{
    SubKey outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::PC2_OUTPUT; ++i)
    {
        k = DESC::SIZE::PC2_OUTPUT - 1 - i;
        idx = DESC::PC2Array[i/DESC::PC2nCol][i%DESC::PC2nCol];
        outBitset[k] = (idx <= DESC::SIZE::PC2_SINPUT) ? inL[DESC::SIZE::PC2_SINPUT - idx] : inR[DESC::SIZE::PC2_SINPUT - (idx - DESC::SIZE::PC2_SINPUT)];
    }

    return outBitset;
}


// -------------------------------------------------------------------------------------------------------------------
/* E-Box */
std::bitset<DESC::SIZE::EBOX_OUTPUT> DES::ExpansionFunction(const HalfBlock& inBitset)
{
    std::bitset<DESC::SIZE::EBOX_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::EBOX_OUTPUT; ++i)
    {
        k = DESC::SIZE::EBOX_OUTPUT - 1 - i;
        idx = DESC::SIZE::EBOX_INPUT - DESC::ExpansionArray[i/DESC::EXnCol][i%DESC::EXnCol];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}


// -------------------------------------------------------------------------------------------------------------------
/* 32-bit permutation performed at the final stage of f-function */
DES::HalfBlock DES::PMap(const HalfBlock& inBitset)
{
    HalfBlock outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::P_OUTPUT; ++i)
    {
        k = DESC::SIZE::P_OUTPUT - 1 - i;
        idx = DESC::SIZE::P_INPUT - DESC::PArray[i/DESC::PnCol][i%DESC::PnCol];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}


// -------------------------------------------------------------------------------------------------------------------
/* The Feistel (F) function */
DES::HalfBlock DES::FeistelFunction(const HalfBlock& inBitset, const SubKey& subKey)
{
    HalfBlock outSBoxOperation{};
    /** 1. Expansion
     *  2. Key-Mixing
    */
    auto preSBoxData{ ExpansionFunction(inBitset) ^ subKey };

    std::bitset<DESC::SIZE::SBOX_INPUT> inSBox{};

    /* There are 8 SBoxes to Operate */
    for(size_t SBoxStepCount{}; SBoxStepCount < DESC::SIZE::SBOX_COUNT; ++SBoxStepCount)
    {
        inSBox = 0;
        for (size_t i{}, j{}; i < DESC::SIZE::SBOX_INPUT; ++i)
        {
            j = DESC::SIZE::SUBKEY - 1 - (DESC::SIZE::SBOX_INPUT * SBoxStepCount + i);
            inSBox[DESC::SIZE::SBOX_INPUT - 1 - i] = preSBoxData[j];
        }

        std::bitset<DESC::SIZE::SBOX_OUTPUT> outSBox{ SBoxMap(static_cast<DESC::SBoxType>(SBoxStepCount), inSBox) };

        for (size_t i{}, j{}; i < DESC::SIZE::SBOX_OUTPUT; ++i)
        {
            j = DESC::SIZE::FFUNC_OUTPUT - 1 - (DESC::SIZE::SBOX_OUTPUT * SBoxStepCount + i);
            outSBoxOperation[j] = outSBox[DESC::SIZE::SBOX_OUTPUT - 1 - i];
        }

    }

    return PMap(outSBoxOperation);
}


// -------------------------------------------------------------------------------------------------------------------
/* Feistel Round Overload (1) */
DES::Block DES::FeistelRound(const Block& inBitset, const SubKey& subKey)
{
    /* R: Right hand side bitset */
    /* L: Left  hand side bitset */
    HalfBlock inR{}, inL{}, outR{}, outL{};

    for(size_t i{}; i < DESC::SIZE::FEISTEL_ROUND_SOUTPUT; ++i)
    {
        inR[i] = inBitset[i];
        inL[i] = inBitset[i + DESC::SIZE::FEISTEL_ROUND_SINPUT];
    }

    outL = inR;
    outR = inL ^ (FeistelFunction(inR, subKey));

    /* Concat left and right segments */

    Block outBitset{};
    for(size_t i{}; i < DESC::SIZE::FEISTEL_ROUND_SOUTPUT; ++i)
    {
        outBitset[i]                                     = outR[i];
        outBitset[i + DESC::SIZE::FEISTEL_ROUND_SOUTPUT] = outL[i];
    }

    return outBitset;
}


// -------------------------------------------------------------------------------------------------------------------
/* Feistel Round Overload (2): Operating on Segments*/
std::tuple<DES::HalfBlock, DES::HalfBlock> DES::FeistelRound(const HalfBlock& inL, const HalfBlock& inR, const SubKey& subKey)
{
    return { inR, (FeistelFunction(inR, subKey) ^ inL) };
}


// -------------------------------------------------------------------------------------------------------------------
/* Key Schedule */
DES::SubKeyArray DES::KeySchedule(const Key& key)
{
    std::array<SubKey, DESC::SIZE::ROUND_COUNT> keys{};
    auto [kl, kr] = PC1MapSplitOut(key);
    
    /* Rounds of Key Generation */
    for (size_t i{}; i < DESC::SIZE::ROUND_COUNT; ++i)
    {
        LeftRotate(kl, DESC::keyShiftArray[i]);
        LeftRotate(kr, DESC::keyShiftArray[i]);
        keys[i] = PC2Map(kl, kr);
    }
    
    return keys;
}


// -------------------------------------------------------------------------------------------------------------------
/* DES Encryption */
DES::Block DES::Encrypt(const Block& plaintext,  std::optional<Key> key, bool printSteps)
{    
    HalfBlock L{}, R{};
    /* 1. Derive subkeys */
    auto keys{ KeySchedule(key.value_or(mKey)) };
    
    /* 2. Pass the plaintext through IP and split the output */
    std::tie(L, R) = IPMapSplitOut(plaintext);
    if (printSteps)
        std::cout << "(0)\tL: " << L << " R: " << R << std::endl;
        // std::cout << "(0)\tL: " << std::hex << L.to_ullong() << " R: " << R.to_ullong() << std::endl;
    

    /* 3. Pass the Data Through Feistel Rounds */
    size_t i{};
    std::for_each(keys.begin(), keys.end(), [&](const auto& key)
    {
        std::tie(L, R) = FeistelRound(L, R, key);
        if (printSteps)
            std::cout << std::dec << "(" << ++i << ")\t" << "L: " << L << " R: " << R << " key: " << key << std::endl;
            // std::cout << std::hex << "(" << i+1 << ")\t" << "L: " << L.to_ullong() << " R: " << R.to_ullong() << " key: " << keys[i].to_ullong() << std::endl;
    });

    /* 4. Reverse the order of the two segments into the 64-bit block and pass through FP */
    return FPMap(R, L);
}


// -------------------------------------------------------------------------------------------------------------------
/* DES Decryption */
DES::Block DES::Decrypt(const Block& ciphertext, std::optional<Key> key, bool printSteps)
{
    HalfBlock L{}, R{};
    /* 1. Derive subkeys */
    auto keys{ KeySchedule(key.value_or(mKey)) };
    
    /* 2. Pass the plaintext through IP and split the output */
    std::tie(L, R) = IPMapSplitOut(ciphertext);
    if (printSteps)
        std::cout << "(0)\tL: " << L << " R: " << R << std::endl;
        // std::cout << "(0)\tL: " << std::hex << L.to_ullong() << " R: " << R.to_ullong() << std::endl;
    

    /* 3. Pass the Data Through Feistel Rounds Using Reverse Order For Keys */
    size_t i{};
    std::for_each(keys.rbegin(), keys.rend(), [&](const auto& key)
    {
        std::tie(L, R) = FeistelRound(L, R, key);
        if (printSteps)
            std::cout << std::dec << "(" << ++i << ")\t" << "L: " << L << " R: " << R << " key: " << key << std::endl;
            // std::cout << std::hex << "(" << i+1 << ")\t" << "L: " << L.to_ullong() << " R: " << R.to_ullong() << " key: " << keys[i].to_ullong() << std::endl;
    });

    /* 4. Reverse the order of the two output segments, concatenate them and form a 64-bit block, and finally feed them into FP */
    return FPMap(R, L);
}

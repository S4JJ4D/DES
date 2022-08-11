/**
 * @file des.cc
 *
 * @brief This source code implements DES
 *
 * @author S4JJ4D
 *
 */

#include <iostream>

#include "des.hpp"


unsigned int DES::SBoxMap(DESC::SBoxType SBoxType, unsigned int inputVal)
{
    /* Extract Column-Select */
    auto colSelect{ (inputVal&0b011110) >> 1 };
    /* Extract Row-Select */
    auto tempVal{ (inputVal&100001) };
    auto rowSelect{ ( (tempVal >> 5) & 1 )*2 + (tempVal & 1) };
    /* Due to zero-indexing of arrays, a subtraction of 1 is necessary */
    return DESC::SBoxArray[static_cast<unsigned int>(SBoxType)][rowSelect][colSelect];
}


std::bitset<DESC::SIZE::SBOX_OUTPUT> DES::SBoxMap(DESC::SBoxType SBoxType, const std::bitset<DESC::SIZE::SBOX_INPUT>& inBitset)
{
    return SBoxMap(SBoxType, inBitset.to_ulong());
}


/* Initial Permutation */
std::bitset<DESC::SIZE::IP_OUTPUT> DES::IPMap(const std::bitset<DESC::SIZE::IP_INPUT>& inBitset)
{
    std::bitset<DESC::SIZE::IP_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::IP_INPUT; ++i)
    {
        k = DESC::SIZE::IP_OUTPUT - 1 - i;
        idx = DESC::SIZE::IP_INPUT - DESC::IPArray[i/DESC::IPnRow][i%DESC::IPnRow];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}


/* Initial Permutation - split the output into two segments */
std::tuple<std::bitset<DESC::SIZE::IP_SOUTPUT>, std::bitset<DESC::SIZE::IP_SOUTPUT>> DES::IPMapSplitOut(const std::bitset<DESC::SIZE::IP_INPUT>& inBitset)
{
    std::bitset<DESC::SIZE::IP_SOUTPUT> leftBitset{}, rightBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::IP_INPUT; ++i)
    {
        k = DESC::SIZE::IP_INPUT - 1 - i;
        idx = DESC::SIZE::IP_INPUT - DESC::IPArray[i/DESC::IPnRow][i%DESC::IPnRow];
        if (i < DESC::SIZE::IP_SOUTPUT)
            leftBitset[k - DESC::SIZE::IP_SOUTPUT] = inBitset[idx];
        else
            rightBitset[k]                        = inBitset[idx];
    }

    return {leftBitset, rightBitset};
}


/* Final Permutation - Overload (1) */
std::bitset<DESC::SIZE::FP_OUTPUT> DES::FPMap(const std::bitset<DESC::SIZE::FP_INPUT>& inBitset)
{
    std::bitset<DESC::SIZE::FP_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::FP_OUTPUT; ++i)
    {
        k = DESC::SIZE::FP_OUTPUT - 1 - i;
        idx = DESC::SIZE::FP_INPUT - DESC::FPArray[i/DESC::IPnCol][i%DESC::IPnCol];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}


/* Final Permutation - Overload (2) */
std::bitset<DESC::SIZE::FP_OUTPUT> DES::FPMap(const std::bitset<DESC::SIZE::FP_SINPUT>& inL, const std::bitset<DESC::SIZE::FP_SINPUT>& inR)
{
    std::bitset<DESC::SIZE::FP_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::FP_OUTPUT; ++i)
    {
        k = DESC::SIZE::FP_OUTPUT - 1 - i;
        idx = DESC::FPArray[i/DESC::IPnCol][i%DESC::IPnCol];
        outBitset[k] = (idx <= DESC::SIZE::FP_SINPUT) ? inL[DESC::SIZE::FP_SINPUT - idx] : inR[DESC::SIZE::FP_SINPUT - (idx - DESC::SIZE::FP_SINPUT)];
    }

    return outBitset;
}


/* Permuted choice 1 */
std::bitset<DESC::SIZE::PC1_OUTPUT> DES::PC1Map(const std::bitset<DESC::SIZE::PC1_INPUT>& inBitset)
{
    std::bitset<DESC::SIZE::PC1_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::PC1_OUTPUT; ++i)
    {
        k = DESC::SIZE::PC1_OUTPUT - 1 - i;
        idx = DESC::SIZE::PC1_INPUT - DESC::PC1Array[i/DESC::PC1nCol][i%DESC::PC1nCol];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}


/* Permuted choice 1 - Splits output to left and right segments */
std::tuple<std::bitset<DESC::SIZE::PC1_SOUTPUT>, std::bitset<DESC::SIZE::PC1_SOUTPUT>> DES::PC1MapSplitOut(const std::bitset<DESC::SIZE::PC1_INPUT>& inBitset)
{ 
    std::bitset<DESC::SIZE::PC1_SOUTPUT> leftBitset{}, rightBitset{};
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


/* Permuted Choice 2 (PC-2) */
std::bitset<DESC::SIZE::PC2_OUTPUT> DES::PC2Map(const std::bitset<DESC::SIZE::PC2_SINPUT>& inL, const std::bitset<DESC::SIZE::PC2_SINPUT>& inR)
{
    std::bitset<DESC::SIZE::PC2_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::PC2_OUTPUT; ++i)
    {
        k = DESC::SIZE::PC2_OUTPUT - 1 - i;
        idx = DESC::PC2Array[i/DESC::PC2nCol][i%DESC::PC2nCol];
        outBitset[k] = (idx <= DESC::SIZE::PC2_SINPUT) ? inL[DESC::SIZE::PC2_SINPUT - idx] : inR[DESC::SIZE::PC2_SINPUT - (idx - DESC::SIZE::PC2_SINPUT)];
    }

    return outBitset;
}


std::bitset<DESC::SIZE::EBOX_OUTPUT> DES::ExpansionFunction(const std::bitset<DESC::SIZE::EBOX_INPUT>& inBitset)
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

/* 32-bit permutation performed at the final stage of f-function */
std::bitset<DESC::SIZE::P_OUTPUT> DES::PMap(const std::bitset<DESC::SIZE::P_INPUT>& inBitset)
{
    std::bitset<DESC::SIZE::P_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DESC::SIZE::P_OUTPUT; ++i)
    {
        k = DESC::SIZE::P_OUTPUT - 1 - i;
        idx = DESC::SIZE::P_INPUT - DESC::PArray[i/DESC::PnCol][i%DESC::PnCol];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}

/* The Feistel (F) function */
std::bitset<DESC::SIZE::FFUNC_OUTPUT> DES::FeistelFunction(const std::bitset<DESC::SIZE::FFUNC_INPUT>& inBitset, const std::bitset<DESC::SIZE::SUBKEY>& key)
{
    std::bitset<DESC::SIZE::FFUNC_OUTPUT> outSBoxOperation{};
    /** 1. Expansion
     *  2. Key-Mixing
    */
    auto preSBoxData{ ExpansionFunction(inBitset) ^ key };

    std::bitset<DESC::SIZE::SBOX_INPUT> inSBox{};
    size_t k1{};

    /* There are 8 SBoxes to Operate */
    for(size_t SBoxStepCount{}; SBoxStepCount < DESC::SIZE::SBOX_COUNT; ++SBoxStepCount)
    {
        inSBox = 0;
        for (size_t i{}, k2{}; i < DESC::SIZE::SBOX_INPUT; ++i)
        {
            k2 = DESC::SIZE::SUBKEY - 1 - (DESC::SIZE::SBOX_INPUT * SBoxStepCount + i);
            inSBox[DESC::SIZE::SBOX_INPUT - 1 - i] = preSBoxData[k2];
        }

        std::bitset<DESC::SIZE::SBOX_OUTPUT> outSBox{ SBoxMap(static_cast<DESC::SBoxType>(SBoxStepCount), inSBox) };

        for (size_t i{}, k2{}; i < DESC::SIZE::SBOX_OUTPUT; ++i)
        {
            k2 = DESC::SIZE::FFUNC_OUTPUT - 1 - (DESC::SIZE::SBOX_OUTPUT * SBoxStepCount + i);
            outSBoxOperation[k2] = outSBox[DESC::SIZE::SBOX_OUTPUT - 1 - i];
        }

    }

    return PMap(outSBoxOperation);
}

/* Feistel Round Overload (1) */
std::bitset<DESC::SIZE::FEISTEL_ROUND_OUTPUT> DES::FeistelRound(const std::bitset<DESC::SIZE::FEISTEL_ROUND_INPUT>& inBitset, const std::bitset<DESC::SIZE::SUBKEY>& key)
{
    /* R: Right hand side bitset */
    /* L: Left  hand side bitset */
    std::bitset<DESC::SIZE::FEISTEL_ROUND_SOUTPUT> inR{}, inL{}, outR{}, outL{};

    for(size_t i{}; i < DESC::SIZE::FEISTEL_ROUND_SOUTPUT; ++i)
    {
        inR[i] = inBitset[i];
        inL[i] = inBitset[i + DESC::SIZE::FEISTEL_ROUND_SINPUT];
    }

    outL = inR;
    outR = inL ^ (FeistelFunction(inR, key));

    /* Concat left and right segments */

    std::bitset<DESC::SIZE::FEISTEL_ROUND_OUTPUT> outBitset{};
    for(size_t i{}; i < DESC::SIZE::FEISTEL_ROUND_SOUTPUT; ++i)
    {
        outBitset[i]                                    = outR[i];
        outBitset[i + DESC::SIZE::FEISTEL_ROUND_SOUTPUT] = outL[i];
    }

    return outBitset;
}

/* Feistel Round Overload (2): Operating on Segments*/
std::tuple<std::bitset<DESC::SIZE::FEISTEL_ROUND_SOUTPUT>, std::bitset<DESC::SIZE::FEISTEL_ROUND_SOUTPUT>>
DES::FeistelRound(const std::bitset<DESC::SIZE::FEISTEL_ROUND_SINPUT>& inL, const std::bitset<DESC::SIZE::FEISTEL_ROUND_SINPUT>& inR, const std::bitset<DESC::SIZE::SUBKEY>& key)
{
    return { inR, (FeistelFunction(inR, key) ^ inL) };
}


/* Key Schedule */
std::array<std::bitset<DESC::SIZE::SUBKEY>, DESC::SIZE::ROUND_COUNT> DES::KeySchedule(const std::bitset<DESC::SIZE::KEY>& key)
{
    std::array<std::bitset<DESC::SIZE::SUBKEY>, DESC::SIZE::ROUND_COUNT> keys{};
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

/* DES Encryption */
std::bitset<DESC::SIZE::DES_BLOCKSIZE> DES::Encrypt(const std::bitset<DESC::SIZE::DES_BLOCKSIZE>& plaintext,  std::optional<std::bitset<DESC::SIZE::KEY>> key, bool printSteps)
{    
    std::bitset<DESC::SIZE::DES_HALF_BLOCKSIZE> L{}, R{};
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

/* DES Decryption */
std::bitset<DESC::SIZE::DES_BLOCKSIZE> DES::Decrypt(const std::bitset<DESC::SIZE::DES_BLOCKSIZE>& ciphertext, std::optional<std::bitset<DESC::SIZE::KEY>> key, bool printSteps)
{
    std::bitset<DESC::SIZE::DES_HALF_BLOCKSIZE> L{}, R{};
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

    /* 4. Reverse the order of the two segments into the 64-bit block and pass through FP */
    return FPMap(R, L);
}
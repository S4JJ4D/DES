#include <iostream>
#include "des.hpp"


unsigned int SBoxMap(DES::SBoxType SBoxType, unsigned int inputVal)
{
    /* Extract Column-Select */
    auto colSelect{ (inputVal&0b011110) >> 1 };
    /* Extract Row-Select */
    auto tempVal{ (inputVal&100001) };
    auto rowSelect{ ( (tempVal >> 5) & 1 )*2 + (tempVal & 1) };
    /* Due to zero-indexing of arrays, a subtraction of 1 is necessary */
    return DES::SBoxArray[static_cast<unsigned int>(SBoxType)][rowSelect][colSelect];
}


std::bitset<DES::SIZE::SBOX_OUTPUT> SBoxMap(DES::SBoxType SBoxType, const std::bitset<DES::SIZE::SBOX_INPUT>& inBitset)
{
    return SBoxMap(SBoxType, inBitset.to_ulong());
}


/* Initial Permutation */
std::bitset<DES::SIZE::IP_OUTPUT> IPMap(const std::bitset<DES::SIZE::IP_INPUT>& inBitset)
{
    std::bitset<DES::SIZE::IP_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DES::SIZE::IP_INPUT; ++i)
    {
        k = DES::SIZE::IP_OUTPUT - 1 - i;
        idx = DES::SIZE::IP_INPUT - DES::IPArray[i/DES::IPnRow][i%DES::IPnRow];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}


/* Initial Permutation - split the output into two segments */
std::tuple<std::bitset<DES::SIZE::IP_SOUTPUT>, std::bitset<DES::SIZE::IP_SOUTPUT>> IPMapSplitOut(const std::bitset<DES::SIZE::IP_INPUT>& inBitset)
{
    std::bitset<DES::SIZE::IP_SOUTPUT> leftBitset{}, rightBitset{};
    for(size_t i{}, idx{}, k{}; i < DES::SIZE::IP_INPUT; ++i)
    {
        k = DES::SIZE::IP_INPUT - 1 - i;
        idx = DES::SIZE::IP_INPUT - DES::IPArray[i/DES::IPnRow][i%DES::IPnRow];
        if (i < DES::SIZE::IP_SOUTPUT)
            leftBitset[k - DES::SIZE::IP_SOUTPUT] = inBitset[idx];
        else
            rightBitset[k]                        = inBitset[idx];
    }

    return {leftBitset, rightBitset};
}


/* Final Permutation - Overload (1) */
std::bitset<DES::SIZE::FP_OUTPUT> FPMap(const std::bitset<DES::SIZE::FP_INPUT>& inBitset)
{
    std::bitset<DES::SIZE::FP_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DES::SIZE::FP_OUTPUT; ++i)
    {
        k = DES::SIZE::FP_OUTPUT - 1 - i;
        idx = DES::SIZE::FP_INPUT - DES::FPArray[i/DES::IPnCol][i%DES::IPnCol];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}


/* Final Permutation - Overload (2) */
std::bitset<DES::SIZE::FP_OUTPUT> FPMap(const std::bitset<DES::SIZE::FP_SINPUT>& inL, const std::bitset<DES::SIZE::FP_SINPUT>& inR)
{
    std::bitset<DES::SIZE::FP_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DES::SIZE::FP_OUTPUT; ++i)
    {
        k = DES::SIZE::FP_OUTPUT - 1 - i;
        idx = DES::FPArray[i/DES::IPnCol][i%DES::IPnCol];
        outBitset[k] = (idx <= DES::SIZE::FP_SINPUT) ? inL[DES::SIZE::FP_SINPUT - idx] : inR[DES::SIZE::FP_SINPUT - (idx - DES::SIZE::FP_SINPUT)];
    }

    return outBitset;
}


/* Permuted choice 1 */
std::bitset<DES::SIZE::PC1_OUTPUT> PC1Map(const std::bitset<DES::SIZE::PC1_INPUT>& inBitset)
{
    std::bitset<DES::SIZE::PC1_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DES::SIZE::PC1_OUTPUT; ++i)
    {
        k = DES::SIZE::PC1_OUTPUT - 1 - i;
        idx = DES::SIZE::PC1_INPUT - DES::PC1Array[i/DES::PC1nCol][i%DES::PC1nCol];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}


/* Permuted choice 1 - Splits output to left and right segments */
std::tuple<std::bitset<DES::SIZE::PC1_SOUTPUT>, std::bitset<DES::SIZE::PC1_SOUTPUT>> PC1MapSplitOut(const std::bitset<DES::SIZE::PC1_INPUT>& inBitset)
{ 
    std::bitset<DES::SIZE::PC1_SOUTPUT> leftBitset{}, rightBitset{};
    for(size_t i{}, idx{}, k{}; i < DES::SIZE::PC1_OUTPUT; ++i)
    {
        k = DES::SIZE::PC1_OUTPUT - 1 - i;
        idx = DES::SIZE::PC1_INPUT - DES::PC1Array[i/DES::PC1nCol][i%DES::PC1nCol];
        if (i < DES::SIZE::PC1_SOUTPUT)
            leftBitset[k - DES::SIZE::PC1_SOUTPUT] = inBitset[idx];
        else
            rightBitset[k]                         = inBitset[idx];
    }

    return {leftBitset, rightBitset};
}


/* Permuted Choice 2 (PC-2) */
std::bitset<DES::SIZE::PC2_OUTPUT> PC2Map(const std::bitset<DES::SIZE::PC2_SINPUT>& inL, const std::bitset<DES::SIZE::PC2_SINPUT>& inR)
{
    std::bitset<DES::SIZE::PC2_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DES::SIZE::PC2_OUTPUT; ++i)
    {
        k = DES::SIZE::PC2_OUTPUT - 1 - i;
        idx = DES::PC2Array[i/DES::PC2nCol][i%DES::PC2nCol];
        outBitset[k] = (idx <= DES::SIZE::PC2_SINPUT) ? inL[DES::SIZE::PC2_SINPUT - idx] : inR[DES::SIZE::PC2_SINPUT - (idx - DES::SIZE::PC2_SINPUT)];
    }

    return outBitset;
}



std::bitset<DES::SIZE::EBOX_OUTPUT> ExpansionFunction(const std::bitset<DES::SIZE::EBOX_INPUT>& inBitset)
{
    std::bitset<DES::SIZE::EBOX_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DES::SIZE::EBOX_OUTPUT; ++i)
    {
        k = DES::SIZE::EBOX_OUTPUT - 1 - i;
        idx = DES::SIZE::EBOX_INPUT - DES::ExpansionArray[i/DES::EXnCol][i%DES::EXnCol];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}

/* 32-bit permutation performed at the final stage of f-function */
std::bitset<DES::SIZE::P_OUTPUT> PMap(const std::bitset<DES::SIZE::P_INPUT>& inBitset)
{
    std::bitset<DES::SIZE::P_OUTPUT> outBitset{};
    for(size_t i{}, idx{}, k{}; i < DES::SIZE::P_OUTPUT; ++i)
    {
        k = DES::SIZE::P_OUTPUT - 1 - i;
        idx = DES::SIZE::P_INPUT - DES::PArray[i/DES::PnCol][i%DES::PnCol];
        outBitset[k] = inBitset[idx];
    }

    return outBitset;
}

/* The Feistel (F) function */
std::bitset<DES::SIZE::FFUNC_OUTPUT> FeistelFunction(const std::bitset<DES::SIZE::FFUNC_INPUT>& inBitset, const std::bitset<DES::SIZE::SUBKEY>& key)
{
    std::bitset<DES::SIZE::FFUNC_OUTPUT> outSBoxOperation{};
    /** 1. Expansion
     *  2. Key-Mixing
    */
    auto preSBoxData{ ExpansionFunction(inBitset) ^ key };

    std::bitset<DES::SIZE::SBOX_INPUT> inSBox{};
    size_t k1{};

    /* There are 8 SBoxes to Operate */
    for(size_t SBoxStepCount{}; SBoxStepCount < DES::SIZE::SBOX_COUNT; ++SBoxStepCount)
    {
        inSBox = 0;
        for (size_t i{}, k2{}; i < DES::SIZE::SBOX_INPUT; ++i)
        {
            k2 = DES::SIZE::SUBKEY - 1 - (DES::SIZE::SBOX_INPUT * SBoxStepCount + i);
            inSBox[DES::SIZE::SBOX_INPUT - 1 - i] = preSBoxData[k2];
        }

        std::bitset<DES::SIZE::SBOX_OUTPUT> outSBox{ SBoxMap(static_cast<DES::SBoxType>(SBoxStepCount), inSBox) };

        for (size_t i{}, k2{}; i < DES::SIZE::SBOX_OUTPUT; ++i)
        {
            k2 = DES::SIZE::FFUNC_OUTPUT - 1 - (DES::SIZE::SBOX_OUTPUT * SBoxStepCount + i);
            outSBoxOperation[k2] = outSBox[DES::SIZE::SBOX_OUTPUT - 1 - i];
        }

    }

    return PMap(outSBoxOperation);
}

/* Feistel Round Overload (1) */
std::bitset<DES::SIZE::FEISTEL_ROUND_OUTPUT> FeistelRound(const std::bitset<DES::SIZE::FEISTEL_ROUND_INPUT>& inBitset, const std::bitset<DES::SIZE::SUBKEY>& key)
{
    /* R: Right hand side bitset */
    /* L: Left  hand side bitset */
    std::bitset<DES::SIZE::FEISTEL_ROUND_SOUTPUT> inR{}, inL{}, outR{}, outL{};

    for(size_t i{}; i < DES::SIZE::FEISTEL_ROUND_SOUTPUT; ++i)
    {
        inR[i] = inBitset[i];
        inL[i] = inBitset[i + DES::SIZE::FEISTEL_ROUND_SINPUT];
    }

    outL = inR;
    outR = inL ^ (FeistelFunction(inR, key));

    /* Concat left and right segments */

    std::bitset<DES::SIZE::FEISTEL_ROUND_OUTPUT> outBitset{};
    for(size_t i{}; i < DES::SIZE::FEISTEL_ROUND_SOUTPUT; ++i)
    {
        outBitset[i]                                    = outR[i];
        outBitset[i + DES::SIZE::FEISTEL_ROUND_SOUTPUT] = outL[i];
    }

    return outBitset;
}

/* Feistel Round Overload (2): Operating on Segments*/
std::tuple<std::bitset<DES::SIZE::FEISTEL_ROUND_SOUTPUT>, std::bitset<DES::SIZE::FEISTEL_ROUND_SOUTPUT>>
FeistelRound(const std::bitset<DES::SIZE::FEISTEL_ROUND_SINPUT>& inL, const std::bitset<DES::SIZE::FEISTEL_ROUND_SINPUT>& inR, const std::bitset<DES::SIZE::SUBKEY>& key)
{
    return { inR, (FeistelFunction(inR, key) ^ inL) };
}


/* Key Schedule */
std::array<std::bitset<DES::SIZE::SUBKEY>, DES::SIZE::ROUND_COUNT> KeySchedule(const std::bitset<DES::SIZE::KEY>& key)
{
    std::array<std::bitset<DES::SIZE::SUBKEY>, DES::SIZE::ROUND_COUNT> keys{};
    auto [kl, kr] = PC1MapSplitOut(key);
    
    /* Rounds of Key Generation */
    for (size_t i{}; i < DES::SIZE::ROUND_COUNT; ++i)
    {
        LeftRotate(kl, DES::keyShiftArray[i]);
        LeftRotate(kr, DES::keyShiftArray[i]);
        keys[i] = PC2Map(kl, kr);
    }
    
    return keys;
}

/* DES Encryption */
std::bitset<DES::SIZE::DES_BLOCKSIZE> EncryptDES(const std::bitset<DES::SIZE::DES_BLOCKSIZE>& plaintext, const std::bitset<DES::SIZE::KEY>& key, bool printSteps)
{    
    std::bitset<DES::SIZE::DES_HALF_BLOCKSIZE> L{}, R{};
    /* 1. Derive subkeys */
    auto keys{ KeySchedule(key) };
    
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
std::bitset<DES::SIZE::DES_BLOCKSIZE> DecryptDES(const std::bitset<DES::SIZE::DES_BLOCKSIZE>& ciphertext, const std::bitset<DES::SIZE::KEY>& key, bool printSteps)
{
    std::bitset<DES::SIZE::DES_HALF_BLOCKSIZE> L{}, R{};
    /* 1. Derive subkeys */
    auto keys{ KeySchedule(key) };
    
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
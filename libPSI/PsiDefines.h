#pragma once
#include <cryptoTools/Common/Defines.h>
#define NTL_Threads
#define  DEBUG
#include "PsiDefines.h"
#include <NTL/ZZ_p.h>
#include <NTL/vec_ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <NTL/ZZ.h>
using namespace NTL;
//#define NTL_Threads_ON

namespace osuCrypto
{
	static const u64 stepSize(1<<8);
	static const u8 numSuperBlocks(4); //wide of T (or field size)
	static const u64 numDummies(1);
	static const u64 maxBinSize(50);
	static const u64 expBinsize(1 << 6);
	static std::vector<block> mOneBlocks(128);
	static const u64 primeLong(128);


	static __m128i mm_bitshift_right(__m128i x, unsigned count)
	{
		__m128i carry = _mm_slli_si128(x, 8);   // old compilers only have the confusingly named _mm_slli_si128 synonym
		if (count >= 64)
			return _mm_slli_epi64(carry, count - 64);  // the non-carry part is all zero, so return early
													   // else
		return _mm_or_si128(_mm_slli_epi64(x, count), _mm_srli_epi64(carry, 64 - count));

	}


	static __m128i mm_bitshift_left(__m128i x, unsigned count)
	{
		__m128i carry = _mm_srli_si128(x, 8);   // old compilers only have the confusingly named _mm_slli_si128 synonym
		if (count >= 64)
			return _mm_srli_epi64(carry, count - 64);  // the non-carry part is all zero, so return early

		return _mm_or_si128(_mm_srli_epi64(x, count), _mm_slli_epi64(carry, 64 - count));
	}

	inline void fillOneBlock(std::vector<block>& blks)
	{
		for (int i = 0; i < blks.size(); ++i)
			blks[i] = mm_bitshift_right(OneBlock, i);
	}

	static void prfOtRows(block* inputs, u64 inputSize, std::vector<std::array<block, numSuperBlocks>>& outputs, std::vector<AES> arrAes)
	{
		std::vector<block> ciphers(inputSize);
		outputs.resize(inputSize);

		for (int j = 0; j < numSuperBlocks - 1; ++j) //1st 3 blocks
			for (int i = 0; i < 128; ++i) //for each column
			{
				arrAes[j * 128 + i].ecbEncBlocks(inputs, inputSize, ciphers.data()); //do many aes at the same time for efficeincy

				for (u64 idx = 0; idx < inputSize; idx++)
				{
					ciphers[idx] = ciphers[idx]&mOneBlocks[i];
					outputs[idx][j] = outputs[idx][j] ^ ciphers[idx];
				}
			}

		
		int j = numSuperBlocks - 1;
		for (int i = 0; i < 128; ++i)
		{
			if (j * 128 + i < arrAes.size()) {
				arrAes[j * 128 + i].ecbEncBlocks(inputs, inputSize, ciphers.data()); //do many aes at the same time for efficeincy
				for (u64 idx = 0; idx < inputSize; idx++)
				{
					ciphers[idx] = ciphers[idx] & mOneBlocks[i];
					outputs[idx][j] = outputs[idx][j] ^ ciphers[idx];
				}
			}
			else {
				break;
			}
		}

	}

	static void prfOtRow(block& input, std::array<block, numSuperBlocks>& output, std::vector<AES> arrAes)
	{
		block cipher;

		for (int j = 0; j < numSuperBlocks - 1; ++j) //1st 3 blocks
			for (int i = 0; i < 128; ++i) //for each column
			{
				arrAes[j * 128 + i].ecbEncBlock(input, cipher);
				cipher= cipher& mOneBlocks[i];
				output[j] = output[j] ^ cipher;
			}


		int j = numSuperBlocks - 1;
		for (int i = 0; i < 128; ++i)
		{
			if (j * 128 + i < arrAes.size()) {
				arrAes[j * 128 + i].ecbEncBlock(input, cipher);
				cipher = cipher& mOneBlocks[i];
				output[j] = output[j] ^ cipher;
			}
			else {
				break;
			}
		}

	}

}

#pragma once

// Hacked together function to get a hash for a sprite
// https://learn.microsoft.com/en-us/windows/win32/seccng/creating-a-hash-with-cng

#ifdef _WIN32

#pragma comment(lib, "bcrypt.lib")

#include <bcrypt.h>
#include <stdio.h>
#include <windows.h>

#include <algorithm>
#include <cstdint>
#include <vector>

#include "olc/olcPixelGameEngine.h"

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)

static std::vector<std::uint8_t> Hash(olc::Sprite& sprite)
{
    std::vector<std::uint8_t> hashOut;

    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    DWORD cbData = 0,
          cbHash = 0,
          cbHashObject = 0;
    PBYTE pbHashObject = NULL;
    PBYTE pbHash = NULL;
    PBYTE data = NULL;
    ULONG dataSize = 0;

    data = reinterpret_cast<PBYTE>(sprite.GetData());
    dataSize = sprite.width * sprite.height * sizeof(olc::Pixel);

    // open an algorithm handle
    if (!NT_SUCCESS(status = BCryptOpenAlgorithmProvider(
                        &hAlg,
                        BCRYPT_SHA256_ALGORITHM,
                        NULL,
                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
        goto Cleanup;
    }

    // calculate the size of the buffer to hold the hash object
    if (!NT_SUCCESS(status = BCryptGetProperty(
                        hAlg,
                        BCRYPT_OBJECT_LENGTH,
                        (PBYTE)&cbHashObject,
                        sizeof(DWORD),
                        &cbData,
                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
        goto Cleanup;
    }

    // allocate the hash object on the heap
    pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
    if (NULL == pbHashObject)
    {
        wprintf(L"**** memory allocation failed\n");
        goto Cleanup;
    }

    // calculate the length of the hash
    if (!NT_SUCCESS(status = BCryptGetProperty(
                        hAlg,
                        BCRYPT_HASH_LENGTH,
                        (PBYTE)&cbHash,
                        sizeof(DWORD),
                        &cbData,
                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptGetProperty\n", status);
        goto Cleanup;
    }

    // allocate the hash buffer on the heap
    pbHash = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHash);
    if (NULL == pbHash)
    {
        wprintf(L"**** memory allocation failed\n");
        goto Cleanup;
    }

    // create a hash
    if (!NT_SUCCESS(status = BCryptCreateHash(
                        hAlg,
                        &hHash,
                        pbHashObject,
                        cbHashObject,
                        NULL,
                        0,
                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptCreateHash\n", status);
        goto Cleanup;
    }

    // hash some data
    if (!NT_SUCCESS(status = BCryptHashData(
                        hHash,
                        data,
                        dataSize,
                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptHashData\n", status);
        goto Cleanup;
    }

    // close the hash
    if (!NT_SUCCESS(status = BCryptFinishHash(
                        hHash,
                        pbHash,
                        cbHash,
                        0)))
    {
        wprintf(L"**** Error 0x%x returned by BCryptFinishHash\n", status);
        goto Cleanup;
    }
    try
    {
        hashOut.resize(cbHash);
        std::copy(pbHash, pbHash + cbHash, hashOut.begin());
    }
    catch (...)
    {
    }
Cleanup:

    if (hAlg)
    {
        BCryptCloseAlgorithmProvider(hAlg, 0);
    }

    if (hHash)
    {
        BCryptDestroyHash(hHash);
    }

    if (pbHashObject)
    {
        HeapFree(GetProcessHeap(), 0, pbHashObject);
    }

    if (pbHash)
    {
        HeapFree(GetProcessHeap(), 0, pbHash);
    }

    return hashOut;
}

#endif

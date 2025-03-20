#include "StackAllocator.h"
#include <cstdlib>   // malloc, free
#include "Debug/DebugConsole.h"

void* StackAllocator::Allocate(size_t sz)
{
    if (sz > cbChunkPayload)
        return nullptr;

    if (m_pFreeSlots != nullptr)
    {
        void* res = m_pFreeSlots;
        m_pFreeSlots = *reinterpret_cast<void**>(m_pFreeSlots);
        return res;
    }
    else
    {
        if (m_freeBytes >= sz)
        {
            m_freeBytes -= sz;
            void* res = m_pFirstByte;
            m_pFirstByte += sz; 
            return res;
        }
        allocateBlock();
        return Allocate(sz);
    }
	UE_LOG("StackAllocator new Memory : %d", sz);
}

void StackAllocator::allocateBlock()
{
    unsigned char* newBlock = reinterpret_cast<unsigned char*>(std::malloc(cbChunkAlloc));
    if (newBlock == nullptr)
        return; // 할당 실패 시 반환

    CMemChunk* pChunk = reinterpret_cast<CMemChunk*>(newBlock);
    pChunk->pNext = m_pChunk;
    m_pChunk = pChunk;
    m_pFirstByte = m_pChunk->data();
    m_freeBytes = cbChunkPayload;
}

StackAllocator::StackAllocator()
    : m_pChunk(nullptr)
    , m_pFirstByte(nullptr)
    , m_freeBytes(0)
    , m_pFreeSlots(nullptr)
{
}

StackAllocator::~StackAllocator()
{
    while (m_pChunk != nullptr)
    {
        CMemChunk* pNext = m_pChunk->pNext;
        std::free(m_pChunk);
        m_pChunk = pNext;
    }
}

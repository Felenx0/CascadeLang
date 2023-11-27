#pragma once

class AreaAllocator {
public:
    inline explicit AreaAllocator(size_t bytes) : m_size(bytes) {
        m_buffer = static_cast<byte *>(malloc(m_size));
        m_offset = m_buffer;
    }

    template<typename T>
    inline T* alloc() {
        void* offset = m_offset;
        m_offset += sizeof(T);
        return static_cast<T*>(offset);
    }

    inline AreaAllocator(const AreaAllocator& other) = delete;

    inline AreaAllocator operator=(const AreaAllocator& other) = delete;

    inline ~AreaAllocator() {
        free(m_buffer);
    }
private:
    size_t m_size;
    byte* m_buffer;
    byte* m_offset;
};

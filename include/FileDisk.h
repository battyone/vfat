#ifndef VFAT_FILEDISK_H
#define VFAT_FILEDISK_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

namespace org::vfat
{
    class FileDisk
    {
    private:
        const char *fileName;
        FILE *filePtr;

    public:
        FileDisk(const char* fileName);
        ~FileDisk();
        void Create();
        void Open();
        void Close();
        void Delete();
        void Read(uint8_t *buffer, long int fileOffset, size_t count) const;
        void Write(uint8_t *buffer, long int fileOffset, size_t count) const;
    };
}

#endif /* VFAT_FILEDISK_H */
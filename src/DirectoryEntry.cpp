#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "../include/Common.h"
#include "../include/BinaryReader.h"
#include "../include/DirectoryEntry.h"

using namespace org::vfat;

#define FDE_ENTRYTYPE_OFFSET 0
//#define FDE_SECONDARYCOUNT_OFFSET 1
#define FDE_ATTRIBUTES_OFFSET 4
#define FDE_CREATED_OFFSET 8
#define FDE_LASTMODIFIED_OFFSET 12
#define FDE_LASTACCESSED_OFFSET 16
#define FDE_NAMELENGTH_OFFSET 20
#define FDE_FIRSTCLUSTER_OFFSET 21
#define FDE_DATALENGTH_OFFSET 25

//#define SEDE_ENTRYTYPE_OFFSET 0
//#define SEDE_SECONDARYFLAGS_OFFSET 1
//#define SEDE_NAMELENGTH_OFFSET 3
//#define SEDE_FIRSTCLUSTER_OFFSET 20
//#define SEDE_DATALENGTH_OFFSET 24

//#define FNEDE_ENTRYTYPE_OFFSET 0
//#define FNEDE_SECODARYFLAGS_OFFSET 1
//#define FNEDE_FILENAME_OFFSET 2

#define FNDE_ENTRYTYPE_OFFSET 0
#define FNDE_FILENAME_OFFSET 2

/*
 * Attribute Offset Size Mask
 * --------------------------
 * Reserved2 6      10
 * Archive   5      1    0x20
 * Directory 4      1    0x10
 * Reserved1 3      1
 * System    2      1    0x04
 * Hidden    1      1    0x02
 * Read-Only 0      1    0x01
 */
#define DIRECTORY_MASK 0x10

//static void fde_readbuf(uint8_t *buf, struct fde *e)
//{
//    e->entry_type = read_u8(buf, FDE_ENTRYTYPE_OFFSET);
//    e->secondary_count = read_u8(buf, FDE_SECONDARYCOUNT_OFFSET);
//    e->attributes = read_u16(buf, FDE_ATTRIBUTES_OFFSET);
//    e->create = read_u32(buf, FDE_CREATE_OFFSET);
//    e->last_modified = read_u32(buf, FDE_LASTMODIFIED_OFFSET);
//    e->last_accessed = read_u32(buf, FDE_LASTACCESSED_OFFSET);
//}

//static void fde_writebuf(struct fde *e, uint8_t *buf)
//{
//    write_u8(buf, FDE_ENTRYTYPE_OFFSET, e->entry_type);
//    write_u8(buf, FDE_SECONDARYCOUNT_OFFSET, e->secondary_count);
//    write_u16(buf, FDE_ATTRIBUTES_OFFSET, e->attributes);
//    write_u32(buf, FDE_CREATE_OFFSET, e->create);
//    write_u32(buf, FDE_LASTMODIFIED_OFFSET, e->last_modified);
//    write_u32(buf, FDE_LASTACCESSED_OFFSET, e->last_accessed);
//}

//static void sede_readbuf(uint8_t *buf, struct sede *e)
//{
//    e->entry_type = read_u8(buf, SEDE_ENTRYTYPE_OFFSET);
//    e->secondary_flags = read_u8(buf, SEDE_SECONDARYFLAGS_OFFSET);
//    e->name_length = read_u8(buf, SEDE_NAMELENGTH_OFFSET);
//    e->first_cluster = read_u32(buf, SEDE_FIRSTCLUSTER_OFFSET);
//    e->data_length = read_u64(buf, SEDE_DATALENGTH_OFFSET);
//}

//static void sede_writebuf(struct sede *e, uint8_t *buf)
//{
//    write_u8(buf, SEDE_ENTRYTYPE_OFFSET, e->entry_type);
//    write_u8(buf, SEDE_SECONDARYFLAGS_OFFSET, e->secondary_flags);
//    write_u8(buf, SEDE_NAMELENGTH_OFFSET, e->name_length);
//    write_u32(buf, SEDE_FIRSTCLUSTER_OFFSET, e->first_cluster);
//    write_u64(buf, SEDE_DATALENGTH_OFFSET, e->data_length);
//}

//static void fnede_readbuf(uint8_t *buf, struct fnede *e)
//{
//    e->entry_type = read_u8(buf, FNEDE_ENTRYTYPE_OFFSET);
//    e->secondary_flags = read_u8(buf, FNEDE_SECODARYFLAGS_OFFSET);
//    memcpy(e->name, buf + FNEDE_FILENAME_OFFSET, FNEDE_UNAME_LENGTH);
//}

//static void fnede_writebuf(struct fnede *e, uint8_t *buf)
//{
//    write_u8(buf, FNEDE_ENTRYTYPE_OFFSET, e->entry_type);
//    write_u8(buf, FNEDE_SECODARYFLAGS_OFFSET, e->secondary_flags);
//    memcpy(buf + FNEDE_FILENAME_OFFSET, e->name, FNEDE_UNAME_LENGTH);
//}

//void lfnde_create(struct lfnde *e)
//{
//    e->fde = static_cast<struct fde *>(malloc(sizeof(struct fde)));
//    e->sede = static_cast<struct sede *>(malloc(sizeof(struct sede)));
//    e->fnede_list = static_cast<struct alist *>(malloc(sizeof(struct alist)));
//    alist_create(e->fnede_list, sizeof(struct fnede));

//    // Set valid default values.
//    e->fde->entry_type = FILE_DIR_ENTRY;
//    e->fde->secondary_count = 1;
//    e->sede->entry_type = STREAMEXT_DIR_ENTRY;
//    e->sede->name_length = 0;
//    e->sede->first_cluster = 0;
//}

DirectoryEntry::DirectoryEntry()
{
    this->nameLength = 0;
    this->firstCluster = 0;
    this->fndeList = new std::vector<FileNameDirectoryEntry *>();
}

DirectoryEntry::~DirectoryEntry()
{
    // Clear list
    for (uint8_t fndeIdx = 0; fndeIdx < this->fndeList->size(); fndeIdx++) {
        struct FileNameDirectoryEntry *fnde = this->fndeList->at(fndeIdx);
        delete fnde;
    }

    this->fndeList->clear();
    delete this->fndeList;
}

uint16_t DirectoryEntry::GetFat32EntryCount() const
{
    return 1 + this->fndeList->size();
}

void DirectoryEntry::Read(uint8_t *buffer)
{
    uint8_t entryType = BinaryReader::ReadUInt8(buffer, FDE_ENTRYTYPE_OFFSET);
    assert(entryType == BASE_DIR_ENTRY);

    this->attributes = BinaryReader::ReadUInt16(buffer, FDE_ATTRIBUTES_OFFSET);
    this->created = BinaryReader::ReadUInt32(buffer, FDE_CREATED_OFFSET);
    this->lastModified = BinaryReader::ReadUInt32(buffer, FDE_LASTMODIFIED_OFFSET);
    this->lastAccessed = BinaryReader::ReadUInt32(buffer, FDE_LASTACCESSED_OFFSET);
    this->nameLength = BinaryReader::ReadUInt8(buffer, FDE_NAMELENGTH_OFFSET);
    this->firstCluster = BinaryReader::ReadUInt32(buffer, FDE_FIRSTCLUSTER_OFFSET);
    this->dataLength = BinaryReader::ReadUInt32(buffer, FDE_DATALENGTH_OFFSET);

    buffer += FAT_DIR_ENTRY_SIZE;

    int fndeCount = (this->nameLength + FNDE_NAME_LENGTH - 1) / FNDE_NAME_LENGTH;
    for (uint8_t i = 0; i < fndeCount; i++) {
        uint8_t entryType = BinaryReader::ReadUInt8(buffer, FNDE_ENTRYTYPE_OFFSET);
        assert(entryType == FILENAME_DIR_ENTRY);

        struct FileNameDirectoryEntry *fnde = new FileNameDirectoryEntry();
        memcpy(fnde->nameBuffer, buffer + FNDE_FILENAME_OFFSET, FNDE_NAME_LENGTH);
        this->fndeList->push_back(fnde);

        buffer += FAT_DIR_ENTRY_SIZE;
    }
}

void DirectoryEntry::Write(uint8_t *buffer) const
{
    uint8_t fndeCount = (this->nameLength + FNDE_NAME_LENGTH - 1) / FNDE_NAME_LENGTH;
    assert(this->fndeList->size() == fndeCount);

    BinaryReader::WriteUInt8(buffer, FDE_ENTRYTYPE_OFFSET, BASE_DIR_ENTRY);
    BinaryReader::WriteUInt16(buffer, FDE_ATTRIBUTES_OFFSET, this->attributes);
    BinaryReader::WriteUInt32(buffer, FDE_CREATED_OFFSET, this->created);
    BinaryReader::WriteUInt32(buffer, FDE_LASTMODIFIED_OFFSET, this->lastModified);
    BinaryReader::WriteUInt32(buffer, FDE_LASTACCESSED_OFFSET, this->lastAccessed);
    BinaryReader::WriteUInt8(buffer, FDE_NAMELENGTH_OFFSET, this->nameLength);
    BinaryReader::WriteUInt32(buffer, FDE_FIRSTCLUSTER_OFFSET, this->firstCluster);
    BinaryReader::WriteUInt32(buffer, FDE_DATALENGTH_OFFSET, this->dataLength);

    buffer += FAT_DIR_ENTRY_SIZE;

    for (uint8_t i = 0; i < this->fndeList->size(); i++) {
        struct FileNameDirectoryEntry *fnde = this->fndeList->at(i);
        BinaryReader::WriteUInt8(buffer, FNDE_ENTRYTYPE_OFFSET, FILENAME_DIR_ENTRY);
        memcpy(buffer + FNDE_FILENAME_OFFSET, fnde->nameBuffer, FNDE_NAME_LENGTH);

        buffer += FAT_DIR_ENTRY_SIZE;
    }
}

bool DirectoryEntry::IsDir() const
{
    uint16_t attr = this->attributes;
    return (attr & DIRECTORY_MASK) != 0;
}

bool DirectoryEntry::IsFile() const
{
    uint16_t attr = this->attributes;
    return (attr & DIRECTORY_MASK) == 0;
}

void DirectoryEntry::SetIsDir(bool val)
{
    uint16_t attr = this->attributes;
    if (val) {
        attr |= DIRECTORY_MASK;
    } else {
        attr &= ~DIRECTORY_MASK;
    }

    this->attributes = attr;
}

uint32_t DirectoryEntry::GetDataLength() const
{
    return this->dataLength;
}

void DirectoryEntry::SetDataLength(uint32_t val)
{
    this->dataLength = val;
}

uint32_t DirectoryEntry::GetStartCluster() const
{
    return this->firstCluster;
}

void DirectoryEntry::SetStartCluster(uint32_t val)
{
    this->firstCluster = val;
}

void DirectoryEntry::GetName(/*out*/ char *name) const
{
    uint8_t fndeCount = (this->nameLength + FNDE_NAME_LENGTH - 1) / FNDE_NAME_LENGTH;
    assert(this->fndeList->size() == fndeCount);

    struct FileNameDirectoryEntry *fnde;
    uint8_t fndeIdx;
    uint8_t charIdx = 0;
    for (fndeIdx = 0; fndeIdx < fndeCount - 1; fndeIdx++) {
        fnde = this->fndeList->at(fndeIdx);
        for (uint8_t i = 0; i < FNDE_NAME_LENGTH; i++) {
            name[charIdx + i] = fnde->nameBuffer[i];
        }

        charIdx += FNDE_NAME_LENGTH;
    }

    fnde = this->fndeList->at(fndeIdx);
    for (uint8_t i = 0; i < this->nameLength - charIdx; i++) {
        name[charIdx + i] = fnde->nameBuffer[i];
    }

    name[this->nameLength] = '\0';
}

void DirectoryEntry::SetName(const char *name)
{
    //struct fnede fnede;
    uint8_t len = strlen(name);
    uint8_t fndeCount = this->fndeList->size();
    uint8_t newFndeCount = (len + (FNDE_NAME_LENGTH - 1)) / FNDE_NAME_LENGTH;

    // Clear list
    for (uint8_t fndeIdx = 0; fndeIdx < fndeCount; fndeIdx++) {
        struct FileNameDirectoryEntry *fnde = this->fndeList->at(fndeIdx);
        delete fnde;
    }

    this->fndeList->clear();

    // Fill the list with the new values
    uint8_t charIdx = 0;
    for (uint8_t fndeIdx = 0; fndeIdx < newFndeCount - 1; fndeIdx++) {
        struct FileNameDirectoryEntry *fnde = new FileNameDirectoryEntry();
        for (uint8_t i = 0; i < FNDE_NAME_LENGTH; i++) {
            fnde->nameBuffer[i] = name[charIdx + i];
        }

        charIdx += FNDE_NAME_LENGTH;
        this->fndeList->push_back(fnde);
    }

    // Special case for the last item
    struct FileNameDirectoryEntry *fnde = new FileNameDirectoryEntry();
    for (uint8_t i = 0; i < len - charIdx; i++) {
        fnde->nameBuffer[i] = name[charIdx + i];
    }

    this->fndeList->push_back(fnde);
    this->nameLength = len;
}

DirectoryEntry* DirectoryEntry::Clone() const
{
    DirectoryEntry *copy = new DirectoryEntry();
    copy->attributes = this->attributes;
    copy->created = this->created;
    copy->lastModified = this->lastModified;
    copy->lastAccessed = this->lastAccessed;
    copy->nameLength = this->nameLength;
    copy->firstCluster = this->firstCluster;
    copy->dataLength = this->dataLength;

    for (size_t i = 0; i < this->fndeList->size(); i++) {
        FileNameDirectoryEntry *fnde = this->fndeList->at(i);
        FileNameDirectoryEntry *copyFnde = new FileNameDirectoryEntry();
        memcpy(copyFnde->nameBuffer, fnde->nameBuffer, FNDE_NAME_LENGTH);
        copy->fndeList->push_back(copyFnde);
    }

    return copy;
}

time_t DirectoryEntry::GetCreatedTime() const
{
    return (time_t) this->created;
}

void DirectoryEntry::SetCreatedTime(time_t time)
{
    this->created = (uint32_t) time;
}

time_t DirectoryEntry::GetLastModifiedTime() const
{
    return (time_t) this->lastModified;
}

void DirectoryEntry::SetLastModifiedTime(time_t time)
{
    this->lastModified = (uint32_t) time;
}

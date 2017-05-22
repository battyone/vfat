#ifndef VFAT_CCHDIR_H
#define VFAT_CCHDIR_H

#include <stdbool.h>

#include "common.h"
#include "alist.h"
#include "cch.h"
#include "lfnde.h"

struct cchdir
{
    struct cch *chain;
    struct alist *entries;
    u32 capacity;
    bool root;
    bool dirty;
};

void cchdir_write(struct cchdir* dir, struct fdisk *disk);
//void cchdir_read(struct fdisk *disk, struct cchdir* dir);
void cchdir_read(struct fdisk *disk, struct fat *fat, struct cchdir* dir, u32 first_cluster, bool root);

void cchdir_createroot(struct fat *fat, struct cchdir *dir);
void cchdir_readroot(struct fdisk *disk, struct fat *fat, struct cchdir *dir);
void cchdir_add(struct cchdir *dir, struct lfnde *e);
void cchdir_remove(struct lfnde *e);
void cchdir_createsubdir(struct fat* fat, struct lfnde *e);
void cchdir_free(struct cchdir *dir);

#endif /* VFAT_CCHDIR_H */

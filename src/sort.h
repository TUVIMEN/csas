#ifndef SORT_H
#define SORT_H

#define SORT_NONE 0x0
#define SORT_NAME 0x1
#define SORT_CNAME 0x2
#define SORT_SIZE 0x3
#define SORT_TYPE 0x4

#define SORT_MT 0xf
#define SORT_REVERSE 0x10
#define SORT_MTR 0x1f
#define SORT_DIR_DISTINCTION 0x20
#define SORT_LDIR_DISTINCTION 0x40
#define SORT_REVERSE_DIR_DISTINCTIONS 0x80

void xfile_sort(xfile *f, const size_t size, uchar flags);

#endif

/* file.h - file abstraction layer */
#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <stdio.h>
#include <wchar.h> /* for wchar_t */

#if _MSC_VER > 1300
# include "platform.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
typedef wchar_t file_tchar;
# define SYS_PATH_SEPARATOR '\\'
# define IS_PATH_SEPARATOR(c) ((c) == '\\' || (c) == '/')
# define IS_PATH_SEPARATOR_W(c) ((c) == L'\\' || (c) == L'/')
#else
typedef  char file_tchar;
# define SYS_PATH_SEPARATOR '/'
# define IS_PATH_SEPARATOR(c) ((c) == '/')
#endif /* _WIN32 */
typedef file_tchar* tpath_t;
typedef const file_tchar* ctpath_t;

/* Generic path functions */
const char* get_basename(const char* path);
char* get_dirname(const char* path);
char* make_path(const char* dir, const char* filename);
int are_paths_equal(ctpath_t a, ctpath_t b);

int is_regular_file(const char* path); /* shall be deprecated */
int if_file_exists(const char* path);

/**
 * Portable file information.
 */
typedef struct file_t
{
	char* path;
#ifdef _WIN32
	wchar_t* wpath;
#endif
	uint64_t size;
	uint64_t mtime;
	unsigned mode;
} file_t;

#ifdef _WIN32
# define FILE_TPATH(file) ((file)->wpath)
#else
# define FILE_TPATH(file) ((file)->path)
#endif

/* bit constants for the file_t.mode bit mask */
#define FILE_IFDIR   0x01
#define FILE_IFLNK   0x02
#define FILE_IFREG   0x04
#define FILE_IFROOT  0x10
#define FILE_IFSTDIN 0x20
#define FILE_IFLIST  0x40
#define FILE_OPT_DONT_FREE_PATH  0x200
#define FILE_OPT_DONT_FREE_WPATH 0x400

#define FILE_ISDIR(file) ((file)->mode & FILE_IFDIR)
#define FILE_ISLNK(file) ((file)->mode & FILE_IFLNK)
#define FILE_ISREG(file) ((file)->mode & FILE_IFREG)

/* file functions */
void file_init(file_t* file, const char* path, int finit_flags);
void file_cleanup(file_t* file);
void file_path_append(file_t* dst, file_t* src, const char* suffix);

enum FileStatModes {
	FNoMode    = 0,
	FUseLstat  = 1
};
int file_stat(file_t* file, int fstat_flags);

enum FileFOpenModes {
	FOpenRead  = 1,
	FOpenWrite = 2,
	FOpenRW    = 3,
	FOpenBin   = 4,
	FOpenMask  = 7
};
FILE* file_fopen(file_t* file, int fopen_flags);
int file_rename(file_t* from, file_t* to);

#ifdef _WIN32
FILE* rsh_tfopen(ctpath_t tpath, file_tchar* tmode);
int file_is_write_locked(file_t* file);
#else
# define file_is_write_locked(f) (0)
# define rsh_tfopen(tpath, tmode) fopen(tpath, tmode)
#endif

typedef struct file_list_t {
	FILE* fd;
	file_t current_file;
	unsigned state;
} file_list_t;
int  file_list_open(file_list_t* list, file_t* file_path);
int  file_list_read(file_list_t* list);
void file_list_close(file_list_t* list);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FILE_H */

#ifndef _KIOKU_FILESYSTEM_H
#define _KIOKU_FILESYSTEM_H

#include "kioku/types.h"
#include "kioku/log.h"

#define KIOKU_DIRSEP "/"

/** Find the first and last chars to use in path for trimming redundant chars on each end.
 * Up to one leading slash may remain.
 * No trailing slashes past a first leading slash may remain.
 * \param[in] path Path to find trim points on.
 * \param[out] start Place to store the new start of the path string.
 * \param[out] end Place to store the new end of the path string.
 */
void kioku_path_trimpoints(const char *path, uint32_t *start, uint32_t *end);

/** Concatenate two paths, storing the result in dest.
 * \param[out] dest Place to store the resulting path.
 * \param[in] destsize Number of bytes available for storage in dest.
 * \param[in] path1 Left side of the path to store.
 * \param[in] path2 Right side of the path to store.
 * \return Number of bytes placed in dest, excluding the NULL byte ('\0') at the end. -1 is returned if any of the pointers are NULL, or if there isn't enough room to store the concatenated path. In such cases the contents of dest are unspecified.
 */
int32_t kioku_path_concat(char *dest, size_t destsize, const char *path1, const char *path2);

/** The concept of a user home directory isn't really a cross-platform concept.
 * To make this portable, we will define a method that determines a Kioku home directory.
 * Kioku needs a safe place where it has permission to read/write and store plugins/configurations.
 * The user can change where users are stored, but the file that lists those locations needs a home.
 * Useful Info:
 * - http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
 * - https://stackoverflow.com/a/3733955
 * - https://stackoverflow.com/a/933996
 * - https://github.com/ehsan/ogre/tree/master/Samples/Browser/src
 * - https://github.com/seltzy/orx/blob/master/code/src/io/orxFile.c
 * \return Path to a consistent directory where Kioku can safely do its work undisturbed.
 */
const char *kioku_get_home_directory();

/** Get the path to the directory where a Kioku user's collection is stored.
 * \param[in] username The keyname of the user to get the path for.
 * \return The stored path to their directory, which may or may not exist.
 */
const char *kioku_get_user_directory(const char *username);

/** Create an empty file along with leading parent directories if necessary.
 * Because git doesn't see directories until there are files in them, why should the filesystem module?
 * \param[in] path Path to the file to create.
 * \return Whether the path and preceding directories could be created.
 */
bool kioku_filesystem_create(const char *path);

/** Rename a file or directory
 * \param[in] path Path to the file/dir to rename.
 * \return Whether the file/dir was renamed.
 */
bool kioku_filesystem_rename(const char *path, const char *newpath);

/** Delete a file or directory. Directories will be deleted recursively.
 * \param[in] path Path to the file/dir to delete.
 * \return Whether the file/dir was deleted.
 */
bool kioku_filesystem_delete(const char *path);

/** Check if a file/dir exists.
 * \param[in] path Path to the file/dir to check.
 * \return Whether the file/dir exists.
 */
bool kioku_filesystem_exists(const char *path);

#endif /* _KIOKU_FILESYSTEM_H */

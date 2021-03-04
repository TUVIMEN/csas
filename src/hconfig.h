/*
    csas - console file manager
    Copyright (C) 2020-2021 TUVIMEN <suchora.dominik7@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#define __THREADS_ENABLE__
#ifdef __THREADS_ENABLE__
    #define __THREADS_FOR_DIR_ENABLE__
    //#define __THREADS_FOR_FILE_ENABLE__
#endif

#define __COLOR_FILES_BY_EXTENSION__
//#define __HIDE_FILES__
#define __FOLLOW_PARENT_DIR__
#define __USER_NAME_ENABLE__
#define __LOAD_CONFIG_ENABLE__
#define __INODE_ENABLE__
//#define __SHOW_KEY_BINDINGS__

#define __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
#ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
    #ifdef __INODE_ENABLE__
        #define __FAST_RESCUE__
        #ifdef __FAST_RESCUE__
            //#define __CHECK_IF_FILE_HAS_THE_SAME_NAME__
        #endif
    #endif
#endif

//#define __FILE_GROUPS_ENABLE__
//#define __FILE_OWNERS_ENABLE__
//#define __BLOCKS_ENABLE__
#define __MODE_ENABLE__
//#define __BLK_SIZE_ENABLE__
//#define __DEV_ENABLE__
#define __NLINK_ENABLE__
//#define __RDEV_ENABLE__
#define __MTIME_ENABLE__
//#ifdef __MTIME_ENABLE__ //!
//#define __SAVE_PREVIEW__
//#endif
//#define __ATIME_ENABLE__
//#define __CTIME_ENABLE__

#define __FILE_SIZE_ENABLE__

#define __SORT_ELEMENTS_ENABLE__
#define __FILESYSTEM_INFO_ENABLE__
#define WORKSPACE_N 10

#define __THREADS_ENABLE__
#ifdef __THREADS_ENABLE__
    #define __THREADS_FOR_DIR_ENABLE__
    #define __THREADS_FOR_FILE_ENABLE__
#endif

#define __COLOR_FILES_BY_EXTENSION__
//#define __SHOW_HIDDEN_FILES_ENABLE__
#define __INOTIFY_ENABLE__
#define __USER_NAME_ENABLE__
#define __LOAD_CONFIG_ENABLE__
#define __INODE_ENABLE__

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
//#define __NLINK_ENABLE__
//#define __RDEV_ENABLE__
#define __MTIME_ENABLE__
//#define __ATIME_ENABLE__
//#define __CTIME_ENABLE__

#define __FILE_SIZE_ENABLE__
#ifdef __FILE_SIZE_ENABLE__
    #define __HUMAN_READABLE_SIZE_ENABLE__
    #define __GET_DIR_SIZE_ENABLE__
#endif

#define __SORT_ELEMENTS_ENABLE__
#define __FILESYSTEM_INFORMATION_ENABLE__
#define WORKSPACE_N 10

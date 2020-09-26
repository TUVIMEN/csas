#define __LOAD_CONFIG_ENABLE__

//#define __FILE_GROUPS_ENABLE__

//#define __FILE_OWNERS_ENABLE__

#define __COLOR_FILES_BY_EXTENSION__

//#define __SHOW_HIDDEN_FILES_ENABLE__

#define __FILE_SIZE_ENABLE__
#ifdef __FILE_SIZE_ENABLE__
    #define __BLOCK_SIZE_ELEMENTS_ENABLE__
    #define __HUMAN_READABLE_SIZE_ENABLE__
    #define __GET_DIR_SIZE_ENABLE__ // new despair
#endif

#define __SORT_ELEMENTS_ENABLE__
#ifdef __SORT_ELEMENTS_ENABLE__
    #define __MTIME_ENABLE__
    //#define __ATIME_ENABLE__
    //#define __CTIME_ENABLE__
#endif

#define __FILESYSTEM_INFORMATION_ENABLE__

#define WORKSPACE_N 10

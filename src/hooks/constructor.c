#define _GNU_SOURCE

#include "exec_utils.h"


/* Update working directory */
__attribute__((constructor)) static void apprun_update_working_directory() {
    apprun_restore_workdir_if_needed();
}



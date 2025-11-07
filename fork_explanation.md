## 3 Part 2.3: What fork Does (Student 1)

**One-line idea:** `fork` creates a **new process** (the **child**) that is a near-identical **copy** of the calling process (the **parent**), with a few exceptions.

### Key points

  * **Process Creation:** Creates a new, separate process. Both processes execute the **same program** from the point immediately following the `fork()` call.
  * **PID/PPID:** The **child process** is assigned a **new, unique PID**. The child's **Parent Process ID (PPID)** is set to the parent's PID.
  * **Address Space:** The child receives a **copy** of the parent's entire address space (code, data, heap, stack). This is typically implemented using **Copy-on-Write (COW)** to save memory until one process writes to a page.
  * **Return Value:** `fork()` returns **three** possible values:
      * **`0`** in the **child** process.
      * **The child's PID** in the **parent** process.
      * **`-1`** on **failure** in the parent process.
  * **Open Files:** Both processes share the **same open file descriptors**; they reference the same underlying kernel file structures and file offsets.
  * **Signals:** The child **inherits** signal dispositions (handlers) from the parent. Pending signals are **not** inherited.

### Idiom (illustrative):

The idiom for distinguishing parent and child, and for the parent to wait for the child:

```c
pid_t pid = fork();

if (pid < 0) {
    // Error handling
    perror("fork failed");
} else if (pid == 0) { 
    // This code only runs in the CHILD process
    printf("I am the child. My PID is %d\n", getpid());
    exit(0); // Child finishes its work
} else { 
    // This code only runs in the PARENT process
    int status;
    printf("I am the parent. My child's PID is %d\n", pid);
    waitpid(pid, &status, 0); // Parent waits for the child to exit
    printf("Child exited with status %d\n", WEXITSTATUS(status));
}
```

### Conclusion: 
The fork() system call is used in Unix-based systems to create a new process by duplicating the calling process. When a program calls fork(), the operating system makes an almost exact copy of the parent process, assigning it a new process ID and independent memory space. After the call, both the parent and child processes continue execution from the same point in the code, but fork() returns different values to each: zero to the child and the child’s PID to the parent. This difference allows the program to distinguish between them and execute separate code paths if needed. In essence, fork90 enables true parallel execution by letting one program split into two running processes that cooperate independently.
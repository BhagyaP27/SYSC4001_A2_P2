## 2 Part 2.2: What exec Does (Student 1)

**One-line idea:** `exec` replaces the current **process image** with a new program (code, data, stack), keeping the same **PID**; on success it does not return.

### Key points

  * **Replacement:** The caller's **address space** is discarded and filled with the executable's text/data/heap/stack.
  * **PID unchanged:** Same **process ID**; only the program image changes.
  * **Args/env:** You supply `argv` (and optionally `envp`); use variants like `execve`, `execlp`, `execvp`.
  * **Open files:** **File descriptors** remain open across `exec` unless marked **`FD_CLOEXEC`**.
  * **Signals:** Custom handlers are reset to defaults per OS rules (ignored signals may remain ignored).
  * **Return:** On success, **no return**; on failure, returns **`-1`** and sets `errno`.
  * **Typical use with fork:** Parent `fork()`; child calls `exec(...)` to run another program; parent `wait()`.

### Idiom (illustrative):

```c
pid_t pid = fork();
if (pid == 0) { // child
    char *argv[] = {"program2", "arg1", NULL};
    execvp("program2", argv); // replace child image
    perror("execvp"); exit(127);
} else if (pid > 0) {
    int status; waitpid(pid, &status, 0);
}
```

### Conclusion: 
## What does the $\text{exec}$ function call do? 💻

family of functions is fundamentally responsible for replacing the entire process image of the calling process with a newly loaded program.  It takes the specified program file and loads it directly into the current process's memory space, overwriting the existing code, data, and stack, and then starts execution from the new program's entry point (e.g., main). Crucially, the process's identity, specifically its Process ID (PID), does not change; it's the *content* of the running process that is transformed. Since the calling program is entirely replaced, a successful exec call never returns to the original function call point, as that code no longer exists in memory. This function is typically executed by a child process immediately after a successful fork() call to enable the child to run a different application than its parent.


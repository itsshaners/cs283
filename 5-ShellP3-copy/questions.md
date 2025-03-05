1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

I call waitpid() on each child process to make sure that they are all complete before continuing. This is done in a loop after all child processes have been forked and executed. If I forgot to call waitpid() on all child processes, the shell won't wait for it to complete and may lead to issues. The shell may start accepting new input while the previous commands are still running, or the child process would become a zombie process after terminations.  

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

It is necessary to close unused pipes after calling dup2() in order to prevent descriptor leaks and to ensure the pipes function properly. When dup2() is used to duplicate a file descriptor, the original file descriptor is still open. If pipes are left open your risk using up available file descriptors, or potentially block other behavior. 

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd command is implemented as a built-in command because it needs to be able to change the current working directory of the shell process itself. If cd was implemented as an external process, the external process would change its own working directory, but not affect the parent shell process. The shell's current working directory would stay the same and make the cd command ineffective. You would then have to implement inter-process communication so the shell would know when to change directories. 

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

If we wanted to allow an arbitrary number of piped commands we would need to use dynamic memory allocation. Doing this would increase the complexity of implementation and potentially affect performance. 

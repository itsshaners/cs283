1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  Using fork lets the parent process keep running while the child process executes the new program.  This is allows us to create new processes without terminating the original one, and crucial for having tasks run concurrently.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the system call fails, it returns a value of -1, and no child process in created. 

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() searches by looking though the directories listed in the PATH environment variable. the PATH variable contains a list of directories where executable files are located. 

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The purpose it to wait for the child process to complete and get its exit status. If we didn't it the program wold continue to run without the necessary data and cause issues.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  It gives us the exit status of the child process. Its important because it lets us know if the child process terminated successfully or not.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  I had trouble passing these tests so I cant speak to the implementation, but its necessary to correctly parse the command lines with arguments that include spaces. 

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  We needed to change our parsing logic to handle quotes arguments and get rid of the double spaces. While I think I did it successfully I had trouble passing the tests. Even looking at the debugging output, the extracted looked identical to the expected. 

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  In a Linux system, signals are used to notify processes of event or conditions that require attention. They allows processes to handle things like termination requests and segmentation faults. Unlike other forms IPCs, signals are lightweight and can be delivered to a process at any time. 

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  1. SIGKILL (Signal  9): is used to forcefully terminate a process. Typically used to terminate unresponsive or misbehaving processes
                   2. SIGTERM (Signal 15): is used to request a process to terminate gracefully. Typically used to allow a process to release resources and save state.
                   3. SIGINT  (Signal  2): is used to interrupt a process when user presses Ctrl+C. Typically used to interrupt and terminate a process manually

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  The process is immediately stopped by the operating system. I cannot be caught, blocked, or ignored like SIGINT. This is to ensure that the processed gets paused, to allow the operating system or debugger to control it execution. The process can later be resumed using the SIGCONT signal. 
 
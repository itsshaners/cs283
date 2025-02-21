1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  It is a good choice because it reads a line of text from stdin and stores it into the provided buffer. It's safe to use since it can prevent buffer overflow. It can also handle EOF and newline characters well.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  This allows us to be more flexible and dynamic with memory allocation as opposed to allocating a fixed-size array. With this approach, we are able to make adjustments to the buffer size at run-time based on requirements, but it more complex.

3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  If spaces are not trimmed, the sell might misinterpret the command or its argument and cuase errors on unexpected behavior. For example, a command with leading spaces might not be recognized as a valid command, while one with trailing spaces might have one interpreted as an argument. 

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  1. Output Redirection(> and >>):
                        - Example: ls > output.txt
                        - The command redirects the STDOUT of LS to output.txt, overwriting the file if it exists.
                        - The challenge is we than have to handle file opening, closing, and ensuring the output is successfully written 

                    2. Input Redirection(<):
                        - Example: sort < input.txt
                        - The command redirects the STDIN of sort to read from input.txt instead of the keyboard
                        - The challenge is we than have to handle file opening, closing, and ensuring that the input is correctly read 
                    3. Error Redirection(2> and 2>>):
                        - Example: grep "pattern" file.txt 2> error.log
                        - The command and redirects the STDERR of grep to error.log, overwriting the file if it exists
                        - The challenge is we than have to handle file opening, closing, and ensuring that error messages are correctly written 
                        

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection involves changing the source or destination of standard input, output, or error stream to or from files. While piping involves connecting the output of one command directly to th input of another.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  It is important to keep them separated because it allows users and scripts to distinguish between regular output and error messages. For example, the separation allows us to redirect error messages to a different location without affecting the regular output. 

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Yes we should provide the option to merge both STDERR and STDOUT. We can do that by using 2>&1. For example output.log 2>&1 would redirect both STDOUT and STDERR to output.log
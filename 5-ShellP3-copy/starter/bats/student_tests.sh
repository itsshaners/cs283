#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

# Define error codes
ERR_TOO_MANY_COMMANDS=-2

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

setup() {
    if [[ ! -x ./dsh ]]; then
        skip "dsh binary not found or not executable"
    fi
}

@test "Basic command execution: ls" {
    run ./dsh <<EOF
ls
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"dsh_cli.c"* ]]
}

@test "Basic command execution: echo" {
    run ./dsh <<EOF
echo Hello, World!
exit
EOF
    [ "$status" -eq 0 ]
    cleaned_output=$(echo "$output" | tr -d '[:space:]')
    [[ "$cleaned_output" == *"Hello,World!"* ]]
}

@test "Piped execution: ls | grep .c" {
    run ./dsh <<EOF
ls | grep .c
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"dshlib.c"* ]]
}

@test "Error handling: Too many piped commands" {
    run ./dsh <<EOF
echo 1 | echo 2 | echo 3 | echo 4 | echo 5 | echo 6 | echo 7 | echo 8 | echo 9
exit
EOF
    [ "$status" -eq $ERR_TOO_MANY_COMMANDS ]
    [[ "$output" == *"dsh3> error: piping limited to 8 commands"* ]]
}

@test "Built-in command: exit" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"dsh3> exiting..."* ]]
}

@test "Built-in command: cd" {
    run ./dsh <<EOF
cd /
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"/"* ]]
}

@test "Built-in command: echo $?" {
    run ./dsh <<EOF
echo $?
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"0"* ]]
}

@test "Redirection: cat from file" {
    run ./dsh <<EOF
cat < out.txt
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello, World!"* ]]
}

@test "Redirection: output to file" {
    run ./dsh <<EOF
echo This is a new line > out.txt
exit
EOF
    [ "$status" -eq 0 ]
    run cat out.txt
    [ "$status" -eq 0 ]
    [[ "$output" == *"This is a new line"* ]]
}

@test "Redirection: append to file" {
    run ./dsh <<EOF
echo This is a new line >> out.txt
exit
EOF
    [ "$status" -eq 0 ]
    run cat out.txt
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello, World!"* ]]
    [[ "$output" == *"This is a new line"* ]]
}

@test "Error handling: invalid command" {
    run ./dsh <<EOF
invalidcommand
exit
EOF
    [ "$status" -ne 0 ]
    [[ "$output" == *"Command not found: invalidcommand"* ]]
}
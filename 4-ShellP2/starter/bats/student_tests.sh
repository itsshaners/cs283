#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "cd command" {
    run ./dsh <<EOF
cd /
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [ "${lines[1]}" = "/" ]
}

@test "cd command with no arguments" {
    run ./dsh <<EOF
cd
pwd
exit
EOF
    [ "$status" -eq 0 ]
}

@test "ls command" {
    run ./dsh <<EOF
ls
exit
EOF
    [ "$status" -eq 0 ]
    [ "${lines[1]}" != "" ]
}

@test "ls -l command" {
    run ./dsh <<EOF
ls -l
exit
EOF
    [ "$status" -eq 0 ]
    [ "${lines[1]}" != "" ]
}

@test "echo command with quoted arguments" {
    run ./dsh <<EOF
echo "hello world"
exit
EOF
    [ "$status" -eq 0 ]
    [ "${lines[1]}" = "hello world" ]
}

@test "invalid command" {
    run ./dsh <<EOF
invalid_command
exit
EOF
    [ "$status" -eq 127 ]
}

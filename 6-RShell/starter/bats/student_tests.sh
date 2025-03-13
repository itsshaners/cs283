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

@test "Check piped commands run without errors" {
    run ./dsh <<EOF
ls | grep dsh
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" =~ "dsh" ]]
}

@test "Check built-in command cd works" {
    run ./dsh <<EOF
cd ..
pwd
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/Users/iainshand/Documents/Drexel/25-Winter/CS283/cs283" ]]
}

@test "Check remote execution of commands" {
    run ./dsh -c -i 127.0.0.1 -p 1234 <<EOF
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}


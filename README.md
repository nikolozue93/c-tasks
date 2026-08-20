# Task #1

Write a Bash script that finds all files matching the `*.log` mask in the working directory and its subdirectories and moves them into a directory called `logs` located in the working directory.

If the `logs` directory does not exist, the script must create it.

### Example

Before running the script:

```text
/home/project
├── application.log
├── README.md
└── test
    ├── test.log
    └── test.sh
```

After running the script:

```text
/home/project
├── README.md
├── logs
│   ├── application.log
│   └── test.log
└── test
    └── test.sh
```

Assume that all `.log` filenames are unique.


# Task #2

The program should accept one or more program names as command-line arguments. For each program name, it should search for an executable file with that name in the directories listed in the `PATH` environment variable.

The directories in `PATH` are separated by the `:` character.

For each argument:

* Search the directories in `PATH` in order.
* Print the full path of the **first executable file** found.
* If no executable file is found, print:

```text
<program> not found
```

The program should exit with:

* `0` if all requested programs are found.
* `1` if at least one program is not found.

Use the following library functions:

* `getenv()`
* `access()` or `stat()`

### Example

Running:

```bash
./mywhich ls cat foobar
```

may produce:

```text
/usr/bin/ls
/usr/bin/cat
foobar not found
```

Since `foobar` was not found, the program should exit with status `1`.

# Task #3

Write a C program that reads commands from a file named `commands.txt`.

Each line of the file contains one command. The program must execute all commands as a pipeline, so that the standard output of each command becomes the standard input of the next command.

For example, if `commands.txt` contains:

```text
cat input.txt
grep error
sort
wc -l
```

the program should behave like the following shell command:

```bash
cat input.txt | grep error | sort | wc -l
```

The standard output of the last command should be written to the program's standard output.

If any command cannot be executed, the program must report which command failed.

For example:

```text
Command "grep error" failed
```

## Example

`commands.txt`:

```text
cat names.txt
sort
uniq
```

Equivalent shell command:

```bash
cat names.txt | sort | uniq
```

Possible output:

```text
Alice
Bob
John
```

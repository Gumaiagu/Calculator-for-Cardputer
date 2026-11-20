# Calculator for M5 Cardputer

A basic calculator made for the M5Cardputer.

## Build

To compile this project, you have to install the [board for M5Cardputer](https://docs.m5stack.com/en/arduino/arduino_board) and the [library](https://docs.m5stack.com/en/arduino/arduino_library).

### Using Arduino IDE

If you want to run the project directly, configure the port and the board, then click the "Upload" button.

If you want get the binary, you can click in Sketch -> "Export compiled binary", then Sketch -> "Show Sketch Folder" to get the binary of the project.

### Using arduino-cli

If you want to run the project directly, run the following command:

```bash
arduino-cli upload Calculator.ino -p /dev/ttyACM0   # Choose the correct port
```

To get the project's binary, use the command:

```bash
arduino-cli compile . --output-dir ./build
```

## Usage

There's a buffer in the middle of the screen. It can hold an indeterminate number of values stored as doubles (decimal values). All operations are done with the values stored in the buffer.

There's a input field where you can type a number or an operation, if you type neither, there will be an error.

> Errors are just visual hints telling the user what went wrong. The program won't stop because you misspelled something.

Here are the available operations:

| Operation | What does                                 |
|-----------|-------------------------------------------|
|     d     | Deletes last buffer                       |
|    +/-    | Inverts the value of last buffer          |
|     +     | Adds last two buffer                      |
|     -     | Subtracts last two buffer                 |
|     *     | Multiply last two buffers                 |
|     /     | Divide last two buffer                    |
|  ** or ^  | The buffer 2 to the power of the buffer 1 |
|    sqrt   | Gets the square root of the last buffer   |
|     !     | Gets the factorial of last buffer         |
|    cos    | Gets the cossine of last buffer           |
|    sin    | Gets the sine of last buffer              |
|    tan    | Gets the tangent of last buffer           |

All operations delete the used buffers and create a new buffer with the result.

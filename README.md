# Circuit Logic Simulator

## Project Overview

This project simulates the behavior of digital circuits using various logic gates. It reads a circuit description from an input file, simulates the circuit's behavior for all possible input combinations, and outputs the resulting truth table.

### Features:
- Simulate standard logic gates: **NOT**, **AND**, **OR**, **XOR**, **NAND**, **NOR**, **DECODER**, and **MULTIPLEXER**.
- Generate a truth table for the given circuit.
- Handle user-defined inputs, outputs, and gates dynamically from the input file.

## Technologies Used:
- **C** for implementation of the logic simulator.
- **Standard I/O** for reading input files and displaying the results.
- **Memory management** with dynamic memory allocation (malloc and free).

## Files and Structure:
- **Main C File**: The program reads a circuit description from an input file, processes the gates, and simulates the circuit behavior.
- **Input File**: A text file that describes the circuit, including variables, inputs, outputs, and gates.

## Input File Format:

The input file should follow this structure:

1. **INPUT N**: Defines `N` input variables.
2. **OUTPUT M**: Defines `M` output variables.
3. **Gates**: Each gate is described by its type followed by its input, selector (if applicable), and output variables.

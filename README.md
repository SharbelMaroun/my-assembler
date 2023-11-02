# Final Project in System Programming Laboratory course
## By Sharbel Maroun & Eddi Vilinets
### Date: 16.08.2023

Project Description:

Our project focuses on developing an assembler for a fictional assembly programming language used in The Open University of Israel's System Programming Lab course final project. Our approach emphasizes simplicity, memory efficiency, and practicality.

**Efficient Memory Usage:**
We prioritize efficient memory usage. Our assembler allocates memory for the binary image in a way that minimizes memory consumption.

**Modular and Simple Structure:**
We've organized the program into separate parts, each responsible for specific tasks. We've made sure that these parts can communicate smoothly, using techniques like pointers and structs to their relevant files. This helps keep our code straightforward and easy to work with.

**Simplified Data Structures, including Macros:**
To make our code more understandable and effective, we've used simplified data structures like typedefs, structs, and constants, occasionally encapsulated within macros.

**Overview of How It Works:**

The assembler operates in several stages:

**Macros Stage:**
Here, the assembler processes the original source file (.as) and generates an expanded source file (.am). The expanded file contains the same source code, but any macros from the original source are replaced with their content. If there are any errors in the macros' syntax, relevant error messages are displayed, and the program will be terminated. If not, the assembler moves to the first pass stage.

**First Run:**
In the first run, the assembler calculates the sizes of the directive and instruction images with allocating memory and creating the memory image as much as possible. It also stores all symbols in the symbol table and checks for code errors. If any errors are found, they are reported as error/warning messages. Error reporting is comprehensive, as the assembler scans the entire source code for any issues. Errors (excluding warnings) prevent the assembler from proceeding to the next stage.

**Second Run:**
In the second run, the assembler continues to convert code lines into binary format, inserts binary values into the memory image at the respective bits, and keeps track of external operands in a table. If the assembler encounters a label operand not present in the symbol table and not marked as external or finds label conflicts, it generates an error message. This stage exclusively deals with this specific error type, as other errors have already been addressed in previous stages. In the presence of such an error, the assembler completes the second run without exporting any files.

**Export (Generate Files):**
If the second run concludes without errors, the assembler proceeds to generate the required output files (.ob, .ext, .ent).

The assembler repeats these steps for each source file provided, continuing until the last file is processed, at which point the program finishes its execution.

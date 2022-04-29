# nand2tetris

This repository is for the study of "THE ELEMENTS OF COMPUTING SYSTEM".

I follow the text to implementation code.

I implementation S/W, which is assembler, VM and Compiler, by C++ Language.


**assembler**

- `Parser` - Parse Hack assembly code, make SymbolTable by `SymbolTable` Class.
- `SymbolTable` - Register Symbol and assign Symbol Address. This class has hash table(by using std::map) for making Symbol to address table.
- `Code` - Write Hack binary Code.
- `HackFileIO` - std::ofstream rapper class.
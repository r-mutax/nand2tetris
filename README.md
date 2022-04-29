# nand2tetris

This repository is for the study of "THE ELEMENTS OF COMPUTING SYSTEM".  
I follow the text to implementation code.  
I implementation S/W, which is assembler, VM and Compiler, by C++ Language.  

**assembler**  
Convert Hack assembly to Hack binary code.
- `Parser` - Parse Hack assembly code, make SymbolTable by `SymbolTable` Class.
- `SymbolTable` - Register Symbol and assign Symbol Address. This class has hash table(by using std::map) for making Symbol to address table.
- `Code` - Write Hack binary Code.
- `HackFileIO` - std::ofstream rapper class.

**VM**  
Convert VM language to Hack assembly code.
- `Parser` - Parsing VM Code.
- `CodeWriter` - Write Hack assembly code from VM Code.

**JackCompiler**  
Compiling Jack language to VM code.
- `JackAnalyzer` - It launches CompilationEngine.
- `CompilationEngine` - It works as central role of compiling Jack language.Parsing Jack language and make ATS.
- `Jacktokenizer` - Tokenize Jack language.
- `OutputProgram` - Print ATS to XML format. This class is used for debugging.
- `Symboltable` - Register Symbol and assign Symbol Index. It has two hash table - for class scope and function scope.
- `VMWriter` - Write VM Code, from ATS which is parcing from Jack Language.
- `GenXMLFile` - It is utility class for write XML format.
- `utility_lib` - This is utility common function library.
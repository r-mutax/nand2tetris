// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

// R0 : loop cnt
// R4 : address screen
// R5 : pixel value

    @32767
    D=A
    D=D+A
    @1
    D=D+A
    @R6
    M=D

(LOOP)
    // =========================
    // stage 1 : keyboard check
    @KBD
    D=M
    @NOTKEY
    D;JEQ
    // keyon
    @R6
    D=M
    @R5
    M=D
    @ENDKEYCHECK
    0;JMP
(NOTKEY)
    // keyoff
    @0
    D=A
    @R5
    M=D
(ENDKEYCHECK)
    // =========================
    // stage 2 : screen draw
    @R0
    M=0

(SCREENLOOP)
    // judge screen continue
    @R0
    D=M
    @8192
    D=D-A
    @SCREENLOOPEND
    D;JGE

    // calc screen address
    @SCREEN
    D=A
    @R0
    D=D+M
    @R4
    M=D

    // load value
    @R5
    D=M

    // set screen bit
    @R4
    A=M
    M=D

    @R0
    M=M+1

    @SCREENLOOP
    0;JMP
(SCREENLOOPEND)

    @LOOP
    0;JMP
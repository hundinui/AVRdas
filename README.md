# AVRdas
AVR disassembler

Licenced under GNU GPLv3

Supports Intel hex format as input.

Command line switches:

    -A              -   prefix disassembly by address and opcode in hex
    
    -D [filename]   -   load a definition file and automatically translate IO baddresses to their names (ex. sts 0x00B2, r16 -> sts TCNT2, r16)
    
    -o [filename]   -   output filename, by default uses stdout


Worked fine for everything i threw at it, though not everything is tested 100%. Should rewrite the preprocessor functions and reduce them, since those were written for the (currently gutted) assembler side. Actually should rewrite a lot of the code so instead of having to compare the opcode against the list its just a jump table, why didn't i do this from the start.

Thanks to Paul Stoffregen for his ihex.c, which is used in this project.

/**
  Creates a virtual machine with 400 bytes of memory divided into 100 words
  INPUT - input.txt AKA Program Card (INPUT DEVIcE - Card Reader)
  OUTPUT - output.txt AKA (OUTPUT DEVICE - Line Printer)
  CPU -{ 7 FUNCTIONS,
   400 BYTE MEMORY, 
   4 BYTE GENERAL PURPOSE REGISTER - R,
   4 BYTE INSTRUCTION REGISTER IR - {OPCODE, OPERAND}  - current working instruction
   1 BYTE SYSTEM INTERRUPT - SI - {0,1,2,3} ,
   1 BYTE FLAG REGISTER - {0,1} - COMPARE FLAG - C, 
   1 BYTE INSTRUCTION COUNTER} - points to next instruction
  INSTRUCTION SET - {LRxx, SRxx, BT, CP, H, GDxx, PDxx} - H<GD,PD- master
  
  $A - Start of program
  $D - DATA Passed
  $E - End of program
  With a single 4 byte general purpose register  

  Assumes Instruction set has no errors ()
  Instruction set

  User executable-  work on 1 word - 4 byte 
    LRxx - load value from xxth memory location to Register
    SRxx - store into xxth memory location from Register
    BT - if C= true, jump to xxth memory location - else continue execution
    CP - Compare Register with xxth memory location - set C flag if equal
    H - Halt
    
  Kernal Only- // work on 1 block - 40 bytes - 1 line - 10 words
    GDxx - Read 40 bytes from DTA section of input.txt (Program Card) to buffer - bufffer to xxth memory location(R)
    PDxx - Write 40 bytes to output.txt (Line printer)
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void INIT(); /// while starting a new job
void READ(); // reads 40 bytes from DTA section of input.txt (Program Card)
void WRITE(); // writes 40 bytes to output.txt (Line printer)
void EUP(); // Execute User Program
void LOAD();  // Loads input.txt
void START();  // sets IC to 0 and Starts program execution
void TERMINATE(); // writes two blank lines to output.txt (Line printer) and LOADS next program

struct Machine {
  char mem[100][4]; // 1 job is of 100 words
  char IR[4]; // current instruction that is being executed
  char C; 
  int IC; // stores address to next instruction to be executed
  char R[4]; // GPR of 4 bytes
  int SI;
} M;

FILE* ProgramCard;
FILE* LinePrinter;


// Copies from B to A 4 bytes
void assign(char* A, const char* B) {
  memcpy(A, B, 4);
}

// Get oprand two bytes form 
int num(char IR[4]) {
  return (IR[2]-'0')*10+(IR[3]-'0');
}// returns mem loc as integer (-'0' to convert char to int)

// Debug function to print memory
void memdump() {
  // Debug dump the program section of memory
  for(int i=0; i<100; i++) {
    printf("%d\t", i+1);
    for(int j=0; j<4; j++) {
      printf("%c", M.mem[i][j]);
    }
    printf("\n");
  }
}

/**
  Module definations
*/
void LOAD() {
  // Loads input.txt
  char Buffer[40];

  int load_program =0;
  int rp =0; // Read pointer - signifies line/block number
  while(fgets(Buffer, 41, ProgramCard)) { // \n is included in the buffer
    if(Buffer[0] == '$') {
       if(Buffer[1] == 'A'){
         INIT();
         load_program=1;
       }
       if(Buffer[1] == 'D') {
         START();
         break;
       };
       if(Buffer[1] == 'E') LOAD();
       continue;
    }  

    if(load_program) {
      // Load the program
      char *r = Buffer;
      int l=0; //  words length - {1,4}
      int i=0; //current word
      char quit = 0;  // flag to quit the loop
      while(1) {
        char *m = M.mem[rp+i];
        switch (*r) {
          case 'G' : l=4;break; // GD
          case 'P' : l=4;break; // PD
          case 'L' : l=4;break; // LR
          case 'S' : l=4;break; // SR
          case 'C' : l=4;break; // CP
          case 'B' : l=4;break; // BT
          case 'H' : l=1;break; // H
          default: quit = 1;
        }
        if(quit) break;

        if(l==1){
          *m = 'H';// load H to memory
          r++;
          // break;
        } else if(l==4 )
        {
            for(int i=0; i<l ;i++, r++) {
              *(m+i) = *r;
            }
        }
        if(i >= 10) break;
        i++;
      }
      rp+= i;// block++ / line++
      // memdump();
    }
  }
}

void READ() {
  // Reads 40 bytes from DTA section of input.txt (Program Card)
  char Buffer[40];
  fgets(Buffer, 41, ProgramCard);
  int mem = num(M.IR);
  int c=0;
  while(Buffer[c] != '\n' && c<40) {
    M.mem[mem+c/4][c%4] = Buffer[c];
    c++;
  }
}

void WRITE() {
  // memdump();
  // Writes 40 bytes to output.txt (Line printer)
  int mem = num(M.IR);
  for(int i=0; i<10; i++) {
    for(int j=0; j<4; j++)
      fputc((M.mem[mem+i][j] == 0)?' ':M.mem[mem+i][j], LinePrinter);
  }
  fputc('\n', LinePrinter);
}


void MOS() {
  if(M.SI == 1) READ();
  if(M.SI == 2) WRITE();
  if(M.SI == 3) TERMINATE(); 

  // EUP();
}

void EUP() {
    // Fetch Intruction
    assign(M.IR, M.mem[M.IC]); // from IC to IR

    // Increment the Instruction counter
    M.IC++; // now it points to next instruction

    // DECODE AND EXECUTE
    if(M.IR[0] == 'L') assign(M.R, M.mem[num(M.IR)]); // from mem to R
    if(M.IR[0] == 'S') assign(M.mem[num(M.IR)], M.R);// from R to mem
    if(M.IR[0] == 'C') M.C = memcmp(M.mem[num(M.IR)], M.R, 4) == 0; 
    if(M.IR[0] == 'B') if(M.C) { M.IC = num(M.IR)-1; M.C = 0; } // -1 because indexing starts from 0

    if(M.IR[0] == 'H'){ M.SI=3; MOS(); return;}
    if(M.IR[0] == 'G'){ M.SI=1; MOS(); }
    if(M.IR[0] == 'P'){ M.SI=2; MOS(); }
    
    EUP();
}

void INIT() {
  // Zero out the entire struct
  memset(&M, 0, sizeof(struct Machine));
}

void START() {
  // Sets IC to 0 and Starts program execution
 M.IC = 0; // because we assumed - evry job's mem loc starts from 0
  EUP();
}

void TERMINATE() {
  // Writes two blank lines to output.txt (Line printer) and LOADS next program
  fputc('\n', LinePrinter);
  fputc('\n', LinePrinter);

  INIT();

  // Loads the next program if any
  LOAD();
}

int main() {
  ProgramCard = fopen("./input.txt", "r");
  LinePrinter = fopen("./output.txt", "w");

  if(ProgramCard == NULL || LinePrinter == NULL) {
    printf("ERROR OPENING input and output files, program exsiting");
    exit(0);
  }
  LOAD();
}
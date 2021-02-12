/* instruction-level simulator for LC3101 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8       /* number of machine registers */
#define MAXLINELENGTH 1000
#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5
#define HALT 6
#define NOOP 7

typedef struct stateStruct
{
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
void performTask(stateType *statePtr);
int getOpcode(int instruction);
void addOp(int instruction, stateType *stateType);
int getBytes(int *instruction, int shiftAmount);
int convertNum(int num);
void nandOp(int instruction, stateType *stateType);
void lwOp(int instruction, stateType *stateType);
void swOp(int instruction, stateType *stateType);
int beqOp(int instruction, stateType *stateType, int pc);
void clearRegisters(stateType *statePtr);
int jalrOp(int instruction, stateType *stateType);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2)
    {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }
    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL)
    {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    clearRegisters(&state);

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++)
    {
        if (sscanf(line, "%d", state.mem + state.numMemory) != 1)
        {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }

        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    performTask(&state);

    return (0);
}

void performTask(stateType *statePtr)
{

    int i;
    int count = 0;

    for (i = 0; i < statePtr->numMemory; i++)
    {
        //incrementing the pc
        statePtr->pc = i;
        count++;
        int opcode = getOpcode(statePtr->mem[i]);

        if (HALT == opcode)
        {
            printState(statePtr);
            statePtr->pc++;
            break;
        }
        else if (ADD == opcode)
        {
            printState(statePtr);
            addOp(statePtr->mem[i], statePtr);
        }
        else if (NAND == opcode)
        {
            printState(statePtr);
            nandOp(statePtr->mem[i], statePtr);
        }
        else if (LW == opcode)
        {
            printState(statePtr);
            lwOp(statePtr->mem[i], statePtr);
        }
        else if (SW == opcode)
        {
            printState(statePtr);
            swOp(statePtr->mem[i], statePtr);
        }
        else if (BEQ == opcode)
        {
            printState(statePtr);
            i = beqOp(statePtr->mem[i], statePtr, i);
        }
        else if (JALR == opcode)
        {
            printState(statePtr);
            i = jalrOp(statePtr->mem[i], statePtr);
        }
        else if (NOOP == opcode)
        {
        }
    }
    clearRegisters(statePtr);
    printf("machine halted \ntotal of %d instructions executed final state of machine:", count);
    printState(statePtr);
}

int getOpcode(int instruction)
{
    int opcode = 0;
    opcode = instruction >> 22;
    
    return opcode;
}

void addOp(int instruction, stateType *stateType)
{
    
    int extractOpcode = instruction - (ADD << 22);
    
    int regA = getBytes(&extractOpcode, 19);
    
    int regB = getBytes(&extractOpcode, 16);
    
    int desgReg = extractOpcode;
    
    stateType->reg[regA] = stateType->reg[desgReg] + stateType->reg[regB];
}

void nandOp(int instruction, stateType *stateType)
{

    int extractOpcode = instruction - (NAND << 22);

    int regA = getBytes(&extractOpcode, 19);

    int regB = getBytes(&extractOpcode, 16);

    int desgReg = extractOpcode;

    stateType->reg[desgReg] = ~(stateType->reg[regA] & stateType->reg[regB]);
}


int jalrOp(int instruction, stateType *stateType)
{

    int extractOpcode = instruction - (JALR << 22);
   
    int regA = getBytes(&extractOpcode, 19);
    

    int regB = getBytes(&extractOpcode, 16);
   
    stateType->reg[regB] = stateType->pc + 1;

    if(regA == regB){
     
     stateType->reg[regA] = stateType->pc;
        
    }
    
    return stateType->reg[regA]-1;
}

void lwOp(int instruction, stateType *stateType)
{
   
    int extractOpcode = instruction - (LW << 22);
  

    int regA = getBytes(&extractOpcode, 19);
    

    int regB = getBytes(&extractOpcode, 16);
    

    int offsetField = convertNum(extractOpcode);

    offsetField = offsetField + stateType->reg[regA];

    int _value = stateType->mem[offsetField];

    stateType->reg[regB] = _value;
}

int beqOp(int instruction, stateType *stateType, int pc)
{
    
    int extractOpcode = instruction - (BEQ << 22);
    
    int regA = getBytes(&extractOpcode, 19);
    
    int regB = getBytes(&extractOpcode, 16);
    
    int offsetField = convertNum(extractOpcode);
   
    if (stateType->reg[regA] == stateType->reg[regB])
    {
        return stateType->pc + offsetField;
    }
    else
    {
        return pc;
    }
}

void swOp(int instruction, stateType *stateType)
{
 
    int extractOpcode = instruction - (SW << 22);
    
    int regA = getBytes(&extractOpcode, 19);
    
    int regB = getBytes(&extractOpcode, 16);
    
    int offsetField = convertNum(extractOpcode);
    
    offsetField = offsetField + stateType->reg[regA];
    
    stateType->mem[offsetField] = stateType->reg[regB];

    if(offsetField >= stateType->numMemory){
        stateType->numMemory = offsetField+1;
    }

}

//utilities
int getBytes(int *instruction, int shiftAmount)
{
    int reg = (*instruction) >> shiftAmount;

    (*instruction) = (*instruction) - (reg << shiftAmount);

    return reg;
}

void printState(stateType *statePtr)
{
    int i;

    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");

    for (i = 0; i < statePtr->numMemory; i++)
    {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }

    printf("\tregisters:\n");

    for (i = 0; i < NUMREGS; i++)
    {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }

    printf("end state\n");
}

void clearRegisters(stateType *statePtr)
{

    int i;
    for (i = 0; i < 8; i++)
    {
        statePtr->reg[i] = 0;
    }
}
int convertNum(int num)
{
    /* convert a 16-bit number into a 32-bit Sun integer */
    if (num & (1 << 15))
    {
        num -= (1 << 16);
    }
    return (num);
}
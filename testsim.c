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
int beqOp(int instruction, stateType *stateType, int pc);

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

    printState(&state);
    performTask(&state);

    return (0);
}

void performTask(stateType *statePtr)
{

    printf("*****testttingggg : performTask******\n");
    int i;

    for (i = 0; i < statePtr->numMemory; i++)
    {
        //incrementing the pc
        
        int opcode = getOpcode(statePtr->mem[i]);

        if (HALT == opcode)
        {
            statePtr->pc = i + 1;
            printf("\nbreak\n");
            printState(statePtr);
            break;
        }
        else if (ADD == opcode)
        {
            statePtr->pc = i + 1;
            addOp(statePtr->mem[i], statePtr);
            printState(statePtr);
        }
        else if (NAND == opcode)
        {
            statePtr->pc = i + 1;
            nandOp(statePtr->mem[i], statePtr);
            printState(statePtr);
        }
        else if (LW == opcode)
        {
            statePtr->pc = i + 1;
            lwOp(statePtr->mem[i], statePtr);
            printState(statePtr);
        }
        else if (SW == opcode)
        {
            statePtr->pc = i + 1;
        }
        else if (BEQ == opcode)
        {
            beqOp(statePtr->mem[i], statePtr, i);
            statePtr->pc = i ;
            printState(statePtr);
        }
        else if (JALR == opcode)
        {
        }
        else if (NOOP == opcode)
        {
        }
    }
}

int getOpcode(int instruction)
{
    int opcode = 0;

    printf("\ninstruction : %d\n", instruction);
    opcode = instruction >> 22;
    printf("Opcode : %d\n", opcode);

    return opcode;
}

void addOp(int instruction, stateType *stateType)
{
    printf("\nADD\n");
    int extractOpcode = instruction - (ADD << 22);
    printf("\nexyracr %d \n", extractOpcode);

    int regA = getBytes(&extractOpcode, 19);
    printf("\nregA %d \n", regA);

    int regB = getBytes(&extractOpcode, 16);
    printf("\nregB %d \n", regB);

    int desgReg = extractOpcode;
    printf("\ndesgReg %d \n", desgReg);

    stateType->reg[regA] = stateType->reg[desgReg] + stateType->reg[regB];
}


void nandOp(int instruction, stateType *stateType)
{
    printf("\nNand %d\n", !(4 && 7));
}

void lwOp(int instruction, stateType *stateType){
printf("\nLW\n");
    int extractOpcode = instruction - (LW << 22);
    printf("\nexyracr %d \n", extractOpcode);

    int regA = getBytes(&extractOpcode, 19);
    printf("\nregA : %d \n", regA);

    int regB = getBytes(&extractOpcode, 16);
    printf("\nregB %d \n", regB);

    int offsetField = convertNum(extractOpcode);;
    printf("\noffsetField %d \n", offsetField);

    offsetField = offsetField + stateType->reg[regA];
    printf("\rega value %d \n", stateType->reg[regA]);

    int _value = stateType->mem[offsetField];

    stateType->reg[regB] = _value;
    
}

int beqOp(int instruction, stateType *stateType, int pc){
    printf("\nBEQ\n");

    int extractOpcode = instruction - (BEQ << 22);
    printf("\nexyracr %d \n", extractOpcode);

    int regA = getBytes(&extractOpcode, 19);
    printf("\nregA : %d \n", regA);

    int regB = getBytes(&extractOpcode, 16);
    printf("\nregB %d \n", regB);

    int offsetField = convertNum(extractOpcode);;
    printf("\noffsetField %d \n", offsetField);

    if(stateType->reg[regA] == stateType->reg[regB]){
        return pc+offsetField;
    } else{
        return pc;
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

int convertNum(int num)
{
    /* convert a 16-bit number into a 32-bit Sun integer */
    if (num & (1 << 15))
    {
        num -= (1 << 16);
    }
    return (num);
}
#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <QString>
#include <QMap>
//QMap set default value 0
QMap<QString,int> opmap={{"lw",35},{"lb",32},{"lbu",36},{"lh",33},{"lhu",37},{"sw",43},{"sb",40},{"sh",41},
                        {"addi",8},{"addiu",9},{"andi",12},{"ori",13},{"xori",14},{"lui",15},{"slti",10},{"sltiu",11},
                        {"beq",4},{"bne",5},{"blez",6},{"bgtz",7},{"bltz",1},{"bgez",1},{"j",2},{"jal",3},
                        {"eret",16},{"mfco",16},{"mtco",16}};
QMap<QString,int> regmap={{"zero",0},{"at",1},{"v0",2},{"v1",3},{"a0",4},{"a1",5},{"a2",6},{"a3",7},
                        {"t0",8},{"t1",9},{"t2",10},{"t3",11},{"t4",12},{"t5",13},{"t6",14},{"t7",15},
                        {"s0",16},{"s1",17},{"s2",18},{"s3",19},{"s4",20},{"s5",21},{"s6",22},{"s7",23},
                        {"t8",24},{"t9",25},{"k0",26},{"k1",27},{"gp",28},{"sp",29},{"fp",30},{"ra",31}};
QMap<QString,int> fcmap={{"add",32},{"addu",33},{"sub",34},{"subu",35},{"slt",42},{"sltu",43},{"and",36},{"or",37},
                        {"xor",38},{"nor",39},{"sll",0},{"srl",2},{"sra",3},{"mult",24},{"multu",25},{"div",26},
                        {"divu",27},{"jalr",9},{"jr",8},{"mfhi",16},{"mflo",18},{"mthi",17},{"mtlo",19},{"break",13},
                        {"syscall",12}};

class Assembler{
public:
    Assembler();
    QString convert(const QString &myasm);
    QString assemble(const QStringList &strlist, int j);
};

#endif // ASSEMBLER_H

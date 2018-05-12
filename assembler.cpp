#include "assembler.h"
#include <QRegExp>
#include <QStringList>
#include <cmath>
#define MAX 1e5

/*进度：
 * break exception
 *
 * 反汇编
 */

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
QMap<QString,int> lmap={};
QList<int> base_A, base_D;
QList<QString> errlist={"ok","[ERROR] The declared base-address is occupied, please check.",
                        "[ERROR] The declared base-address is occupied, please check.",
                       "[ERROR] Unsupported instructions or pseudo-instructions are involved."};
QList<uint> clist;
int ERR;

Assembler::Assembler(){}

QString Assembler::convert(const QString &myasm){
    bool ok=true;
    int i, j, k, pos, pc, tmp_int;
    QRegExp nocom("(((\\/\\/)|#).*\\n)");// discard all after "//" or "#"
    QRegExp rx("(\\b\\w*(\\(\\$)?\\w+\\)?\\b)");// get tuples
    QString tmp_str = myasm, mybin;
    nocom.setMinimal(true);
    tmp_str.replace(nocom," ");// discard all after "//" or "#"
    tmp_str = tmp_str.simplified();
    QStringList alist = tmp_str.split(QRegExp("(:|;) ")), blist, tmp_list;

    for(i=pc=ERR=0; i<alist.size(); i++){ // prepare: label & qseudo & base
        pos = j = 0;
        while ((pos = rx.indexIn(alist.at(i), pos)) != -1) { // read in tuples
            tmp_list << rx.cap(1);
            pos += rx.matchedLength();
            j++;
        }
        prepare(tmp_list, &blist, j, &pc);
        tmp_list.clear();
    }
//    return blist.join("\n");
    if(base_A.size()!=0)
        pc = base_A.at(0);
    for(i=0, clist.clear(); i<blist.size()&&!ERR; i++, pc+=4){ // assembler
        if(base_A.size()*base_D.size()!=0&&pc==base_D.at(0))
            break;
        pos = 0;
        while ((pos = rx.indexIn(blist.at(i), pos)) != -1) { // read in tuples
            tmp_list << rx.cap(1);
            pos += rx.matchedLength();
        }
        clist << assemble(tmp_list, pc);
        tmp_list.clear();
    }

    if(base_D.size()&&!ERR){
        for(; i<blist.size(); i+=4){ // data
            // 可在这里加报错
            for(k=tmp_int=0;k<4;k++){
                tmp_str = blist.at(i+k);
                tmp_int += tmp_str.toInt(&ok,16)*(int)pow(16,6-k*2);
            }
            clist << tmp_int;
            tmp_list.clear();
        }
    }

    for(i=0,pc=base_A.at(0); i<clist.size()&&!ERR; i++, pc+=4){
        mybin += QString("0x%1: %2\n").arg(pc,8,16,QLatin1Char('0')).arg(clist.at(i),32,2,QLatin1Char('0'));
    }

    base_A.clear();
    base_D.clear();
    lmap.clear();
    return mybin;
}
//默认地址输入格式是8位16进制
void Assembler::prepare(const QStringList &strlist, QStringList *blistp,const int &j, int *pc){
    bool ok = true;
    int k;
    QString tmp_str = "";
    QChar tmp_c;
    //QStringList tmp_list;
    if(strlist.at(0)=="BaseAddre"){
        tmp_str = strlist.at(1);
        k = tmp_str.toInt(&ok,16);
        base_A.append(k);
        if(k<base_A.at(0)+*pc){
            ERR = 1;
            return;
        }
        else if(k>base_A.at(0)+*pc)
            for(;*pc+base_A.at(0)<k; *pc+=4)
                *blistp << "nop";
    }
    else if(strlist.at(0)=="DataAddre"){
        tmp_str = strlist.at(1);
        k = tmp_str.toInt(&ok,16);
        base_D.append(k);
        if(k<base_A.at(0)+*pc){
            ERR = 2;
            return;
        }
        else if(k>base_A.at(0)+*pc)
            for(;*pc+base_A.at(0)<k; *pc+=4)
                *blistp << "nop";
    }
    else if(strlist.at(0)=="dd"){ // data
        for(int i=1; i<j; i++){
            tmp_str = strlist.at(i);
            *blistp << tmp_str.mid(2,2);
            *blistp << tmp_str.mid(4,2);
            *blistp << tmp_str.mid(6,2);
            *blistp << tmp_str.mid(8,2);
        }
        *pc += 4;
    }
    else if(strlist.at(0)=="dw"){ // data
        for(int i=1; i<j; i++){
            tmp_str = strlist.at(i);
            if(tmp_str.mid(0,2)=="0x"){
                *blistp << tmp_str.mid(2,2);
                *blistp << tmp_str.mid(4,2);
            }
            else{
                for(k=0; k<tmp_str.size(); k++){
                    tmp_c = tmp_str.at(k);
                    *blistp << QString("%1").arg(tmp_c.toLatin1(),2,16,QLatin1Char('0'));
                }
                if(k%2)
                    *blistp << "0";
            }
        }
        *pc += 2;
    }
    else if(strlist.at(0)=="db"){ // data
        for(int i=1; i<j; i++){
            tmp_str = strlist.at(i);
            if(tmp_str.mid(0,2)=="0x")
                *blistp << tmp_str.mid(2,2);
            else{
                for(int k=0; k<tmp_str.size(); k++){
                    tmp_c = tmp_str.at(k);
                    *blistp << QString("%1").arg(tmp_c.toLatin1(),2,16,QLatin1Char('0'));
                }
            }
        }
        *pc +=1;
    }
    else if(j==1) //label
        lmap[strlist.at(0)] = *pc;
    else if(strlist.at(0)=="la"){
        tmp_str = strlist.at(2).mid(0,4); // high 4-digit
        tmp_str = QString("%1").arg(tmp_str.toInt(&ok, 16)); // transform into decimal
        *blistp << "lui at " + tmp_str;
        tmp_str = strlist.at(2).mid(4,4); // low 4-digit
        tmp_str = QString("%1").arg(tmp_str.toInt(&ok, 16)); // transform into decimal
        *blistp << "ori at " + strlist.at(1) + " " + tmp_str;
        *pc += 8;
    }
    else if(strlist.at(0)=="move"){
        *blistp << "add " + strlist.at(1) + " " + strlist.at(2) + " zero";
        *pc += 4;
    }
    else if(strlist.at(0)=="li"){
        *blistp << "addi " + strlist.at(1) + " zero " << strlist.at(2);
        *pc += 4;
    }
    else{ // regular
        for(int i=0; i<j; i++)
            tmp_str += strlist.at(i) + " ";
        *blistp << tmp_str;
        *pc += 4;
    }
}

uint Assembler::assemble(const QStringList &strlist, int pc){
    bool ok = true;
    int tmp;
    QString tmp_str;
    QRegExp offset("(\\d+)\\(\\$(\\w+)");
    if(opmap[strlist.at(0)]){ // I & J
        switch(opmap[strlist.at(0)]){
        case 1: //bltz
            tmp = (strlist.at(0)=="bltz")?0:1;
            return opmap[strlist.at(0)]*(int)pow(2,26) + // op
                    regmap[strlist.at(1)]*(int)pow(2,21) + // rs
                    tmp*(int)pow(2,16) + //rt
                    strlist.at(2).toInt(&ok, 10); // offset
        case 2: // j
        case 3: // jal
            return opmap[strlist.at(0)]*(int)pow(2,26) + // op
                    lmap[strlist.at(1)]; // address
        case 4: // beq
        case 5: // bne
            tmp = (lmap[strlist.at(3)]>=pc)?(lmap[strlist.at(3)]-pc):(65536+lmap[strlist.at(3)]-pc);
            return opmap[strlist.at(0)]*(int)pow(2,26) + // op
                    regmap[strlist.at(1)]*(int)pow(2,21) + // rs
                    regmap[strlist.at(2)]*(int)pow(2,16) + // rt
                    tmp; // offset
        case 6: // blez
        case 7: // bgtz
            tmp = (lmap[strlist.at(2)]>=pc)?(lmap[strlist.at(2)]-pc):(65536+lmap[strlist.at(2)]-pc);
            return opmap[strlist.at(0)]*(int)pow(2,26) + // op
                    regmap[strlist.at(1)]*(int)pow(2,21) + // rs
                    tmp; // offset
        case 8: // addi
        case 9: // addiu
        case 10: // slti
        case 11: // sltiu
        case 12: // andi
        case 13: // ori
        case 14: // xori
            tmp_str = strlist.at(3);
            return opmap[strlist.at(0)]*(int)pow(2,26) + // op
                    regmap[strlist.at(2)]*(int)pow(2,21) + // rs
                    regmap[strlist.at(1)]*(int)pow(2,16) + // rt
                    tmp_str.toInt(&ok, 10); // imm
        case 15: // lui
            tmp_str = strlist.at(2);
            return opmap[strlist.at(0)]*(int)pow(2,26) + // op
                    regmap[strlist.at(1)]*(int)pow(2,16) + // rt
                    tmp_str.toInt(&ok, 10); // imm
        case 16: // eret mfc0 mtc0
            if(strlist.at(0)=="eret")
                return 1107296280;
            else{
                tmp = (strlist.at(0)=="mfc0")?0:4;
                return opmap[strlist.at(0)]*(int)pow(2,26) + // op
                        tmp*(int)pow(2,21) + // rs
                        regmap[strlist.at(1)]*(int)pow(2,16) + // rt
                        regmap[strlist.at(2)]*(int)pow(2,11);
            }
        case 32: // lb
        case 33: // lh
        case 35: // lw
        case 36: // lbu
        case 37: // lhu
        case 40: // sb
        case 41: // sh
        case 43: // sw
            int pos = offset.indexIn(strlist.at(2));
            if (pos > -1) {
                tmp_str = offset.cap(1); // num
                tmp = tmp_str.toInt(&ok,10);
                tmp_str = offset.cap(2);  // reg
            }
            return opmap[strlist.at(0)]*(int)pow(2,26) + // op
                    regmap[tmp_str]*(int)pow(2,21) + // rs
                    regmap[strlist.at(1)]*(int)pow(2,16) + // rt
                    tmp; // offset
        }
    }
    else{ // R & others: 移动，传输，陷阱
           if(strlist.at(0)=="srl"||strlist.at(0)=="sll"||strlist.at(0)=="sra"){ // 移动
               tmp_str = strlist.at(3);
               return regmap[strlist.at(2)]*(int)pow(2,16) + // rt
                       regmap[strlist.at(1)]*(int)pow(2,11) + // rt
                       tmp_str.toInt(&ok, 10)*(int)pow(2,6) + // shamt
                       fcmap[strlist.at(0)]; // func
           }
           else if(strlist.at(0)=="jalr"||strlist.at(0)=="jr"){ // jalr jr
               tmp_str = (strlist.at(0)=="jalr")?strlist.at(2):"zero";
               return regmap[strlist.at(1)]*(int)pow(2,21) +
                       regmap[tmp_str]*(int)pow(2,11) +
                       fcmap[strlist.at(0)]; // func
           }
           else if(strlist.at(0)=="mfhi"||strlist.at(0)=="mflo"){ // 传输1: mfhi, mflo
               return regmap[strlist.at(1)]*(int)pow(2,16) +
                       fcmap[strlist.at(0)]; // func
           }
           else if(strlist.at(0)=="mthi"||strlist.at(0)=="mtlo"){ // 传输2: mthi, mtlo
               return regmap[strlist.at(1)]*(int)pow(2,21) +
                       fcmap[strlist.at(0)];
           }
           else if(strlist.at(0)=="syscall"||strlist.at(0)=="break"){ // 陷阱 ??break存疑
               return fcmap[strlist.at(0)];
           }
           else if(strlist.at(0)=="nop")
               return 0;
           else if(fcmap[strlist.at(0)]){ // R
               return regmap[strlist.at(1)]*(int)pow(2,21) +
                       regmap[strlist.at(2)]*(int)pow(2,16) +
                       regmap[strlist.at(3)]*(int)pow(2,11) +
                       fcmap[strlist.at(0)]; // func
           }
           else {
               ERR = 3;
               return 0; // error
           }
        }
    return 0;
   }

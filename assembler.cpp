#include "assembler.h"
#include <QRegExp>
#include <QStringList>

/*进度：
当前问题：
 只能输出一行 问题在tmp_str->alist
 R/0op除break解决，还需解决label：先扫一遍，处理伪指令
已解决：
 多行注释\n
*/
Assembler::Assembler(){}

QString Assembler::convert(const QString &myasm){
    int i, j, pos;
    QRegExp nocom("(((\\/\\/)|\\#).*\\n)");// discard all after "//" or "#"
    QRegExp rx("(\\b\\w*(\\(\\$)?\\w+\\)?\\b)");// get tuples
    QString tmp_str = myasm, mybin;
    tmp_str.replace(nocom,"");// discard all after "//" or "#"
    tmp_str = tmp_str.simplified();
    QStringList alist = tmp_str.split("\n"), blist, tmp_list;

    for(i=0; i<alist.size(); i++){
        pos = j = 0;
        while ((pos = rx.indexIn(alist.at(i), pos)) != -1) { // read in tuples
            tmp_list << rx.cap(1);
            pos += rx.matchedLength();
            j++;
        }
        blist << assemble(tmp_list, j);
        tmp_list.clear();
    }
    mybin = blist.join("\n");
    return mybin;
}

QString Assembler::assemble(const QStringList &strlist, int j){
    bool ok;
    QString tmp_str;
    if(opmap[strlist.at(0)]){ // I & J
        ;
    }
    else{ // R & others: 移动，传输，陷阱,label
        if(strlist.at(0)=="srl"||strlist.at(0)=="sll"||strlist.at(0)=="sra"){ // 移动
            tmp_str = strlist.at(3);
            return QString("%1").arg(0,6,2,QLatin1Char('0')) +" "+ // op:0*6
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // rs:0*5
                    QString("%1").arg(regmap[strlist.at(2)],5,2,QLatin1Char('0')) +" "+ // rt
                    QString("%1").arg(regmap[strlist.at(1)],5,2,QLatin1Char('0')) +" "+ // rt
                    QString("%1").arg(tmp_str.toInt(&ok, 10),5,2,QLatin1Char('0')) +" "+ // shamt
                    QString("%1").arg(fcmap[strlist.at(0)],6,2,QLatin1Char('0')); // func
        }
        else if(strlist.at(0)=="jalr"||strlist.at(0)=="jr"){ // jalr jr
            tmp_str = (strlist.at(0)=="jalr")?strlist.at(2):"zero";
            return QString("%1").arg(0,6,2,QLatin1Char('0')) +" "+ // op:0*6
                    QString("%1").arg(regmap[strlist.at(1)],5,2,QLatin1Char('0')) +" "+ // rs:0*5
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // rt:0*5
                    QString("%1").arg(regmap[tmp_str],5,2,QLatin1Char('0')) +" "+ // rd
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // shamt
                    QString("%1").arg(fcmap[strlist.at(0)],6,2,QLatin1Char('0')); // func
        }
        else if(strlist.at(0)=="mfhi"||strlist.at(0)=="mflo"){ // 传输1: mfhi, mflo
            return QString("%1").arg(0,6,2,QLatin1Char('0')) +" "+ // op:0*6
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // rs:0*5
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // rt:0*5
                    QString("%1").arg(regmap[strlist.at(1)],5,2,QLatin1Char('0')) +" "+ // rd
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // shamt
                    QString("%1").arg(fcmap[strlist.at(0)],6,2,QLatin1Char('0')); // func
        }
        else if(strlist.at(0)=="mthi"||strlist.at(0)=="mtlo"){ // 传输2: mthi, mtlo
            return QString("%1").arg(0,6,2,QLatin1Char('0')) +" "+ // op:0*6
                    QString("%1").arg(regmap[strlist.at(1)],5,2,QLatin1Char('0')) +" "+ // rs:
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // rt:0*5
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // rd:0*5
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // shamt
                    QString("%1").arg(fcmap[strlist.at(0)],6,2,QLatin1Char('0')); // func
        }
        else if(strlist.at(0)=="syscall"||strlist.at(0)=="break"){ // 陷阱 ??break存疑
            return QString("%1").arg(0,6,2,QLatin1Char('0')) +" "+ // op:0*6
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // rs:0*5
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // rt:0*5
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // rd:0*5
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // shamt:0*5
                    QString("%1").arg(fcmap[strlist.at(0)],6,2,QLatin1Char('0')); // func
        }
        else if(j==1){ // label 讲道理label应该去掉 j的时候直接存map对应的地址
            ;
        }
        else{ // R
            return QString("%1").arg(0,6,2,QLatin1Char('0')) +" "+ // op:0*6
                    QString("%1").arg(regmap[strlist.at(1)],5,2,QLatin1Char('0')) +" "+ // rs
                    QString("%1").arg(regmap[strlist.at(2)],5,2,QLatin1Char('0')) +" "+ // rt
                    QString("%1").arg(regmap[strlist.at(3)],5,2,QLatin1Char('0')) +" "+ // rd
                    QString("%1").arg(0,5,2,QLatin1Char('0')) +" "+ // shamt
                    QString("%1").arg(fcmap[strlist.at(0)],6,2,QLatin1Char('0')); // func
        }
    }
    return "ERROR";
}

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <QString>
#include <QMap>
//QMap set default value 0

class Assembler{
public:
    Assembler();
    QString convert(const QString &myasm);
    uint assemble(const QStringList &strlist, int pc);
    void prepare(const QStringList &strlist, QStringList *blistp,const int &j, int *pc);
    void data_prepare(const QStringList &strlist, QStringList *blistp,const int &j);

};

#endif // ASSEMBLER_H

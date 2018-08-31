#ifndef FILEREADER_H
#define FILEREADER_H

#include <QDate>
#include <QDebug>
#include <QFileInfo>

struct entry{
    QString date;
    int category;
    QString description;
    float price;
};

class fileReader
{
public:
    fileReader();
    std::vector<entry> readFile(QString fileName);
};

#endif // FILEREADER_H

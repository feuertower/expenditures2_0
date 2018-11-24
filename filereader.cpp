#include "filereader.h"

fileReader::fileReader()
{

}

std::vector<entry> fileReader::readFile(QString fileName)
{
    std::vector<entry> entryVec;
    QFile costsFile;
    costsFile.setFileName(fileName);
    QFile tmp("tmp.txt");
    tmp.open(QIODevice::WriteOnly);

    if(!costsFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could´t open file:  " << fileName << endl;
    }
    else
    {
        QTextStream streamRead(&costsFile);
        streamRead.skipWhiteSpace();

        while(!streamRead.atEnd())
        {
            entry e;
            QStringList daten = streamRead.readLine().split(";");
            e.date = daten[0];

            if(e.date == "#" || e.date == NULL) continue;

            QDate lastDate = QDate::fromString(e.date, "dd.MM.yyyy");
            e.date = lastDate.toString("yyyy-MM-dd");
            e.category = daten[1].toInt();
            e.description = daten[2];
            e.price = daten[3].toFloat();

            int diffMonth = QDate::currentDate().month() - lastDate.month();
            if(QDate::currentDate().year() - lastDate.year()) diffMonth += 12;

            while(diffMonth > 0)
            {
                lastDate = QDate::fromString(e.date, "yyyy-MM-dd");
                lastDate = lastDate.addMonths(1);
                e.date = lastDate.toString("yyyy-MM-dd");

                entryVec.push_back(e);
                diffMonth--;
            }
            e.date = lastDate.toString("dd.MM.yyyy");
            QTextStream writeStream(&tmp);
            writeStream << e.date << ";" << e.category << ";" << e.description << ";" << e.price << ";\r\n";

        }
    }
    costsFile.close();
    tmp.close();
    costsFile.remove();
    tmp.rename("tmp.txt", "fixedCosts.txt");

    return entryVec;
}

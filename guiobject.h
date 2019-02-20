#ifndef GUIOBJECT_H
#define GUIOBJECT_H
//  includes for charts
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
QT_CHARTS_USE_NAMESPACE

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QLineEdit>
#include <QWidget>
#include <QSpacerItem>
#include <QDialog>
#include <QTabWidget>
#include <QDateEdit>
#include <QComboBox>

//  includes for menuBar
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>

#include <string.h>
#include <vector>

#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>

#include <QKeyEvent>

#include "sqldatabase.h"
#include "filereader.h"

struct chartFragment{
    QString name;
    float value;
    QBrush color;
};


class guiObject : public QMainWindow
{
    Q_OBJECT
public:
    guiObject(QWidget *parent = nullptr);
    ~guiObject();

private:
    void setupUi();
    void backupDb();
    bool copyBackupFile();
    void setMenuActions();
    void showDayTab();
    void showMonthTab();
    void showYearTab();
    void changeDialog(QModelIndex index, QSqlQueryModel *model);
    void keyPressEvent(QKeyEvent *keyEvent);
    void setFixedCosts();
    void setCharts();

    bool isNumber(QString s);

    QChartView* getPieChart(QString heading, std::vector<chartFragment> fragq);
    QStandardItemModel* getItemModel(QSqlQueryModel* model, std::vector<QString> headerNames);

private slots:
    void actionNew();
    void actionSearch();
    void on_tabBarClicked(int index);

private:
    QWidget* m_widget;
    QHBoxLayout* m_mainHbox;
    QVBoxLayout* m_mainVbox;
    QSpacerItem* m_spacerItem;

    QTabWidget* m_tabWidget;

    QWidget* m_charts;

    QWidget* m_dayWidget;
    QTableView* m_dayTable;
    QHBoxLayout* m_dayTabLayout;

    QWidget* m_monthWidget;
    QTableView* m_monthTable;
    QHBoxLayout* m_monthTabLayout;

    QWidget* m_yearWidget;
    QTableView* m_yearTable;
    QHBoxLayout* m_yearTabLayout;

    QChartView* m_categorieChart;
    QPieSeries* m_pieSeries;

    QMenu* m_fileMenu;
    QAction* m_actionNewEntry;
    QAction* m_actionSearchEntry;

    QDialog* m_newDialog;
    QHBoxLayout* m_newDialogHbox;
    QVBoxLayout* m_newDialogVbox;
    QPushButton* m_newDialogButton;
    QLineEdit* m_newDialogLineEditDesc;
    QLineEdit* m_newDialogLineEditPrice;
    QDateEdit* m_newDialogDateEdit;
    QComboBox* m_newDialogComboBox;

    QDialog* m_findDialog;
    QHBoxLayout* m_findDialogHbox;
    QPushButton* m_findDialogButton;
    QLineEdit* m_findDialogLineEdit;

    sqlDatabase* m_sqlDB;

    QBrush m_colors[10];
};

#endif // GUIOBJECT_H

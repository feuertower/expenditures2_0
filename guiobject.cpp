#include "guiobject.h"

guiObject::guiObject(QWidget *parent)
{
    m_widget = new QWidget();
    m_mainHbox = new QHBoxLayout();
    m_mainVbox = new QVBoxLayout();
    m_spacerItem = new QSpacerItem(100, 1);

    m_tabWidget = new QTabWidget();

    m_dayTable = new QTableView();
    m_dayWidget = new QWidget();
    m_dayTabLayout = new QHBoxLayout();

    m_monthTable = new QTableView();
    m_monthWidget = new QWidget();
    m_monthTabLayout = new QHBoxLayout();

    m_yearTable = new QTableView();
    m_yearWidget = new QWidget();
    m_yearTabLayout = new QHBoxLayout();

    m_charts = new QWidget();
    m_categorieChart = new QChartView();

    m_tabWidget->addTab(m_dayWidget, tr("day"));
    m_tabWidget->addTab(m_monthWidget, tr("month"));
    m_tabWidget->addTab(m_yearWidget, tr("year"));
    m_tabWidget->addTab(m_charts, tr("charts"));
    QObject::connect(m_tabWidget, &QTabWidget::tabBarClicked, this,
                     guiObject::on_tabBarClicked);

    m_mainHbox->addWidget(m_tabWidget);
    m_mainVbox->addLayout(m_mainHbox);
    m_mainVbox->setMargin(10);

    m_widget->setLayout(m_mainVbox);

    setCentralWidget(m_widget);
    setWindowTitle("Costs");
    resize(1000, 900);

    setMenuActions();
    m_fileMenu = menuBar()->addMenu(tr("Actions"));
    m_fileMenu->addAction(m_actionNewEntry);
    m_fileMenu->addAction(m_actionSearchEntry);

    m_sqlDB = new sqlDatabase();
    QString dbMsg = (m_sqlDB->openDb() == true) ? tr("DB connection succeed") :
                                                  tr("DB connection failed");
    statusBar()->showMessage(dbMsg);

    setFixedCosts();
    setCharts();
    //Show day tab after start
    showDayTab();
}

guiObject::~guiObject()
{
    delete m_yearTabLayout;
    delete m_yearWidget;
    delete m_yearTable;
    delete m_monthTabLayout;
    delete m_monthWidget;
    delete m_monthTable;
    delete m_dayTabLayout;
    delete m_dayWidget;
    delete m_dayTable;
    delete m_tabWidget;
    delete m_spacerItem;
    delete m_mainHbox;
    delete m_mainVbox;
    delete m_widget;
}

void guiObject::setMenuActions()
{
    m_actionNewEntry = new QAction(tr("&New Entry"), this);
    m_actionNewEntry->setShortcuts(QKeySequence::New);
    QObject::connect(m_actionNewEntry, &QAction::triggered, this,
                     guiObject::actionNew);

    m_actionSearchEntry = new QAction(tr("&Search Entry"), this);
    m_actionSearchEntry->setShortcut(QKeySequence::Find);
    QObject::connect(m_actionSearchEntry, &QAction::triggered, this,
                     guiObject::actionSearch);

}

void guiObject::showDayTab()
{
    std::vector<QString> headerNames;
    headerNames.push_back("ID");
    headerNames.push_back("Price");
    headerNames.push_back("Description");
    headerNames.push_back("Category");
    headerNames.push_back("Date");

    QString queryStr = "SELECT E.Date, C.categorie, E.Description, E.Price, E.ID FROM Expenditures as E INNER JOIN Categories as C ON E.Categorie = C.ID ORDER BY E.Date DESC";

    QSqlQuery* query = m_sqlDB->executeQuery(queryStr);
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(*query);

    QStandardItemModel* itemModel = getItemModel(model, headerNames);
    m_dayTable->setModel(itemModel);
    m_dayTable->resizeColumnToContents(2);
    m_dayTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_dayTable->setAlternatingRowColors(true);
    m_dayTabLayout->addWidget(m_dayTable);
    m_dayWidget->setLayout(m_dayTabLayout);

    QObject::connect(m_dayTable, &QTableView::doubleClicked,
                     [=](){
        QModelIndex index =  m_dayTable->currentIndex();
        changeDialog(index, model);
    });

}

void guiObject::showMonthTab()
{
    QString queryStr = "SELECT case strftime('%m', date) "
                       "when '01' then 'Januar' "
                       "when '02' then 'Februar' "
                       "when '03' then 'März' "
                       "when '04' then 'April' "
                       "when '05' then 'Mai' "
                       "when '06' then 'Juni' "
                       "when '07' then 'Juli' "
                       "when '08' then 'August' "
                       "when '09' then 'September' "
                       "when '10' then 'Oktober' "
                       "when '11' then 'November' "
                       "when '12' then 'Dezember' "
                       "else '' end as Monat, strftime('%Y', date) as Jahr, sum(price) as Price "
                       "FROM expenditures GROUP BY Monat, Jahr ORDER BY Jahr DESC, strftime('%m', date) DESC;";

    QSqlQuery* query = m_sqlDB->executeQuery(queryStr);
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(*query);

    std::vector<QString> headerNames;
    headerNames.push_back("Costs");
    headerNames.push_back("Year");
    headerNames.push_back("Month");
    QStandardItemModel* itemModel = getItemModel(model, headerNames);

    int month = model->rowCount();
    queryStr = "SELECT sum(price)/";
    queryStr.append(QString::number(month));
    queryStr.append(" FROM Expenditures");
    model->setQuery(queryStr);
    QList<QStandardItem*> itemList;
    QModelIndex index = model->index(0, 0, QModelIndex());
    QVariant var = model->data(index);
    QStandardItem* item = new QStandardItem(QString::number(var.toFloat(), 'f', 2) + "€");
    itemList.append(item);
    item->setTextAlignment(Qt::AlignRight);
    itemModel->appendColumn(itemList);
    QStandardItem* headerItem = new QStandardItem("Average");
    itemModel->setHorizontalHeaderItem(3, headerItem);


    m_monthTable->setModel(itemModel);
    m_monthTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_monthTable->setAlternatingRowColors(true);

    delete m_monthTabLayout;
    m_monthTabLayout = new QHBoxLayout();

    m_monthTabLayout->addWidget(m_monthTable);
    m_monthWidget->setLayout(m_monthTabLayout);

}

void guiObject::showYearTab()
{
    QString queryStr = "SELECT strftime('%Y', date) as Jahr, sum(price) as Price FROM Expenditures GROUP BY Jahr ORDER BY Jahr DESC;";
    QSqlQuery* query = m_sqlDB->executeQuery(queryStr);
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(*query);

    std::vector<QString> headerNames;
    headerNames.push_back("Average");
    headerNames.push_back("Total");
    headerNames.push_back("Year");
    QStandardItemModel* itemModel = getItemModel(model, headerNames);

    m_yearTable->setModel(itemModel);
    m_yearTabLayout->addWidget(m_yearTable);
    m_yearWidget->setLayout(m_yearTabLayout);
    m_yearTable->setAlternatingRowColors(true);
    m_yearTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void guiObject::keyPressEvent(QKeyEvent *keyEvent)
{
    QItemSelectionModel* selectionModel = m_dayTable->selectionModel();
    QModelIndex currentIndex = m_dayTable->currentIndex();
    if(keyEvent->key() == Qt::Key_Delete && selectionModel->isSelected(currentIndex))
    {
        QDialog* dialog = new QDialog(this);
        QVBoxLayout* vbox = new QVBoxLayout();
        QLabel* label = new QLabel();
        QPushButton* button = new QPushButton("Delete");

        QString labelText;
        int row = currentIndex.row();
        QModelIndex index = m_dayTable->model()->index(row, 4, QModelIndex());
        QString id = m_dayTable->model()->data(index, Qt::DisplayRole).toString();
        labelText = "Do you want to delete entry ";
        labelText.append(id);
        labelText.append("?");
        label->setText(labelText);

        vbox->addWidget(label);
        vbox->addWidget(button);
        dialog->setLayout(vbox);
        dialog->show();

        QObject::connect(button, &QPushButton::clicked,
                         [=](){
            QString queryStr = "DELETE FROM Expenditures WHERE id=";
            queryStr.append(id);
            m_sqlDB->executeQuery(queryStr);
            qDebug() << queryStr << endl;
            showDayTab();

            delete button;
            delete label;
            delete vbox;
            delete dialog;
        });
    }
}

bool guiObject::isNumber(QString s)
{
    bool result;
    float tmp;
    tmp = s.toFloat();
    tmp != 0 ? result = false : result = true;
    return result;
}

void guiObject::setFixedCosts()
{
    fileReader reader;
    std::vector<entry> entryVec;
    entryVec = reader.readFile("fixedCosts.txt");

    for(entry e : entryVec)
    {
        QString queryStr = "INSERT INTO Expenditures(date, categorie, description, price) VALUES(\"";
        queryStr.append(e.date);
        queryStr.append("\", \"");
        queryStr.append(QString::number(e.category));
        queryStr.append("\", \"");
        queryStr.append(e.description);
        queryStr.append("\", \"");
        queryStr.append(QString::number(e.price));
        queryStr.append("\")");

        m_sqlDB->executeQuery(queryStr);
        statusBar()->showMessage("fixed costs set!");
    }
}

void guiObject::setCharts()
{
    //  get divider(month) for average expenditure
    QString queryStrMonth = "SELECT strftime('%m', date) as Monat, strftime('%Y', date) as Jahr, sum(price) as Price FROM expenditures GROUP BY Monat, Jahr ORDER BY Jahr DESC, strftime('%m', date) DESC;";

    QSqlQuery* queryMonth = m_sqlDB->executeQuery(queryStrMonth);
    QSqlQueryModel* modelMonth = new QSqlQueryModel();
    modelMonth->setQuery(*queryMonth);

    int month = modelMonth->rowCount();
    //-----------------------------------------------


    std::vector<chartFragment> pieVec;
    QString queryStr = "select c.Categorie, sum(e.Price) as price from expenditures as e inner join Categories as c on e.Categorie = c.ID group by c.Categorie order by price asc";
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(queryStr);

    for(int i = 0; i < model->rowCount(); i++)
    {
        chartFragment frag;
        QModelIndex indexName = model->index(i, 0, QModelIndex());
        QModelIndex indexCost = model->index(i, 1, QModelIndex());
        frag.name = model->data(indexName).toString() + "(" + QString::number(model->data(indexCost).toFloat() / month, 'f', 2) + "€" + ")";
        frag.value = model->data(indexCost).toFloat();
        pieVec.push_back(frag);
    }
    m_categorieChart = getPieChart("Categories monthly", pieVec);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_categorieChart);
    m_charts->setLayout(layout);

}

QStandardItemModel *guiObject::getItemModel(QSqlQueryModel *model,
                                            std::vector<QString> headerNames)
{
    while(model->canFetchMore())
        model->fetchMore();

    int columns = model->columnCount();
    int rows = model->rowCount();

    QStandardItemModel* itemModel = new QStandardItemModel();
    for(int i = 0; i < columns; i++)
    {
        QString name = headerNames.back();
        headerNames.pop_back();
        itemModel->setHorizontalHeaderItem(i, new QStandardItem(name));
    }

    for(int i = 0; i < rows; i++)
    {
        QList<QStandardItem*> *list = new QList<QStandardItem*>;
        for(int j = 0; j < columns; j++)
        {
            QModelIndex index = model->index(i, j, QModelIndex());
            QString columnName = model->record(i).fieldName(j);

            QStandardItem* item;
            if(columnName == "Price")
            {
                item = new QStandardItem(QString::number(model->data(index, Qt::DisplayRole).toFloat(), 'f', 2) + "€");
                item->setTextAlignment(Qt::AlignRight);
            }
            else if(columnName == "Date")
            {
                QString dateDb = model->data(index, Qt::DisplayRole).toString();
                QDate date = QDate::fromString(dateDb, "yyyy-MM-dd");
                item = new QStandardItem(date.toString("dd. MMMM yyyy"));
            }
            else if(columnName == "ID")
            {
                item = new QStandardItem(model->data(index, Qt::DisplayRole).toString());
                item->setTextAlignment(Qt::AlignRight);
            }
            else
            {
                item = new QStandardItem(model->data(index, Qt::DisplayRole).toString());
            }

            list->append(item);
        }
        itemModel->appendRow(*list);
    }

    return itemModel;
}

QChartView *guiObject:: getPieChart(QString heading, std::vector<chartFragment> frag)
{    
    QColor rgb;
    for(int i = 0; i < 10; i++)
    {
        rgb.setRgb(100, 100, i*25);
        m_colors[i] = rgb;
    }

    m_pieSeries = new QPieSeries();
    for(auto addFrag : frag)
    {
        m_pieSeries->append(addFrag.name, addFrag.value);
    }

    int seriesSize = m_pieSeries->count();
    for(int i = 0; i < seriesSize; i++)
    {
        QPieSlice* slice = m_pieSeries->slices().at(i);
        chartFragment fragment = frag.at(i);
        //        slice->setBrush(fragment.color);
        slice->setBrush(m_colors[i]);
        slice->setLabelVisible(false);
    }

    QChart *chart = new QChart();
    chart->addSeries(m_pieSeries);
    chart->setTitle(heading);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);


    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    return chartView;
}

void guiObject::actionNew()
{
    statusBar()->showMessage(tr("action new triggered"));
    m_newDialog = new QDialog(this);
    m_newDialog->setWindowTitle(tr("New Entry"));
    m_newDialog->setVisible(true);

    m_newDialogVbox = new QVBoxLayout();
    m_newDialogButton = new QPushButton(tr("Insert"));
    m_newDialogLineEditDesc = new QLineEdit();
    m_newDialogLineEditPrice = new QLineEdit();
    m_newDialogDateEdit = new QDateEdit();
    m_newDialogComboBox = new QComboBox();

    QFont font;
    font.setPointSize(15);
    m_newDialogDateEdit->setDate(QDate::currentDate());
    m_newDialogDateEdit->setFont(font);
    QString queryStr = "SELECT categorie FROM categories";
    QSqlQuery* query = m_sqlDB->executeQuery(queryStr);
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(*query);
    m_newDialogComboBox->setModel(model);
    m_newDialogComboBox->setFont(font);
    m_newDialogButton->setFont(font);
    m_newDialogLineEditDesc->setFixedHeight(80);
    m_newDialogLineEditDesc->setFixedWidth(400);
    m_newDialogLineEditDesc->setAlignment(Qt::AlignTop);
    m_newDialogLineEditDesc->setPlaceholderText("Description");
    font.setPointSize(12);
    m_newDialogLineEditDesc->setFont(font);
    m_newDialogLineEditPrice->setPlaceholderText("Price");
    m_newDialogLineEditPrice->setFont(font);

    m_newDialogVbox->addWidget(m_newDialogDateEdit);
    m_newDialogVbox->addWidget(m_newDialogComboBox);
    m_newDialogVbox->addWidget(m_newDialogLineEditDesc);
    m_newDialogVbox->addWidget(m_newDialogLineEditPrice);
    m_newDialogVbox->addWidget(m_newDialogButton);

    m_newDialog->setLayout(m_newDialogVbox);

    QObject::connect(m_newDialogButton, &QPushButton::clicked,
                     [&](){
        int category = m_newDialogComboBox->currentIndex()+1;
        QDate date = m_newDialogDateEdit->date();
        QString queryStr = "INSERT INTO Expenditures(date, categorie, description, price) VALUES (\"";
        queryStr.append(date.toString("yyyy-MM-dd"));
        queryStr.append("\", \"");
        queryStr.append(QString::number(category));
        queryStr.append("\", \"");
        queryStr.append(m_newDialogLineEditDesc->text());
        queryStr.append("\", \"");
        queryStr.append(m_newDialogLineEditPrice->text());
        queryStr.append("\");");

        m_sqlDB->executeQuery(queryStr);
        delete m_newDialog;
        showDayTab();
    });
}

/**
 * @brief guiObject::actionSearch
 * issue: if one searches for substring -> error in sum
 */
void guiObject::actionSearch()
{
    statusBar()->showMessage(tr("action search triggered"));
    m_findDialog = new QDialog(this);
    m_findDialog->setWindowTitle(tr("Search Entry"));
    m_findDialog->setVisible(true);
    m_findDialog->resize(300, 40);

    m_findDialogHbox = new QHBoxLayout();
    m_findDialogButton = new QPushButton(tr("Search"));
    m_findDialogLineEdit = new QLineEdit();
    m_findDialogLineEdit->resize(300, 20);
    m_findDialogLineEdit->setPlaceholderText("Type Search String here");
    m_findDialogHbox->addWidget(m_findDialogLineEdit);
    m_findDialogHbox->addWidget(m_findDialogButton);

    m_findDialog->setLayout(m_findDialogHbox);

    QObject::connect(m_findDialogButton, &QPushButton::clicked,
                     [&](){
        QDialog* searchResult = new QDialog(this);
        searchResult->resize(500, 700);
        QHBoxLayout* resultHLayout = new QHBoxLayout();
        QVBoxLayout* resultVLayout = new QVBoxLayout();
        QLabel* sumLabel = new QLabel("testSum");
        QLabel* averageLabel = new QLabel("testAv");
        QLabel* countLabel = new QLabel("testCount");

        QString querySearchEntries = "SELECT E.Date, C.categorie, E.Description, E.Price FROM Expenditures as E INNER JOIN Categories as C ON E.Categorie = C.ID WHERE description LIKE \"";
        querySearchEntries.append(m_findDialogLineEdit->text());
        querySearchEntries.append("%\" COLLATE NOCASE ORDER BY E.Date DESC");
        qDebug() << querySearchEntries << endl;
        m_sqlDB->executeQuery(querySearchEntries);

        QString queryData = "SELECT SUM(E.Price), Count(E.Price) FROM Expenditures as E WHERE description=\"";
        queryData.append(m_findDialogLineEdit->text());
        queryData.append("\" COLLATE NOCASE");
        qDebug() << queryData << endl;
        QSqlQuery* sqlData = m_sqlDB->executeQuery(queryData);
        sqlData->next();
        sumLabel->setText("Gesamt: " + QString::number(sqlData->value(0).toFloat(), 'f', 2) + "€");
        countLabel->setText("Einträge: " + sqlData->value(1).toString());
        float average = sqlData->value(0).toFloat()/sqlData->value(1).toFloat();
        averageLabel->setText("Durchschnitt: " + QString::number(average, 'f', 2) + "€");

        delete m_findDialog;

        QSqlQuery* query = m_sqlDB->executeQuery(querySearchEntries);
        QSqlQueryModel* model = new QSqlQueryModel();
        model->setQuery(*query);

        std::vector<QString> headerNames;
        headerNames.push_back("Price");
        headerNames.push_back("Description");
        headerNames.push_back("Category");
        headerNames.push_back("Date");

        QStandardItemModel* itemModel = getItemModel(model, headerNames);
        searchResult->setWindowTitle("Search Result");
        searchResult->setVisible(true);
        QTableView* searchModel = new QTableView(searchResult);
        searchModel->setModel(itemModel);
        searchModel->resizeColumnToContents(2);
        searchModel->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        searchModel->setAlternatingRowColors(true);
        searchModel->setSelectionMode(QAbstractItemView::NoSelection);

        resultHLayout->addWidget(sumLabel);
        resultHLayout->addWidget(averageLabel);
        resultHLayout->addWidget(countLabel);
        resultVLayout->addWidget(searchModel);
        resultVLayout->addLayout(resultHLayout);
        searchResult->setLayout(resultVLayout);
    });
}

void guiObject::on_tabBarClicked(int index)
{
    switch(index)
    {
    case 0:
        statusBar()->showMessage(tr("Day tab clicked"));
        showDayTab();
        break;
    case 1:
        statusBar()->showMessage(tr("Month tab clicked"));
        showMonthTab();
        break;
    case 2:
        statusBar()->showMessage(tr("Year tab clicked"));
        showYearTab();
        break;
    }
}

void guiObject::changeDialog(QModelIndex index, QSqlQueryModel *model)
{
    int row = index.row();
    QModelIndex dateIndex = model->index(row, 0);
    QModelIndex categoryIndex = model->index(row, 1);
    QModelIndex descIndex = model->index(row, 2);
    QModelIndex priceIndex = model->index(row, 3);
    QModelIndex idIndex = model->index(row, 4);

    QDate date = model->data(dateIndex, Qt::DisplayRole).toDate();
    QString category = model->data(categoryIndex, Qt::DisplayRole).toString();
    QString description = model->data(descIndex, Qt::DisplayRole).toString();
    QString price = model->data(priceIndex, Qt::DisplayRole).toString();
    QString id = model->data(idIndex, Qt::DisplayRole).toString();

    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Change Entry");
    dialog->setVisible(true);

    QVBoxLayout* vbox = new QVBoxLayout();
    QLabel* label = new QLabel();
    QPushButton* changeButton = new QPushButton("Change");

    label->setText("You´re going to change " + id + "!");

    vbox->addWidget(label);

    QDateEdit* dateEdit = new QDateEdit(date);
    QComboBox* comboBoxCategory = new QComboBox();
    QLineEdit* lineEditDesc = new QLineEdit();
    QLineEdit* lineEditPrice = new QLineEdit();

    QFont font;
    font.setPointSize(15);

    QString queryStr = "SELECT categorie FROM categories";
    QSqlQuery* query = m_sqlDB->executeQuery(queryStr);
    model = new QSqlQueryModel();
    model->setQuery(*query);
    comboBoxCategory->setModel(model);
    comboBoxCategory->setFont(font);
    comboBoxCategory->setCurrentText(category);
    lineEditDesc->setText(description);
    lineEditDesc->setFont(font);
    lineEditDesc->setFixedHeight(80);
    lineEditDesc->setFixedWidth(400);
    lineEditDesc->setAlignment(Qt::AlignTop);
    lineEditPrice->setText(price);
    lineEditPrice->setFont(font);
    changeButton->setFont(font);
    dateEdit->setFont(font);
    label->setFont(font);

    vbox->addWidget(dateEdit);
    vbox->addWidget(comboBoxCategory);
    vbox->addWidget(lineEditDesc);
    vbox->addWidget(lineEditPrice);
    vbox->addWidget(changeButton);
    dialog->setLayout(vbox);

    QObject::connect(changeButton, &QPushButton::clicked,
                     [=](){
       QString updateStr = "UPDATE Expenditures SET";
       updateStr.append(" date = \"");
       updateStr.append(dateEdit->date().toString("yyyy-MM-dd"));
       updateStr.append("\", categorie = \"");
       updateStr.append(QString::number(comboBoxCategory->currentIndex()+1));
       updateStr.append("\", description = \"");
       updateStr.append(lineEditDesc->text());
       updateStr.append("\", price = \"");
       updateStr.append(lineEditPrice->text());
       updateStr.append("\" WHERE id = ");
       updateStr.append(id);

       qDebug() << updateStr << endl;
       m_sqlDB->executeQuery(updateStr);

       showDayTab();
       delete dialog;
    });

}

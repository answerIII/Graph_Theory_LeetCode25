#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QIntValidator>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->txtBasicK->setValidator(new QIntValidator(1, INT_MAX, this));
    ui->txtBasicM->setValidator(new QIntValidator(1, INT_MAX, this));
    ui->txtBFSK->setValidator(new QIntValidator(1, INT_MAX, this));
    ui->txtBFSM->setValidator(new QIntValidator(1, INT_MAX, this));
    ui->txtS->setValidator(new QIntValidator(0, INT_MAX, this));
    ui->txtT->setValidator(new QIntValidator(0, INT_MAX, this));

    connect(ui->createGraphButton, &QPushButton::clicked,
            this, &MainWindow::onCreateGraphButtonClicked);

    connect(ui->btnCreateBasic, &QPushButton::clicked,
            this, &MainWindow::onCreateBasicClicked);

    connect(ui->btnCreateBFS, &QPushButton::clicked,
            this, &MainWindow::onCreateBFSClicked);

    connect(ui->btnExact, &QPushButton::clicked, this, &MainWindow::onExactCalculateClicked);
    connect(ui->btnApproxBFS, &QPushButton::clicked, this, &MainWindow::onApproxBFSClicked);
    connect(ui->btnApproxBasic, &QPushButton::clicked, this, &MainWindow::onApproxBasicClicked);

    connect(ui->btnCutRun, &QPushButton::clicked, this, &MainWindow::onCutRunClicked);
}


void MainWindow::onExactCalculateClicked() {
    if(!currentGraph) {
        QMessageBox::warning(this, "Ошибка", "Сначала создайте граф!");
        return;
    }

    bool ok;
    uint32 s = ui->txtS->text().toUInt(&ok);
    if(!ok) {
        QMessageBox::warning(this, "Ошибка", "Некорректная вершина s!");
        return;
    }

    uint32 t = ui->txtT->text().toUInt(&ok);
    if(!ok) {
        QMessageBox::warning(this, "Ошибка", "Некорректная вершина t!");
        return;
    }

    try {
        bool internal = ui->chkInternalRepr->isChecked();
        uint64 result = currentGraph->findShortestPath(s, t, internal);
        showResult("Точный алгоритм", result);
    } catch(const std::exception& e) {
        QMessageBox::critical(this, "Ошибка", QString("Ошибка: %1").arg(e.what()));
    }
}


void MainWindow::onApproxBFSClicked() {
    if(!landmarksBFS) {
        QMessageBox::warning(this, "Ошибка", "Сначала создайте Landmarks BFS!");
        return;
    }

    bool ok;
    uint32 s = ui->txtS->text().toUInt(&ok);
    if(!ok) {
        QMessageBox::warning(this, "Ошибка", "Некорректная вершина s!");
        return;
    }

    uint32 t = ui->txtT->text().toUInt(&ok);
    if(!ok) {
        QMessageBox::warning(this, "Ошибка", "Некорректная вершина t!");
        return;
    }

    bool internal = ui->chkInternalRepr->isChecked();
    uint64 result = landmarksBFS->computeApproximateDistance(
        *currentGraph, s, t, internal
        );
    showResult("Landmarks BFS", result);
}


void MainWindow::onApproxBasicClicked() {
    if(!landmarksBasic) {
        QMessageBox::warning(this, "Ошибка", "Сначала создайте Landmarks Basic!");
        return;
    }

    bool ok;
    uint32 s = ui->txtS->text().toUInt(&ok);
    if(!ok) {
        QMessageBox::warning(this, "Ошибка", "Некорректная вершина s!");
        return;
    }

    uint32 t = ui->txtT->text().toUInt(&ok);
    if(!ok) {
        QMessageBox::warning(this, "Ошибка", "Некорректная вершина t!");
        return;
    }

    bool internal = ui->chkInternalRepr->isChecked();
    uint64 result = landmarksBasic->computeApproximateDistance(
        *currentGraph, s, t, internal
        );
    showResult("Landmarks Basic", result);
}


void MainWindow::showResult(const QString& method, uint64 result) {
    QString msg = QString("[%1] %2 -> %3 = %4\n")
    .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
        .arg(ui->txtS->text())
        .arg(ui->txtT->text())
        .arg(result);

    ui->txtResults->append(QString("%1Результат: %2").arg(method).arg(msg));
}


void MainWindow::onCreateGraphButtonClicked() {
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    "Выберите файл графа",
                                                    "",
                                                    "Graph Files (*.txt *.graph)"
                                                    );

    if (!filePath.isEmpty()) {
        try {
            isDirected = ui->chkIsDirected->isChecked();
            currentGraph = new Graph(filePath.toStdString(), !isDirected);
            isDirected = currentGraph->isDirected();
            this->updateStats();
            this->updateDegreeStats();
            if (isDirected) {
                QMessageBox::information(this, "Успех", "Ориентированный граф создан!");
            } else {
                QMessageBox::information(this, "Успех", "Неориентированный граф создан!");
            }

        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Ошибка",
                                  QString("Ошибка создания графа: %1").arg(e.what()));
        }
    }
}


void MainWindow::updateStats()
{
    if (!currentGraph) {
        ui->textEditStats->setPlainText("Граф не загружен.");
        return;
    }

    QString report;

    if (isDirected) {
        auto resp = currentGraph->Analyze_SCC();
        report += QString("SCC: count = %1, max size = %2, part = %3\n")
                      .arg(resp.num_of_sets_dir)
                      .arg(resp.max_size_dir)
                      .arg(resp.part);
    }

    {
        auto resp = currentGraph->Analyze_WCC();
        report += QString("WCC: count = %1, max size = %2, part = %3\n")
                      .arg(resp.num_of_sets)
                      .arg(resp.max_WCC_size)
                      .arg(resp.part);
    }

    {
        uint32_t d = currentGraph->Analyze_diameter();
        report += QString("Diameter = %1\n").arg(d);
    }

    {
        uint32_t p90rand = currentGraph->Analyze_diameter_b();
        uint32_t p90snow = currentGraph->Analyze_diameter_c();
        report += QString("90%-percentile random pairs = %1\n").arg(p90rand);
        report += QString("90%-percentile snowball     = %1\n").arg(p90snow);
    }

    {
        auto cl = currentGraph->AverageClusteringCoefficient();
        report += QString("Triangles total = %1\n").arg(cl.triangles_total);
        report += QString("Average coefficient = %1\n").arg(cl.average_coefficient);
        report += QString("WCC coefficient     = %1\n").arg(cl.WCC_coefficient);

        double gcc = currentGraph->GlobalClusteringCoefficient();
        report += QString("Global clustering coefficient = %1\n").arg(gcc);
    }

    ui->textEditStats->setPlainText(report);
}


void MainWindow::updateDegreeStats()
{
    if (!currentGraph) {
        ui->textEditDegrees->setPlainText("Граф не загружен.");
        return;
    }

    auto resp = currentGraph->Analyze_degrees();
    size_t numNodes = currentGraph->vertixNumber();

    QString report;
    report += QString("Min degree: %1\n").arg(resp.minDegree);
    report += QString("Max degree: %1\n").arg(resp.maxDegree);
    report += QString("Average degree: %1\n\n").arg(resp.avgDegree);

    report += "Degree distribution (normal scale):\n";
    report += "Degree\tCount\tProbability\n";
    for (auto const& [deg, cnt] : resp.degreeCount) {
        double prob = double(cnt) / double(numNodes);
        report += QString("%1\t%2\t%3\n")
                      .arg(deg)
                      .arg(cnt)
                      .arg(prob, 0, 'f', 4);
    }

    report += "\nDegree distribution (log-log scale):\n";
    report += "log10(Degree)\tlog10(Probability)\n";
    for (auto const& [deg, cnt] : resp.degreeCount) {
        if (deg == 0) continue;
        double prob = double(cnt) / double(numNodes);
        if (prob <= 0.0) continue;
        report += QString("%1\t%2\n")
                      .arg(std::log10(double(deg)), 0, 'f', 4)
                      .arg(std::log10(prob), 0, 'f', 4);
    }

    ui->textEditDegrees->setPlainText(report);
}


void MainWindow::onCreateBasicClicked() {
    if (!currentGraph || isDirected) {
        QMessageBox::warning(this, "Ошибка", "Сначала создайте неориентированный граф!");
        return;
    }

    try {
        QString method = ui->cmbBasicMethod->currentText();
        uint32 k = ui->txtBasicK->text().toUInt();
        uint32 M = ui->txtBasicM->text().isEmpty() ? 0 : ui->txtBasicM->text().toUInt();

        landmarksBasic = new LandmarksBasic(*currentGraph,
                                            method.toStdString(),
                                            k,
                                            M);
        QMessageBox::information(this, "Успех", "LandmarksBasic создан!");
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка",
                              QString("Ошибка: %1").arg(e.what()));
    }
}

void MainWindow::onCreateBFSClicked() {
    if (!currentGraph || isDirected) {
        QMessageBox::warning(this, "Ошибка", "Сначала создайте неориентированный граф!");
        return;
    }

    try {
        QString method = ui->cmbBFSMethod->currentText();
        uint32 k = ui->txtBFSK->text().toUInt();
        uint32 M = ui->txtBFSM->text().isEmpty() ? 0 : ui->txtBFSM->text().toUInt();

        landmarksBFS = new LandmarksBFS(*currentGraph,
                                        method.toStdString(),
                                        k,
                                        M);
        QMessageBox::information(this, "Успех", "LandmarksBFS создан!");
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка",
                              QString("Ошибка: %1").arg(e.what()));
    }
}

void MainWindow::onCutRunClicked()
{
    if (!currentGraph) {
        QMessageBox::warning(this, "Ошибка", "Граф не загружен");
        return;
    }

    int percent = ui->spinCutPercent->value();

    QString method = ui->comboCutMethod->currentText();
    if (method == "Random_cut") {
        currentGraph->Random_cut(percent);
    } else if (method == "Max_cut") {
        currentGraph->Max_cut(percent);
    }

    updateStats();
    updateDegreeStats();
}


MainWindow::~MainWindow()
{
    delete ui;
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "graph.h"
#include "landmarksBasic.h"
#include "landmarksBFS.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Graph* currentGraph = nullptr;
    bool isDirected = false;
    LandmarksBasic* landmarksBasic = nullptr;
    LandmarksBFS* landmarksBFS = nullptr;

    void showResult(const QString& method, uint64 result);
    void updateStats();
    void updateDegreeStats();

private slots:
    void onCreateGraphButtonClicked();
    void onCreateBasicClicked();
    void onCreateBFSClicked();
    void onExactCalculateClicked();
    void onApproxBFSClicked();
    void onApproxBasicClicked();
    void onCutRunClicked();
};
#endif // MAINWINDOW_H

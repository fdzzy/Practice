#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "debug.h"
#include "node.h"
#include "nut.h"
#include "snake.h"

#define Default_extend_length 3
#define record_file "records.txt"
#define RECORDS_NUM 10

class QAction;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private slots:
    void on_timer_timeout();
    void newGame();
    void showRecords();
    void clearRecords();
    void exitGame();
    void about();
    void aboutQt();

private:
    void createActions();
    void createMenus();
    void createContextMenu();
    void createStatusBar();
    void paintNode(const QColor &color, const Node &node);
    void startGame();
    void pauseGame();
    void stopGame();
    void makeNut();
    void readRecords();
    void writeRecords();
    void setRecord(int candidate);

    QAction *newAction;
    QAction *showRecordsAction;
    QAction *clearRecordsAction;
    QAction *exitAction;
    QAction *aboutAction;
    QAction *aboutQtAction;

    QMenu *gameMenu;
    QMenu *helpMenu;

    QLabel *scoreLabel;
    QLabel *statusLabel;

    bool gameRunning; /* indicate whether game is running */
    bool gamePaused; /* indicate whether game is running */
    int matrixWidth, matrixHeight, pixel;
    int first_lines_shift; /* shift y a little, as it seems the first 25 lines can't be seen */
    int last_lines_shift; /* shift y a little, as it seems the last 25 lines can't be seen */
    int score;
    Direction direction;
    QTimer *timer;
    Snake *snake;
    Nut *nut;
    std::vector<std::pair<int, QString> > records;
};

#endif

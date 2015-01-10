#include <QtWidgets>
#include "mainwindow.h"

MainWindow::MainWindow()
{
    /* Initializtion */
    //matrixWidth = 128;
    //matrixHeight = 96;
    matrixWidth = 64;
    matrixHeight = 48;
    pixel = 8;
    direction = Right;
    gameRunning = false;
    gamePaused = false;
    /* Don't change this, it seems the first 25 lines of pixels can't be shown */
    first_lines_shift = 25;
    /* Also, if the status bar is enabled, the last 23 lines of pixels can't be shown */
    last_lines_shift = 23;

    score = 0;
    snake = NULL;
    nut = NULL;
    timer = new QTimer(this);
    timer->setInterval(200);    /* 200 mili-seconds */
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
    readRecords();
    
    createActions();
    createMenus();
    createContextMenu();
    createStatusBar();
    setWindowTitle(tr("Greedy Snake Game"));
    setFixedSize(matrixWidth * pixel, first_lines_shift + last_lines_shift + matrixHeight * pixel);

    startGame();
}

MainWindow::~MainWindow()
{
    if(timer) delete timer;
    if(snake) delete snake;
    if(nut) delete nut;
}

void MainWindow::readRecords()
{
    QSettings settings("Joey Zhang", "Greedy Snake Game");

    int count = 0;
    if(settings.contains("recordsCount")) {
        count = settings.value("recordsCount").toInt();
    }
    for(int i=0; i<count ; i++) {
        QString keyScore(QString("rankScore%1").arg(i));
        QString keyName(QString("rankName%1").arg(i));
        int score = settings.value(keyScore).toInt();
        QString name(settings.value(keyName).toString());
        records.push_back(std::make_pair(score, name));
    }
}

void MainWindow::writeRecords()
{
    QSettings settings("Joey Zhang", "Greedy Snake Game");

    settings.setValue("recordsCount", records.size());
    for(int i=0; i<records.size() ; i++) {
        QString keyScore(QString("rankScore%1").arg(i));
        QString keyName(QString("rankName%1").arg(i));
        settings.setValue(keyScore, records[i].first);
        settings.setValue(keyName, records[i].second);
    }
}

void MainWindow::clearRecords()
{
    int rtval = QMessageBox::warning(this, tr("Greedy Snake Game"),
                        tr("Are you sure you want to clear the record history?"),
                        QMessageBox::Yes | QMessageBox::No);
    if(rtval == QMessageBox::Yes) {
        QSettings settings("Joey Zhang", "Greedy Snake Game");
        settings.clear();
        records.clear();
    }
}

void MainWindow::showRecords()
{
    QString recordsText;

    if(records.empty()) {
        recordsText.append(tr("No records found!"));
    } else {
        int i;
        /* header */
        recordsText.append(tr("Rank\tName\tScore\n"));
        for(i=0; i<records.size() ; i++) {
            recordsText.append(tr("%1\t%2\t%3\n").arg(i+1).arg(records[i].second).arg(records[i].first));
        }
    }
    QMessageBox::information(this, tr("Greedy Snake Game"), recordsText);
}

void MainWindow::setRecord(int candidate)
{
    int length = records.size(); 
    /* candidate doesn't need to be recorded */
    if(length == RECORDS_NUM && 
        (candidate <= records[RECORDS_NUM-1].first)) {
        QMessageBox::information(this, tr("Greedy Snake Game"),
            tr("Game Over!\nYour score is: %1").arg(score));
    } else {
    /* need to insert a record */
        int i, j;
        QString name = QInputDialog::getText(this, tr("Greedy Snake Game"), 
            tr("You set a record %1\nInput your name: ").arg(score));
        if(name.isEmpty()) {
            name = tr("Anonymous");
        }
        for(i=0; i<length ; i++) {
            if(candidate > records[i].first) break;
        }
        if(length == 0) {
            records.push_back(std::make_pair(candidate, name));
        } else {
            if(length != RECORDS_NUM) {
                records.push_back(records[length-1]);
            }
            for(j=length-1; j>i ; j--) {
                records[j] = records[j-1];
            }
            records[i] = std::make_pair(candidate, name);
        }
        writeRecords();
    }
}

/*
* Wrapper of the Nut::regenerate(), only here can it verify that Nut is not on the body of the snake.
*/
void MainWindow::makeNut()
{
    if(snake == NULL) return;
    if(nut == NULL) {
        nut = new Nut(matrixWidth, matrixHeight);
    } else {
        nut->regenerate();
    }
    while(snake->isBody(nut->getNode())) {
        nut->regenerate();
    }
}

void MainWindow::startGame()
{
    /* starting from paused state */
    if(gameRunning == true && gamePaused == true) {
        gamePaused = false;
        timer->start();
        return;
    }

    /* brand new start */
    if(snake == NULL) {
        snake = new Snake(matrixWidth, matrixHeight);
    } else {
        snake->restart();
    }

    makeNut();
    timer->start();
    score = 0;
    scoreLabel->setText(QString("Score: %1").arg(score));
    direction = Right;
    gameRunning = true;
}

void MainWindow::pauseGame()
{
    if(gameRunning) {
        timer->stop();
        gamePaused = true;
    }
}

void MainWindow::stopGame()
{
    if(gameRunning == false) return;
    timer->stop();
    gameRunning = false;
    gamePaused = false;
}

void MainWindow::paintNode(const QColor &color, const Node &node)
{
    QPainter painter(this);
    painter.setBrush(color);
    painter.drawRect(node.x * pixel, first_lines_shift + node.y * pixel, pixel, pixel);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    LOG("paint event received!\n");
    QPainter painter(this);
    /* Paint the background */
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, size().width(), size().height());

    /* Paint the nut */
    paintNode(Qt::red, nut->getNode());
    
#ifdef DEBUG
    /* Paint the test nodes, the four corners */
    Node test(0,0);
    paintNode(Qt::yellow, test);
    test = Node(0,matrixHeight-1);
    paintNode(Qt::yellow, test);
    test = Node(matrixWidth-1,0);
    paintNode(Qt::yellow, test);
    test = Node(matrixWidth-1,matrixHeight-1);
    paintNode(Qt::yellow, test);
#endif
    
    /* Paint the snake */
    int i;
    const std::vector<Node> &nodes = snake->getNodes();
    for(i=0; i < nodes.size(); i++) { 
        paintNode(Qt::green, nodes[i]);
    }
}

/* FIXME */
//void MainWindow::focusInEvent()
//void MainWindow::focusOutEvent()

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int rtval;
    if(gameRunning == false) {
        return QWidget::keyPressEvent(event);
    }

    switch(event->key()){
    case Qt::Key_Up:
        if(direction != Down)
            direction = Up;
        break;
    case Qt::Key_Down:
        if(direction != Up)
            direction = Down;
        break;
    case Qt::Key_Left:
        if(direction != Right)
            direction = Left;
        break;
    case Qt::Key_Right:
        if(direction != Left)
            direction = Right;
        break;
    case Qt::Key_P:
    case Qt::Key_Space:
        pauseGame();
        rtval = QMessageBox::information(this, tr("Greedy Snake Game"),
                                tr("Game Paused! Press \"OK\" to continue."),
                                QMessageBox::Ok);
        if(rtval == QMessageBox::Ok) {
            startGame();
        }
        break;
    case Qt::Key_Escape:
        pauseGame();
        rtval = QMessageBox::warning(this, tr("Greedy Snake Game"),
                            tr("Are you sure you want to exit?"),
                            QMessageBox::Yes | QMessageBox::No);
        if(rtval == QMessageBox::Yes)
            close();
        startGame();
    default:
        break;
    }
    on_timer_timeout();
}

void MainWindow::on_timer_timeout()
{
    static int extend = 0;
    Node *snake_head;
    LOG("Timer timeout event received!\n");
    if(extend) {
        snake_head = snake->move(direction, true);
        extend--;
    } else {
        snake_head = snake->move(direction);
    }
    if(!snake->isAlive() || snake_head == NULL) {
        stopGame();
        setRecord(score);
        int rtval = QMessageBox::warning(this, tr("Greedy Snake Game"),
            tr("Would you like to try again?"),
            QMessageBox::Yes | QMessageBox::No);
        if(rtval == QMessageBox::Yes) {
            startGame();
        }
        return;
    }
    /* See if the snake gets the nut */
    if(*snake_head == nut->getNode()) {
        extend = Default_extend_length;
        makeNut();
        score += 10;
        scoreLabel->setText(QString("Score: %1").arg(score));
    }
    update();
}
 
void MainWindow::createActions()
{
    newAction = new QAction(tr("&New Game"), this);
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, SIGNAL(triggered()), this, SLOT(newGame()));

    showRecordsAction = new QAction(tr("&Show Records"), this);
    connect(showRecordsAction, SIGNAL(triggered()), this, SLOT(showRecords()));

    clearRecordsAction = new QAction(tr("&Clear Records"), this);
    connect(clearRecordsAction, SIGNAL(triggered()), this, SLOT(clearRecords()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(exitGame()));

    aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("About &Qt"), this);
    connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    gameMenu = menuBar()->addMenu(tr("&Game"));
    gameMenu->addAction(newAction);
    gameMenu->addAction(showRecordsAction);
    gameMenu->addAction(clearRecordsAction);
    gameMenu->addSeparator();
    gameMenu->addAction(exitAction); 

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

}

void MainWindow::createContextMenu()
{
    addAction(newAction);
    addAction(exitAction);
}

void MainWindow::createStatusBar()
{
    scoreLabel = new QLabel("");
    scoreLabel->setAlignment(Qt::AlignHCenter);
    scoreLabel->setMinimumSize(scoreLabel->sizeHint());

    statusLabel = new QLabel(" Press \"Space\" or \"p\" to pause, \"Esc\" to exit");
    statusLabel->setAlignment(Qt::AlignHCenter);
    statusLabel->setIndent(3);

    statusBar()->addWidget(scoreLabel); 
    statusBar()->addWidget(statusLabel, 1); 

    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::gray);
    statusBar()->setAutoFillBackground(true);
    statusBar()->setPalette(pal);
}

void MainWindow::newGame()
{
    /* FIXME: actually, newGame() should have something else to do other than startGame() */
    startGame();
}

void MainWindow::exitGame()
{
    //int rtval = QMessageBox::warning(this, tr("Greedy Snake Game"),
    //                        tr("Are you sure you want to exit?"),
    //                        QMessageBox::Yes | QMessageBox::No);
    //if(rtval == QMessageBox::Yes)
    close();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Greedy Snake Game"),
                       tr("<h2>Greedy Snake 0.1</h2>"
                        "<p>Author: Joey Zhang</p>"
                        "<p>Email: <a href=\"mailto:happyjoey.zhang@gmail.com\">"
                        "happyjoey.zhang@gmail.com</a></p>")); 
}

void MainWindow::aboutQt()
{
    QApplication::aboutQt();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

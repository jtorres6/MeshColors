#include "loadingwindow.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QMovie>

LoadingWindow::LoadingWindow()
{
    QVBoxLayout *videoLayout = new QVBoxLayout(this);
    QLabel *videoText = new QLabel("Loaging shaders...");
    QLabel *lbl = new QLabel(this);
    QString moviePath = QString(PROJECT_PATH) + "models/loading.gif";
    setPixmap(QPixmap("models/loading.gif"));
    movie = new QMovie(moviePath);
    lbl->setMovie(movie);
    lbl->show();
    lbl->setFixedWidth(400);
    lbl->setFixedHeight(300);
    videoLayout->addWidget(lbl);
    videoLayout->addStretch();
    videoLayout->addWidget(videoText);
    movie->start();
}

void LoadingWindow::mousePressEvent(QMouseEvent *event)
{

}

void LoadingWindow::setVisible(bool visible)
{
    visible ? movie->start() : movie->stop();
    QSplashScreen::setVisible(visible);
}

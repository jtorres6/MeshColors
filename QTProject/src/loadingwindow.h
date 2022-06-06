#ifndef LOADINGWINDOW_H
#define LOADINGWINDOW_H

#include <QSplashScreen>
#include <QMainWindow>
#include <QWidget>

class LoadingWindow : public QSplashScreen
{
    Q_OBJECT
public:
    LoadingWindow();

    QMovie *movie;
    virtual void mousePressEvent(QMouseEvent *event) override;

public Q_SLOTS:
    virtual void setVisible(bool visible);

};

#endif // LOADINGWINDOW_H

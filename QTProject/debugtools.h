#ifndef DEBUGTOOLS_H
#define DEBUGTOOLS_H

#include <QLabel>

class DebugTools
{
public:

    const static DebugTools* GetInstance();

    void static DrawDebugString(QWidget* parent, const QString& InText, const int InPosX, const int InPosY, const int InHeight, const int InWidht, const QString& InColor);
private:
    QLabel *label;
    DebugTools();
};

#endif // DEBUGTOOLS_H

#ifndef DEBUGTOOLS_H
#define DEBUGTOOLS_H

#include <QLabel>
#include <QElapsedTimer>

struct FDebugLabel
{
    QLabel* Label;
    float SpawnTime;
    float Duration;

    FDebugLabel()
        : Label(nullptr)
        , SpawnTime(0.0f)
        , Duration(0.0f)
    {
    }

    FDebugLabel(QLabel* InLabel, float InSpawnTime, float InDuration)
        : Label(InLabel)
        , SpawnTime(InSpawnTime)
        , Duration(InDuration)
    {
    }
};

class DebugTools
{
public:

    static DebugTools* GetInstance();

    void static Update();

    void static DrawDebugString(QWidget* parent, const QString& InText, const int InPosX, const int InPosY, const int InHeight, const int InWidht, const QString& InColor, const float InDuration = 0.0f);
private:
    DebugTools();

    QVector<FDebugLabel> labels;
    QElapsedTimer timer;
};

#endif // DEBUGTOOLS_H

#include "debugtools.h"

#include <QLabel>

static DebugTools* instance;

DebugTools::DebugTools()
{
    timer.start();
}


DebugTools* DebugTools::GetInstance()
{
    if(instance == nullptr)
    {
        instance = new DebugTools();
    }

    return instance;
}

void DebugTools::Update()
{
    QVector<FDebugLabel>& DebugLabels = GetInstance()->labels;

    for(int i = 0; i < DebugLabels.size(); i++)
    {
        const float ElapsedTime = float(GetInstance()->timer.elapsed() * 0.001f) - DebugLabels[i].SpawnTime;
        if(ElapsedTime > DebugLabels[i].Duration)
        {
            DebugLabels[i].Label->clear();
            DebugLabels.remove(i);
        }
        else
        {
            DebugLabels[i].Label->show();
        }
    }
}

void DebugTools::DrawDebugString(QWidget* parent, const QString& InText, const int InPosX, const int InPosY, const int InHeight, const int InWidht, const QString& InColor, const float InDuration /*= 0.0f*/)
{
    QLabel* label = new QLabel();

    label->clear();
    label->setParent(parent);
    label->setGeometry(InPosX, InPosY, InHeight, InWidht);
    label->setStyleSheet(InColor);
    label->setText(InText);
    label->show();

    FDebugLabel debugLabel(label, float(GetInstance()->timer.elapsed() * 0.001f), InDuration);
    GetInstance()->labels.append(debugLabel);
}

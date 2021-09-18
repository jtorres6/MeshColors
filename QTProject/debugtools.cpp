#include "debugtools.h"

#include <QLabel>


static DebugTools* instance;

DebugTools::DebugTools()
{
    label = new QLabel();
}


const DebugTools* DebugTools::GetInstance()
{
    if(instance == nullptr)
    {
        instance = new DebugTools();
    }

    return instance;
}
void DebugTools::DrawDebugString(QWidget* parent, const QString& InText, const int InPosX, const int InPosY, const int InHeight, const int InWidht, const QString& InColor)
{
    if(GetInstance() == nullptr)
        return;

    if(instance->label == nullptr)
        return;

    instance->label->clear();
    instance->label->setParent(parent);
    instance->label->setGeometry(InPosX, InPosY, InHeight, InWidht);
    instance->label->setStyleSheet(InColor);
    instance->label->setText(InText);
    instance->label->show();
}

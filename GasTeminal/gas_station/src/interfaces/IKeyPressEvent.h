#pragma once

class QKeyEvent;

class IKeyPressEvent
{
public:
    virtual void keyPressEvent(QKeyEvent* event) = 0;
};

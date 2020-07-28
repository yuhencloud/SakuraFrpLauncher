#pragma once
#ifndef SFLMSGBOX_H
#define SFLMSGBOX_H

#include <QMessageBox>

#include "SFLCommonDefs.h"

class SFLMsgBox : public QMessageBox
{
    Q_OBJECT

public:

    static SFLMsgBox* GetInstance(
    );

    void DeleteInstance(
    );

    void ResetItems(
    );

    void SetBoxType(
        MsgBoxType msg_box_type
    );

private:

    SFLMsgBox();
    ~SFLMsgBox();

private:

    static SFLMsgBox* m_msg_box_instance;

};

#endif // SFLMSGBOX_H

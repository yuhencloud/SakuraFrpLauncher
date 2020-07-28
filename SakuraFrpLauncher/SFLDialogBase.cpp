#include "SFLDialogBase.h"

SFLDialogBase::SFLDialogBase(QWidget *parent)
    : QDialog(parent)
{
    
}

SFLDialogBase::~SFLDialogBase()
{

}

void SFLDialogBase::keyPressEvent(
    QKeyEvent* e
) {
    switch (e->key()) {
        case Qt::Key_Escape: {
            break;
        }
        default: {
            QDialog::keyPressEvent(e);
        }
    }
}
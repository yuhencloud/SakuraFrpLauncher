#pragma once
#ifndef SFLGROUPTABWIDGET_H
#define SFLGROUPTABWIDGET_H

#include <QTabWidget>

class SFLGroupTabWidget : public QTabWidget
{
    Q_OBJECT

public:

    SFLGroupTabWidget(QWidget *parent = 0);
    ~SFLGroupTabWidget();

public:

    void InitTabWidget(
    );

private:



};

#endif // SFLGROUPTABWIDGET_H

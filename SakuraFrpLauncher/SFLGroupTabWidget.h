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

    void InitGroupTabWidget(
    );

private:



};

#endif // SFLGROUPTABWIDGET_H

#pragma once
#ifndef SFLTUNNELWIDGET_H
#define SFLTUNNELWIDGET_H

#include <QWidget>

#include "SFLCommonDefs.h"

class QCheckBox;
class SFLTunnelTableWidget;

class SFLTunnelWidget : public QWidget
{
    Q_OBJECT

public:

    SFLTunnelWidget(QWidget *parent = 0);
    ~SFLTunnelWidget();

public:

    void InitTunnelWidget(
        const NodeItemInfo& node_item_info
    );

    void GetNodeItemInfo(
        NodeItemInfo& node_item_info
    );

    void TerminateAllProcess(
    );

private slots:

    void OnSelectAllCheckBoxStateChanged(
        int state
    );

    void OnStartSelectedBtnClicked(
    );

    void OnStopSelectedBtnClicked(
    );

private:

    QCheckBox* m_select_all_check_box;
    SFLTunnelTableWidget* m_tunnel_table_widget;

};

#endif // SFLTUNNELWIDGET_H

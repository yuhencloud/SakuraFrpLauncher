#include "SFLGroupTabWidget.h"

#include "SFLTunnelTableWidget.h"
#include "SFLDBMgr.h"

SFLGroupTabWidget::SFLGroupTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
}

SFLGroupTabWidget::~SFLGroupTabWidget()
{

}

void SFLGroupTabWidget::InitGroupTabWidget(
) {
    // Çå¿Õµ±Ç°tabwidget
    for (int i = this->count() - 1; i >= 0; --i) {
        SFLTunnelTableWidget* tunnel_table_widget = dynamic_cast<SFLTunnelTableWidget*>(this->widget(i));
        if (nullptr != tunnel_table_widget) {
            delete tunnel_table_widget;
            tunnel_table_widget = nullptr;
        }
        this->removeTab(i);
    }

    QSqlDatabase db = SFLDBMgr::GetInstance()->GetSqlConn();
	QVector<NodeItemInfo> node_item_info_list;
	SFLDBMgr::GetInstance()->GetNodeInfoList(db, node_item_info_list);
	for (auto node_item_info : node_item_info_list) {
        SFLTunnelTableWidget* tunnel_table_widget = new SFLTunnelTableWidget(this);
        tunnel_table_widget->InitTunnelTableWidget(node_item_info);
        this->addTab(tunnel_table_widget, node_item_info.node_name);
    }
    SFLDBMgr::GetInstance()->GiveBackSqlConn(db);
}
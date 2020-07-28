#include "SakuraFrpLauncher.h"

#include <QtWidgets/QApplication>
#include <QDir>
#include <QSharedMemory>

#include "SFLDBMgr.h"
#include "SFLMsgBox.h"

void SetupQssFile(
) {
    QFile qss_file(":/Resources/qss/SakuraFrpLauncher.qss");
    if (qss_file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(qss_file.readAll());  
        qApp->setStyleSheet(qss);  
        qss_file.close();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置当前目录，之后使用相对路径
    QString app_dir_path = QApplication::applicationDirPath();
    QString native_app_dir_path =QDir::toNativeSeparators(app_dir_path);
    QDir::setCurrent(native_app_dir_path);

    // 加载库路径
    qApp->addLibraryPath("plugins");

    // 加载样式
    SetupQssFile();

    // 只能启动一个实例
    QSharedMemory share_mem(QApplication::applicationFilePath() + "SakuraFrpLauncher");
    if (!share_mem.create(1)) {
        SFLMsgBox::GetInstance()->SetBoxType(e_information_type_ok);
        SFLMsgBox::GetInstance()->setText("不要重复开启Sakura Frp客户端，若想运行多个客户端请将软件复制到其他目录下再运行");
        SFLMsgBox::GetInstance()->exec();
        return 0;
    }

    // 创建/打开本地数据库
    SFLDBMgr::GetInstance()->OpenLocalDB("data.db");

    SakuraFrpLauncher w;
    w.show();
    return a.exec();
}

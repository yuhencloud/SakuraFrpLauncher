#include "SFLLoadingDlg.h"
#include <QLabel>
#include <QMovie>
#include <QBoxLayout>

SFLLoadingDlg::SFLLoadingDlg(QWidget *parent) :
    SFLDialogBase(parent)
{
    this->setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    this->setModal(true);
    this->setFixedSize(200, 150);
    this->setWindowTitle(QString::fromLocal8Bit("提示"));
    this->setObjectName("SFLLoadingDlg");

    QVBoxLayout* main_v_layout = new QVBoxLayout(this);
    main_v_layout->setMargin(0);

    m_text_label = new QLabel(this);
    m_text_label->setObjectName("SFLLoadingDlg_text_label");
    m_text_label->setAlignment(Qt::AlignCenter);
    m_text_label->setText(QString::fromLocal8Bit("请等待..."));

    QLabel* loading_label = new QLabel(this);

    QMovie* login_movie = new QMovie(this);
    login_movie->setFileName(":/Resources/images/sfl-inprogress.gif");
    loading_label->setMovie(login_movie);
    loading_label->setAlignment(Qt::AlignCenter);
    login_movie->start();

    main_v_layout->addSpacing(30);
    main_v_layout->addWidget(loading_label);
    main_v_layout->addWidget(m_text_label);
    main_v_layout->addSpacing(20);
    this->setLayout(main_v_layout);
}

SFLLoadingDlg::~SFLLoadingDlg()
{
}

void SFLLoadingDlg::SetText( 
    const QString& text
) {
    m_text_label->setText(text);
}
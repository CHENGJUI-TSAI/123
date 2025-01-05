#include "Qtproject.h"
#include "ui_Qtproject.h"
#include <QResizeEvent>
#include <QPushButton>
#include <QPixmap>
#include <QPalette>

Qtproject::Qtproject(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::Qtproject)
{
    ui->setupUi(this);
    setWindowTitle("五子棋小遊戲");

    // 設置背景圖片
    QPixmap pixmap(":/icons/images/background.png");
    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    setPalette(palette);

    // 配置按鈕
    setButtonProperties(ui->pushButton_1, ":/icons/images/button1.png", QSize(200, 120));
    setButtonProperties(ui->pushButton_2, ":/icons/images/button2.png", QSize(200, 120));
    setButtonProperties(ui->pushButton_3, ":/icons/images/button3.png", QSize(200, 120));

    // 按鈕點擊事件
    connect(ui->pushButton_1, &QPushButton::clicked, this, &Qtproject::onButton1Clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &Qtproject::onButton2Clicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &Qtproject::onButton3Clicked);
}

Qtproject::~Qtproject()
{
    delete ui;
}

void Qtproject::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    QPixmap pixmap(":/icons/images/background.png");
    QPalette palette;
    palette.setBrush(backgroundRole(),
                     QBrush(pixmap.scaled(this->size(),
                                          Qt::IgnoreAspectRatio,
                                          Qt::SmoothTransformation)));
    setPalette(palette);
}

void Qtproject::onButton1Clicked()
{
    // 本地對戰新視窗（將 parent 設為 nullptr，使其獨立顯示）
    auto localGameWindow = new LocalGame(nullptr);
    localGameWindow->setAttribute(Qt::WA_DeleteOnClose); // 視窗關閉時自動刪除
    localGameWindow->setWindowTitle("本地對戰");
    localGameWindow->show();
}

void Qtproject::onButton2Clicked()
{
    // AI 對戰新視窗
    auto aiGameWindow = new AIvsPlayer(this); // 若想獨立，可改為 nullptr
    aiGameWindow->setAttribute(Qt::WA_DeleteOnClose);
    aiGameWindow->setWindowTitle("AI 對戰");
    aiGameWindow->show();
}

void Qtproject::onButton3Clicked()
{
    // 網絡對戰新視窗
    auto networkGameWindow = new NetworkGame(this); // 若想獨立，可改為 nullptr
    networkGameWindow->setAttribute(Qt::WA_DeleteOnClose);
    networkGameWindow->setWindowTitle("網絡對戰");
    networkGameWindow->show();
}

void Qtproject::setButtonProperties(QPushButton *button, const QString &iconPath, const QSize &iconSize)
{
    button->setIcon(QIcon(iconPath));
    button->setIconSize(iconSize);
}

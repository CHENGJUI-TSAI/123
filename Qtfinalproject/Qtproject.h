#ifndef QTPROJECT_H
#define QTPROJECT_H

#include <QMainWindow>
#include <QPushButton>
#include <QScopedPointer>
#include "LocalGame.h"
#include "AIvsPlayer.h"
#include "NetworkGame.h"

namespace Ui {
class Qtproject;
}

class Qtproject : public QMainWindow
{
    Q_OBJECT

public:
    explicit Qtproject(QWidget *parent = nullptr);
    ~Qtproject();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onButton1Clicked(); // 本地對戰
    void onButton2Clicked(); // AI 對戰
    void onButton3Clicked(); // 網絡對戰

private:
    Ui::Qtproject *ui;

    void setButtonProperties(QPushButton *button, const QString &iconPath, const QSize &iconSize); // 設置按鈕屬性
};

#endif // QTPROJECT_H

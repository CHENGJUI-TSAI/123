#ifndef LOCALGAME_H
#define LOCALGAME_H

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QStack>
#include <QPushButton>
#include <QLabel>
#include <QPixmap> // 新增：載入圖片用

class LocalGame : public QWidget
{
    Q_OBJECT

public:
    explicit LocalGame(QWidget *parent = nullptr);
    void startGame();
    void undoLastMove(); // 悔棋

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    // 棋譜資料
    QVector<QVector<int>> board;         // 0: 空, 1: 黑棋, 2: 白棋
    QStack<QPair<int, int>> moveHistory; // 移動歷史紀錄
    int currentPlayer;                   // 當前玩家 (1: 黑棋, 2: 白棋)

    // 棋盤參數
    static const int BOARD_SIZE = 15;    // 15x15
    static const int CELL_SIZE = 48;     // 每個交叉點的間距 (可以依您的木質背景做微調)

    // 背景圖片
    QPixmap boardImage;                  // 木質棋盤背景圖

    // 右側控制元件
    QPushButton *undoButton;
    QLabel *currentPlayerLabel;

    // 內部函式
    bool checkWin(int x, int y);
    void updateCurrentPlayerLabel();
};

#endif // LOCALGAME_H

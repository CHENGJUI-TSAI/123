#ifndef AIVSPLAYER_H
#define AIVSPLAYER_H

#include <QMainWindow>
#include <QVector>
#include <QPair>
#include <QStack>
#include <QPixmap>

class QComboBox;
class QPushButton;
class QLabel;

class AIvsPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit AIvsPlayer(QWidget *parent = nullptr);
    ~AIvsPlayer();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onUndoClicked();          // 悔棋按鈕
    void onDifficultyChanged(int); // 切換 AI 難度

private:
    // 棋盤參數
    static const int BOARD_SIZE = 15;
    static const int CELL_SIZE  = 48;

    // 棋盤資料 (0: 空, 1: 黑(玩家), 2: 白(AI))
    QVector<QVector<int>> board;

    // 下子紀錄
    struct Move {
        int x;
        int y;
        int stone; // 1 or 2
    };
    QStack<Move> moveHistory;

    // 當前玩家 (1=黑, 2=白)
    int currentPlayer;

    // 棋盤背景圖
    QPixmap boardImage;

    // UI 元件
    QComboBox  *difficultyCombo;
    QPushButton *undoButton;
    QLabel     *infoLabel;

    // AI 難度
    enum Difficulty {
        Easy   = 0,  // 初階：隨機
        Medium = 1,  // 中階：單步簡易評分
        Hard   = 2   // 高階：雙層搜索
    };
    Difficulty currentDifficulty;

    // ====== 核心流程 ======
    void setupUI();
    void startGame();
    bool checkWin(int x, int y);       // 判斷五子連線
    void doAIMove();                   // AI 下子
    void undoOneRound();               // 悔棋(回退玩家 + AI)

    // ====== AI 幫手函式 ======
    // 專門檢查「玩家(黑=1) 是否有一步就能連5」的空位，AI 必須先擋
    QPair<int,int> findBlockMoveForOpponent();

    // Easy：隨機
    QPair<int,int> randomPick();

    // Medium：單步簡易評分 (從前三名中隨機挑)
    QPair<int,int> findBestMoveMedium();
    int evaluateMove(int x, int y, int stone);

    // Hard：雙層搜索 (Depth=2)
    QPair<int,int> findBestMoveHard();
    int evaluateBoard();  // 全盤評分 (供 Hard 使用)
};

#endif // AIVSPLAYER_H

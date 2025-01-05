#include "LocalGame.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>

LocalGame::LocalGame(QWidget *parent)
    : QWidget(parent),
    currentPlayer(1),
    undoButton(nullptr),
    currentPlayerLabel(nullptr)
{
    // 假設固定視窗大小：720x720 + 右側120 = 840x720
    setFixedSize(BOARD_SIZE * CELL_SIZE + 120, BOARD_SIZE * CELL_SIZE);

    // 初始化棋盤資料
    board = QVector<QVector<int>>(BOARD_SIZE, QVector<int>(BOARD_SIZE, 0));

    // 載入木質棋盤圖片（可用 Qt Resource 或絕對路徑 / 相對路徑）
    // 這裡假設您已在 .qrc 註冊了 ":/images/wood_board.png"
    boardImage.load(":/icons/images/board.png");

    // 右側的標籤與按鈕（此處略作示範）
    currentPlayerLabel = new QLabel(this);
    currentPlayerLabel->setGeometry(BOARD_SIZE * CELL_SIZE + 10, 20, 100, 30);
    currentPlayerLabel->setAlignment(Qt::AlignCenter);

    undoButton = new QPushButton(tr("悔棋"), this);
    undoButton->setGeometry(BOARD_SIZE * CELL_SIZE + 10, 60, 100, 30);
    connect(undoButton, &QPushButton::clicked, this, &LocalGame::undoLastMove);

    updateCurrentPlayerLabel();
}

void LocalGame::startGame()
{
    board.fill(QVector<int>(BOARD_SIZE, 0));
    currentPlayer = 1;
    moveHistory.clear();
    update();
    updateCurrentPlayerLabel();
}

void LocalGame::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    // 1) 如果視窗大小與圖檔不一致，先做縮放
    //    讓圖片貼滿左側棋盤區域 (寬高 = BOARD_SIZE * CELL_SIZE)
    //    若您想保持圖片比例，可用 KeepAspectRatio, 這裡用 IgnoreAspectRatio 直接填滿
    QPixmap scaledBoard = boardImage.scaled(
        BOARD_SIZE * CELL_SIZE,
        BOARD_SIZE * CELL_SIZE,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    // 2) 先畫整個背景：可以直接把整個視窗塗成黑或其他顏色
    //    不過若想讓右側顯示相同木紋，也可以用同張圖切一部分，或乾脆直接用單色等
    //    這裡示範：先整個塗成深色
    painter.fillRect(rect(), QColor(60, 60, 60));

    // 3) 在左側 (0,0)~(720,720) 貼上縮放後的木質棋盤背景
    painter.drawPixmap(0, 0, scaledBoard);

    // 4) 在其上繪製棋子
    //    注意：假設您的木質圖檔本身就含有 15x15 的線與星位
    //    這時候就不必再自己畫線
    //    只要確保每一顆棋子的位置能「對準」圖上的交叉點
    //    也就是 x * CELL_SIZE + CELL_SIZE/2, y * CELL_SIZE + CELL_SIZE/2
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            int stone = board[row][col];
            if (stone == 0) continue; // 空格

            if (stone == 1) {
                painter.setBrush(Qt::black);
            } else if (stone == 2) {
                painter.setBrush(Qt::white);
            }

            // 棋子中心對準 (col, row) 的交叉點
            painter.drawEllipse(
                col * CELL_SIZE + CELL_SIZE / 2 - 12,
                row * CELL_SIZE + CELL_SIZE / 2 - 12,
                24, 24
                );
        }
    }
}

void LocalGame::mousePressEvent(QMouseEvent *event)
{
    // 如果點到右側 (>= 720px) 就不處理下棋
    if (event->x() >= BOARD_SIZE * CELL_SIZE) {
        return;
    }

    // 計算點擊到的 (x, y) 索引
    int col = (event->x() - CELL_SIZE / 2) / CELL_SIZE;
    int row = (event->y() - CELL_SIZE / 2) / CELL_SIZE;

    // 邊界 & 空格檢查
    if (col >= 0 && col < BOARD_SIZE &&
        row >= 0 && row < BOARD_SIZE &&
        board[row][col] == 0)
    {
        board[row][col] = currentPlayer;
        moveHistory.push({col, row});

        // 是否勝利
        if (checkWin(col, row)) {
            QMessageBox::information(
                this, tr("遊戲結束"),
                currentPlayer == 1 ? tr("黑棋獲勝！") : tr("白棋獲勝！")
                );
            startGame();
            return;
        }

        // 切換玩家
        currentPlayer = 3 - currentPlayer;
        update();
        updateCurrentPlayerLabel();
    }
}

bool LocalGame::checkWin(int x, int y)
{
    // 4個方向：水平、垂直、斜向下、斜向上
    const int directions[4][2] = {
        {1, 0},
        {0, 1},
        {1, 1},
        {1, -1}
    };

    for (auto &dir : directions) {
        int count = 1;

        // 正向
        for (int i = 1; i < 5; ++i) {
            int nx = x + i * dir[0];
            int ny = y + i * dir[1];
            if (nx >= 0 && nx < BOARD_SIZE &&
                ny >= 0 && ny < BOARD_SIZE &&
                board[ny][nx] == currentPlayer)
            {
                ++count;
            } else {
                break;
            }
        }

        // 反向
        for (int i = 1; i < 5; ++i) {
            int nx = x - i * dir[0];
            int ny = y - i * dir[1];
            if (nx >= 0 && nx < BOARD_SIZE &&
                ny >= 0 && ny < BOARD_SIZE &&
                board[ny][nx] == currentPlayer)
            {
                ++count;
            } else {
                break;
            }
        }

        if (count >= 5) {
            return true;
        }
    }
    return false;
}

void LocalGame::undoLastMove()
{
    if (!moveHistory.isEmpty()) {
        auto lastMove = moveHistory.pop();
        board[lastMove.second][lastMove.first] = 0;
        currentPlayer = 3 - currentPlayer;
        update();
        updateCurrentPlayerLabel();
    }
}

void LocalGame::updateCurrentPlayerLabel()
{
    if (!currentPlayerLabel) return;
    currentPlayerLabel->setText(
        currentPlayer == 1 ? tr("黑棋回合") : tr("白棋回合")
        );
}

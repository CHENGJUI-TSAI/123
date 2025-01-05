#include "AIvsPlayer.h"

#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <algorithm> // std::sort

AIvsPlayer::AIvsPlayer(QWidget *parent)
    : QMainWindow(parent),
    currentPlayer(1),
    currentDifficulty(Easy),
    difficultyCombo(nullptr),
    undoButton(nullptr),
    infoLabel(nullptr)
{
    // 固定視窗大小：15x15 棋盤(15*48=720) + 下方 60px 放按鈕/選單
    setFixedSize(BOARD_SIZE * CELL_SIZE, BOARD_SIZE * CELL_SIZE + 60);

    // 建立棋盤
    board = QVector<QVector<int>>(BOARD_SIZE, QVector<int>(BOARD_SIZE, 0));

    // 載入木質棋盤圖 (請確保 .qrc 中有 "board.png" 或改用實際路徑)
    boardImage.load(":/icons/images/board.png");

    // 設置介面 & 開始遊戲
    setupUI();
    startGame();
}

AIvsPlayer::~AIvsPlayer()
{
}

void AIvsPlayer::setupUI()
{
    // 難度選單
    difficultyCombo = new QComboBox(this);
    difficultyCombo->addItem("初階 (Easy)",   Easy);
    difficultyCombo->addItem("中階 (Medium)", Medium);
    difficultyCombo->addItem("高階 (Hard)",   Hard);
    // 放在下方 60px 高度的區域
    difficultyCombo->setGeometry(10, BOARD_SIZE * CELL_SIZE + 10, 100, 30);
    connect(difficultyCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onDifficultyChanged(int)));

    // 悔棋按鈕
    undoButton = new QPushButton("悔棋", this);
    undoButton->setGeometry(120, BOARD_SIZE * CELL_SIZE + 10, 80, 30);
    connect(undoButton, &QPushButton::clicked, this, &AIvsPlayer::onUndoClicked);

    // Label 提示訊息
    infoLabel = new QLabel(this);
    infoLabel->setGeometry(210, BOARD_SIZE * CELL_SIZE + 10, 480, 30);
    infoLabel->setText("黑棋(玩家)回合");
}

void AIvsPlayer::startGame()
{
    // 清空棋盤
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            board[r][c] = 0;
        }
    }
    // 歷史清除
    moveHistory.clear();

    // 玩家(黑)先下
    currentPlayer = 1;
    infoLabel->setText("黑棋(玩家)回合");

    update();
}

/* ============= 繪製畫面：先貼背景圖，再畫棋子 ============= */
void AIvsPlayer::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    // 先將整個視窗背景填成深色，避免圖片縮放出現空白邊
    painter.fillRect(rect(), QColor(50,50,50));

    // 將木質棋盤圖縮放到 (720x720) 貼在左上角
    QPixmap scaledBoard = boardImage.scaled(
        BOARD_SIZE * CELL_SIZE,
        BOARD_SIZE * CELL_SIZE,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );
    painter.drawPixmap(0, 0, scaledBoard);

    // 在其上畫棋子
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            int stone = board[y][x];
            if (stone == 0) continue;

            if (stone == 1) painter.setBrush(Qt::black);
            else            painter.setBrush(Qt::white);

            // 棋子直徑 24，可視自己需求微調
            painter.drawEllipse(
                x * CELL_SIZE + CELL_SIZE/2 - 12,
                y * CELL_SIZE + CELL_SIZE/2 - 12,
                24, 24
                );
        }
    }
}

/* ============= 滑鼠點擊：玩家(黑)回合才處理 ============= */
void AIvsPlayer::mousePressEvent(QMouseEvent *event)
{
    if (currentPlayer != 1) return;  // 只有黑棋回合可點

    int col = (event->x() - CELL_SIZE/2) / CELL_SIZE;
    int row = (event->y() - CELL_SIZE/2) / CELL_SIZE;

    if (col >= 0 && col < BOARD_SIZE &&
        row >= 0 && row < BOARD_SIZE &&
        board[row][col] == 0)
    {
        // 玩家(黑)落子
        board[row][col] = 1;
        moveHistory.push({col, row, 1});

        // 檢查勝利
        if (checkWin(col, row)) {
            update();
            QMessageBox::information(this, "遊戲結束", "黑棋獲勝！");
            startGame();
            return;
        }

        // 換 AI
        currentPlayer = 2;
        infoLabel->setText("白棋(AI)回合");
        update();

        doAIMove();
    }
}

/* ============= AI 下子流程 ============= */
void AIvsPlayer::doAIMove()
{
    // === 1) 若整盤下滿(可留可刪：若您想保留「和局」檢查就留) ===
    int emptyCount = 0;
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (board[r][c] == 0) emptyCount++;
        }
    }
    if (emptyCount == 0) {
        QMessageBox::information(this, "遊戲結束", "和局！");
        startGame();
        return;
    }

    // === 2) 檢查「玩家下一手就能贏」的點 (AI 必須先擋) ===
    QPair<int,int> mustBlock = findBlockMoveForOpponent();
    if (mustBlock.first >= 0) {
        // 有必須阻擋的點 => 直接下在該處
        int col = mustBlock.first;
        int row = mustBlock.second;
        board[row][col] = 2;
        moveHistory.push({col, row, 2});

        // 檢查 AI 是否(意外)連到五
        if (checkWin(col, row)) {
            update();
            QMessageBox::information(this, "遊戲結束", "白棋獲勝！");
            startGame();
            return;
        }

        // 回到玩家
        currentPlayer = 1;
        infoLabel->setText("黑棋(玩家)回合");
        update();
        return;
    }

    // === 3) 若沒必擋點，再執行原本策略 (Easy / Medium / Hard) ===
    QPair<int,int> chosenMove;
    switch (currentDifficulty) {
    case Easy:
        chosenMove = randomPick();
        break;
    case Medium:
        chosenMove = findBestMoveMedium();
        break;
    case Hard:
        chosenMove = findBestMoveHard();
        break;
    }

    int col = chosenMove.first;
    int row = chosenMove.second;
    if (col < 0 || row < 0) {
        // 若找不到可下(理論上不會)，當和局
        QMessageBox::information(this, "遊戲結束", "和局！");
        startGame();
        return;
    }

    // AI(白)落子
    board[row][col] = 2;
    moveHistory.push({col, row, 2});

    // 檢查勝利
    if (checkWin(col, row)) {
        update();
        QMessageBox::information(this, "遊戲結束", "白棋獲勝！");
        startGame();
        return;
    }

    // 回到玩家
    currentPlayer = 1;
    infoLabel->setText("黑棋(玩家)回合");
    update();
}

/* ============= 專門用來「找必擋點」的函式 =============
   假設玩家是 1，AI 是 2。
   偵測所有空位，若「假設玩家下在該處就可馬上連5」，則回傳該處。
   若找不到則回傳 {-1, -1}.
*/
QPair<int,int> AIvsPlayer::findBlockMoveForOpponent()
{
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (board[r][c] == 0) {
                // 試著讓玩家(1) 下在 (c, r)，看是否馬上贏
                board[r][c] = 1;
                bool wouldWin = checkWin(c, r);
                board[r][c] = 0; // 撤銷

                if (wouldWin) {
                    // 這個位置如果玩家下，就可馬上 5 連
                    // -> AI 必須先擋
                    return {c, r};
                }
            }
        }
    }
    return {-1, -1};
}

/* ============= Easy: 隨機下子 ============= */
QPair<int,int> AIvsPlayer::randomPick()
{
    QVector<QPair<int,int>> empties;
    empties.reserve(BOARD_SIZE * BOARD_SIZE);

    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (board[r][c] == 0) {
                empties.push_back({c, r});
            }
        }
    }

    if (empties.isEmpty()) {
        return {-1, -1};
    }

    int idx = QRandomGenerator::global()->bounded(empties.size());
    return empties[idx];
}

/* ============= Medium: 單步評分 (攻 + 防) ============= */
QPair<int,int> AIvsPlayer::findBestMoveMedium()
{
    struct Candidate {
        int x;
        int y;
        int score;
    };
    QVector<Candidate> candidates;

    // 收集空位
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (board[r][c] == 0) {
                // AI(2) 的分 + 防玩家(1) 的分
                int aiScore     = evaluateMove(c, r, 2);
                int playerScore = evaluateMove(c, r, 1);
                int total       = aiScore + playerScore;
                candidates.push_back({c, r, total});
            }
        }
    }

    if (candidates.isEmpty()) {
        return {-1, -1};
    }

    // 分數排序(大到小)
    std::sort(candidates.begin(), candidates.end(),
              [](auto &a, auto &b){
                  return a.score > b.score;
              });

    // 從前3名中隨機選
    int pickCount = qMin(3, candidates.size());
    int idx = QRandomGenerator::global()->bounded(pickCount);
    return {candidates[idx].x, candidates[idx].y};
}

/* ============= 單步評分(假設在(x,y)落 stone)，計算可連線長度 ============= */
int AIvsPlayer::evaluateMove(int x, int y, int stone)
{
    static const int directions[4][2] = {
        {1,0}, {0,1}, {1,1}, {1,-1}
    };
    int total = 0;
    // 假設把 (x,y) 當成已經落子
    for (auto &dir : directions) {
        int count = 1;

        // 正向
        for (int i = 1; i < 5; ++i) {
            int nx = x + i*dir[0];
            int ny = y + i*dir[1];
            if (nx<0||nx>=BOARD_SIZE||ny<0||ny>=BOARD_SIZE) break;
            if (board[ny][nx] == stone) count++;
            else break;
        }

        // 反向
        for (int i = 1; i < 5; ++i) {
            int nx = x - i*dir[0];
            int ny = y - i*dir[1];
            if (nx<0||nx>=BOARD_SIZE||ny<0||ny>=BOARD_SIZE) break;
            if (board[ny][nx] == stone) count++;
            else break;
        }

        // 簡單給分 (可再調整比重)
        switch (count) {
        case 2: total += 10;   break;
        case 3: total += 50;   break;
        case 4: total += 500;  break;
        case 5: total += 9999; break;
        default: total += count;
        }
    }
    return total;
}

/* ============= Hard: 雙層搜索 =============
   第 1 步：AI 嘗試所有空位 -> 下子
   第 2 步：模擬玩家(1)最佳反擊 -> 對 AI 來說評分最糟
   選能讓「最糟分數」最高的步。
*/
QPair<int,int> AIvsPlayer::findBestMoveHard()
{
    QVector<QPair<int,int>> empties;
    empties.reserve(BOARD_SIZE * BOARD_SIZE);

    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (board[r][c] == 0) {
                empties.push_back({c, r});
            }
        }
    }
    if (empties.isEmpty()) {
        return {-1, -1};
    }

    int bestValue = -999999;
    QPair<int,int> bestMove = {-1, -1};

    // AI(2) 下在 (x, y)
    for (auto &mv : empties) {
        int x = mv.first;
        int y = mv.second;

        board[y][x] = 2; // 下子

        // 若已連五 => 直接拿這步(最快贏)
        if (checkWin(x, y)) {
            board[y][x] = 0;
            return {x, y};
        }

        // 模擬「玩家(1)」回應
        int worstCaseForAI = 999999;
        for (auto &opp : empties) {
            int ox = opp.first;
            int oy = opp.second;
            if (board[oy][ox] != 0) continue; // 早被占了

            board[oy][ox] = 1;
            int scoreAfterOpp = evaluateBoard();
            if (checkWin(ox, oy)) {
                scoreAfterOpp = -999999;
            }
            board[oy][ox] = 0;

            if (scoreAfterOpp < worstCaseForAI) {
                worstCaseForAI = scoreAfterOpp;
            }
        }

        board[y][x] = 0; // 撤銷

        // AI 要最大化「最糟分數」
        if (worstCaseForAI > bestValue) {
            bestValue = worstCaseForAI;
            bestMove  = {x, y};
        }
    }
    return bestMove;
}

/* ============= 全盤評分(供 Hard 搜索) ============= */
int AIvsPlayer::evaluateBoard() {
    int score = 0;
    static const int directions[4][2] = {
        {1, 0}, {0, 1}, {1, 1}, {1, -1}
    };

    // 中心區域的權重矩陣 (可調整)
    static const int weight[BOARD_SIZE][BOARD_SIZE] = {
        {3, 4, 5, 7, 7, 5, 4, 3},
        {4, 6, 8, 9, 9, 8, 6, 4},
        {5, 8, 11, 13, 13, 11, 8, 5},
        {7, 9, 13, 16, 16, 13, 9, 7},
        {7, 9, 13, 16, 16, 13, 9, 7},
        {5, 8, 11, 13, 13, 11, 8, 5},
        {4, 6, 8, 9, 9, 8, 6, 4},
        {3, 4, 5, 7, 7, 5, 4, 3}
    };

    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            int stone = board[y][x];
            if (stone == 0) continue; // 空格

            for (auto &dir : directions) {
                int count = 1;
                int openEnds = 0;
                bool doubleThreat = false; // 判斷雙威脅

                // 正向檢查
                for (int i = 1; i < 5; ++i) {
                    int nx = x + i * dir[0];
                    int ny = y + i * dir[1];
                    if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) break;
                    if (board[ny][nx] == stone) count++;
                    else if (board[ny][nx] == 0) {
                        openEnds++;
                        break;
                    } else break;
                }

                // 反向檢查
                for (int i = 1; i < 5; ++i) {
                    int nx = x - i * dir[0];
                    int ny = y - i * dir[1];
                    if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) break;
                    if (board[ny][nx] == stone) count++;
                    else if (board[ny][nx] == 0) {
                        openEnds++;
                        break;
                    } else break;
                }

                // 判斷是否形成雙威脅
                if (count >= 3 && openEnds >= 2) {
                    doubleThreat = true;
                }

                // 根據連接數和空位數判斷分數
                int base = 0;
                if (count >= 5) {
                    base = 99999; // 贏局
                } else if (count == 4 && openEnds >= 1) {
                    base = doubleThreat ? 20000 : 10000; // 雙活四優先
                } else if (count == 3 && openEnds == 2) {
                    base = doubleThreat ? 2000 : 500; // 雙活三
                } else if (count == 3 && openEnds == 1) {
                    base = 100; // 死三
                } else if (count == 2 && openEnds == 2) {
                    base = 50; // 活二
                } else if (count == 2 && openEnds == 1) {
                    base = 10; // 死二
                }

                // 加入區域權重
                base += weight[y][x];

                if (stone == 2) {
                    // AI 加分
                    score += base;
                } else {
                    // 玩家 扣分
                    score -= base;
                }
            }
        }
    }

    return score;
}


/* ============= 判斷五子連線 ============= */
bool AIvsPlayer::checkWin(int x, int y)
{
    int stone = board[y][x];
    static const int directions[4][2] = {
        {1,0}, {0,1}, {1,1}, {1,-1}
    };

    for (auto &dir : directions) {
        int count = 1;

        // 正向
        for (int i = 1; i < 5; ++i) {
            int nx = x + i*dir[0];
            int ny = y + i*dir[1];
            if (nx<0||nx>=BOARD_SIZE||ny<0||ny>=BOARD_SIZE) break;
            if (board[ny][nx] == stone) count++;
            else break;
        }
        // 反向
        for (int i = 1; i < 5; ++i) {
            int nx = x - i*dir[0];
            int ny = y - i*dir[1];
            if (nx<0||nx>=BOARD_SIZE||ny<0||ny>=BOARD_SIZE) break;
            if (board[ny][nx] == stone) count++;
            else break;
        }

        if (count >= 5) {
            return true;
        }
    }
    return false;
}

/* ============= 悔棋：回退「AI + 玩家」各一步 ============= */
void AIvsPlayer::onUndoClicked()
{
    undoOneRound();
}

void AIvsPlayer::undoOneRound()
{
    if (moveHistory.size() < 2) {
        QMessageBox::information(this, "提示", "沒有足夠的步數可以悔棋了");
        return;
    }
    // 先退 AI
    {
        Move last = moveHistory.pop();
        board[last.y][last.x] = 0;
    }
    // 再退 玩家
    {
        Move last = moveHistory.pop();
        board[last.y][last.x] = 0;
    }

    // 回到玩家回合
    currentPlayer = 1;
    infoLabel->setText("黑棋(玩家)回合");
    update();
}

/* ============= 切換 AI 難度 ============= */
void AIvsPlayer::onDifficultyChanged(int index)
{
    QVariant data = difficultyCombo->itemData(index);
    if (data.isValid()) {
        currentDifficulty = static_cast<Difficulty>(data.toInt());
    }

    QString diffName;
    switch (currentDifficulty) {
    case Easy:   diffName = "初階";   break;
    case Medium: diffName = "中階"; break;
    case Hard:   diffName = "高階";   break;
    }
    QMessageBox::information(this, "難度切換", "AI 難度已切換為：" + diffName);
}

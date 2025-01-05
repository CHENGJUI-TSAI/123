#ifndef NETWORKGAME_H
#define NETWORKGAME_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QStack>
#include <QPixmap>

class QPushButton;
class QTextEdit;
class QLabel;

class NetworkGame : public QMainWindow
{
    Q_OBJECT

public:
    explicit NetworkGame(QWidget *parent = nullptr);
    ~NetworkGame();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onHostButtonClicked();
    void onJoinButtonClicked();
    void onSendMessage();
    void onUndoButtonClicked();
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    QLabel     *statusLabel;
    QPushButton *hostButton;
    QPushButton *joinButton;
    QTextEdit   *chatView;
    QTextEdit   *chatInput;
    QPushButton *sendButton;
    QPushButton *undoButton;

    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;

    bool isHost;
    int currentPlayer;

    QVector<QVector<int>> board;
    QStack<QPair<int, int>> moveHistory;
    QPixmap boardImage;

    void resetGame();
    bool checkWin(int x, int y);
    void drawStarPoints(QPainter &painter);
};

#endif // NETWORKGAME_H

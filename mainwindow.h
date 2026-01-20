#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QClipboard>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QDateTime>
#include <deque>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct ClipboardItem {
    QString content;
    QDateTime timestamp;
    bool isImage;
    QPixmap imageData;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static constexpr int MAX_HISTORY_SIZE = 50;

private slots:
    void onClipboardChanged();
    void onItemClicked(QListWidgetItem *item);
    void onItemDoubleClicked(QListWidgetItem *item);
    void clearHistory();
    void deleteSelectedItem();
    void copySelectedItem();
    void toggleWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void setupUI();
    void setupSystemTray();
    void updateHistoryDisplay();
    void addToHistory(const QString &text);
    void addImageToHistory(const QPixmap &image);
    QString truncateText(const QString &text, int maxLength = 80);

    Ui::MainWindow *ui;
    QClipboard *clipboard;
    std::deque<ClipboardItem> history;
    QListWidget *historyList;
    QLabel *statusLabel;
    QPushButton *clearBtn;
    QPushButton *deleteBtn;
    QPushButton *copyBtn;
    QSystemTrayIcon *trayIcon;
    bool ignoreNextChange;
};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QApplication>
#include <QMimeData>
#include <QCloseEvent>
#include <QMessageBox>
#include <QStyle>
#include <QScreen>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QBuffer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ignoreNextChange(false)
    , historyModified(false)
{
    ui->setupUi(this);
    
    setWindowTitle("Clipboard History Manager");
    setMinimumSize(450, 550);
    
    // Set application icon
    QIcon appIcon(":/app_icon.png");
    setWindowIcon(appIcon);
    qApp->setWindowIcon(appIcon);
    
    // Get clipboard instance
    clipboard = QApplication::clipboard();
    
    setupUI();
    setupSystemTray();
    
    // Load saved history
    loadHistory();
    
    // Setup auto-save timer (save every 5 seconds if modified)
    saveTimer = new QTimer(this);
    connect(saveTimer, &QTimer::timeout, this, &MainWindow::saveHistory);
    saveTimer->start(5000);
    
    // Connect clipboard change signal
    connect(clipboard, &QClipboard::dataChanged, this, &MainWindow::onClipboardChanged);
    
    // Center window on screen
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QGuiApplication::primaryScreen()->availableGeometry()
        )
    );
}

MainWindow::~MainWindow()
{
    // Save history before exit
    saveHistory();
    delete ui;
}

QString MainWindow::getDataFilePath()
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    return dataDir + "/clipboard_history.json";
}

QString MainWindow::getImageStoragePath()
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString imageDir = dataDir + "/images";
    QDir().mkpath(imageDir);
    return imageDir;
}

void MainWindow::loadHistory()
{
    QString filePath = getDataFilePath();
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly)) {
        return; // No saved history yet
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        return;
    }
    
    QJsonArray array = doc.array();
    QString imageDir = getImageStoragePath();
    
    for (const QJsonValue &val : array) {
        QJsonObject obj = val.toObject();
        
        ClipboardItem item;
        item.content = obj["content"].toString();
        item.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
        item.isImage = obj["isImage"].toBool();
        
        if (item.isImage) {
            QString imagePath = imageDir + "/" + obj["imageFile"].toString();
            item.imageData = QPixmap(imagePath);
        }
        
        history.push_back(item);
    }
    
    updateHistoryDisplay();
}

void MainWindow::saveHistory()
{
    if (!historyModified) {
        return;
    }
    
    QString filePath = getDataFilePath();
    QString imageDir = getImageStoragePath();
    
    QJsonArray array;
    int imageIndex = 0;
    
    for (const ClipboardItem &item : history) {
        QJsonObject obj;
        obj["content"] = item.content;
        obj["timestamp"] = item.timestamp.toString(Qt::ISODate);
        obj["isImage"] = item.isImage;
        
        if (item.isImage && !item.imageData.isNull()) {
            QString imageName = QString("img_%1_%2.png")
                .arg(item.timestamp.toMSecsSinceEpoch())
                .arg(imageIndex++);
            QString imagePath = imageDir + "/" + imageName;
            item.imageData.save(imagePath, "PNG");
            obj["imageFile"] = imageName;
        }
        
        array.append(obj);
    }
    
    QJsonDocument doc(array);
    QFile file(filePath);
    
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Compact));
        file.close();
        historyModified = false;
    }
}

void MainWindow::setupUI()
{
    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);
    
    // Title and status
    QLabel *titleLabel = new QLabel("📋 Clipboard History", this);
    titleLabel->setStyleSheet(R"(
        QLabel {
            font-size: 24px;
            font-weight: bold;
            color: #2d3748;
            padding: 8px 0;
        }
    )");
    mainLayout->addWidget(titleLabel);
    
    statusLabel = new QLabel(QString("Items: 0 / %1").arg(MAX_HISTORY_SIZE), this);
    statusLabel->setStyleSheet(R"(
        QLabel {
            font-size: 13px;
            color: #718096;
            padding-bottom: 4px;
        }
    )");
    mainLayout->addWidget(statusLabel);
    
    // History list widget
    historyList = new QListWidget(this);
    historyList->setStyleSheet(R"(
        QListWidget {
            background-color: #ffffff;
            border: 2px solid #e2e8f0;
            border-radius: 12px;
            padding: 8px;
            font-size: 14px;
        }
        QListWidget::item {
            background-color: #f7fafc;
            border: 1px solid #e2e8f0;
            border-radius: 8px;
            padding: 12px;
            margin: 4px 0;
            color: #2d3748;
        }
        QListWidget::item:hover {
            background-color: #edf2f7;
            border-color: #cbd5e0;
        }
        QListWidget::item:selected {
            background-color: #4299e1;
            border-color: #3182ce;
            color: white;
        }
    )");
    historyList->setWordWrap(true);
    historyList->setSpacing(2);
    connect(historyList, &QListWidget::itemClicked, this, &MainWindow::onItemClicked);
    connect(historyList, &QListWidget::itemDoubleClicked, this, &MainWindow::onItemDoubleClicked);
    mainLayout->addWidget(historyList, 1);
    
    // Info label
    QLabel *infoLabel = new QLabel("💡 Double-click an item to copy it back to clipboard", this);
    infoLabel->setStyleSheet(R"(
        QLabel {
            font-size: 12px;
            color: #a0aec0;
            font-style: italic;
            padding: 4px 0;
        }
    )");
    infoLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(infoLabel);
    
    // Button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    
    QString buttonStyle = R"(
        QPushButton {
            padding: 10px 20px;
            font-size: 14px;
            font-weight: 600;
            border-radius: 8px;
            border: none;
        }
        QPushButton:hover {
            opacity: 0.9;
        }
        QPushButton:pressed {
            opacity: 0.8;
        }
        QPushButton:disabled {
            background-color: #e2e8f0;
            color: #a0aec0;
        }
    )";
    
    copyBtn = new QPushButton("📄 Copy", this);
    copyBtn->setStyleSheet(buttonStyle + R"(
        QPushButton {
            background-color: #48bb78;
            color: white;
        }
        QPushButton:hover {
            background-color: #38a169;
        }
    )");
    copyBtn->setEnabled(false);
    connect(copyBtn, &QPushButton::clicked, this, &MainWindow::copySelectedItem);
    buttonLayout->addWidget(copyBtn);
    
    deleteBtn = new QPushButton("🗑️ Delete", this);
    deleteBtn->setStyleSheet(buttonStyle + R"(
        QPushButton {
            background-color: #fc8181;
            color: white;
        }
        QPushButton:hover {
            background-color: #f56565;
        }
    )");
    deleteBtn->setEnabled(false);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteSelectedItem);
    buttonLayout->addWidget(deleteBtn);
    
    clearBtn = new QPushButton("🧹 Clear All", this);
    clearBtn->setStyleSheet(buttonStyle + R"(
        QPushButton {
            background-color: #ed8936;
            color: white;
        }
        QPushButton:hover {
            background-color: #dd6b20;
        }
    )");
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearHistory);
    buttonLayout->addWidget(clearBtn);
    
    mainLayout->addLayout(buttonLayout);
    
    setCentralWidget(centralWidget);
    
    // Set window style
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f0f4f8;
        }
    )");
}

void MainWindow::setupSystemTray()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/app_icon.png"));
    trayIcon->setToolTip("Clipboard History Manager - Running in background");
    
    QMenu *trayMenu = new QMenu(this);
    
    QAction *showAction = trayMenu->addAction("Show Window");
    connect(showAction, &QAction::triggered, this, &MainWindow::toggleWindow);
    
    QAction *clearAction = trayMenu->addAction("Clear History");
    connect(clearAction, &QAction::triggered, this, &MainWindow::clearHistory);
    
    trayMenu->addSeparator();
    
    QAction *infoAction = trayMenu->addAction("Running... (Use Task Manager to exit)");
    infoAction->setEnabled(false);
    
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
    
    connect(trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            toggleWindow();
        }
    });
}

void MainWindow::onClipboardChanged()
{
    if (ignoreNextChange) {
        ignoreNextChange = false;
        return;
    }
    
    const QMimeData *mimeData = clipboard->mimeData();
    
    if (mimeData->hasImage()) {
        QPixmap pixmap = qvariant_cast<QPixmap>(mimeData->imageData());
        if (!pixmap.isNull()) {
            addImageToHistory(pixmap);
        }
    } else if (mimeData->hasText()) {
        QString text = mimeData->text();
        if (!text.isEmpty()) {
            // Don't add duplicates of the most recent item
            if (history.empty() || history.front().content != text) {
                addToHistory(text);
            }
        }
    }
}

void MainWindow::addToHistory(const QString &text)
{
    ClipboardItem item;
    item.content = text;
    item.timestamp = QDateTime::currentDateTime();
    item.isImage = false;
    
    // Add to front (stack behavior)
    history.push_front(item);
    
    // Remove oldest if exceeding max size
    if (history.size() > MAX_HISTORY_SIZE) {
        history.pop_back();
    }
    
    historyModified = true;
    updateHistoryDisplay();
}

void MainWindow::addImageToHistory(const QPixmap &image)
{
    ClipboardItem item;
    item.content = QString("[Image %1x%2]").arg(image.width()).arg(image.height());
    item.timestamp = QDateTime::currentDateTime();
    item.isImage = true;
    item.imageData = image;
    
    // Add to front (stack behavior)
    history.push_front(item);
    
    // Remove oldest if exceeding max size
    if (history.size() > MAX_HISTORY_SIZE) {
        history.pop_back();
    }
    
    historyModified = true;
    updateHistoryDisplay();
}

void MainWindow::updateHistoryDisplay()
{
    historyList->clear();
    
    for (size_t i = 0; i < history.size(); ++i) {
        const ClipboardItem &item = history[i];
        
        QString displayText;
        if (item.isImage) {
            displayText = QString("🖼️ %1").arg(item.content);
        } else {
            displayText = truncateText(item.content);
        }
        
        QString timeStr = item.timestamp.toString("hh:mm:ss");
        QString fullText = QString("[%1] %2").arg(timeStr, displayText);
        
        QListWidgetItem *listItem = new QListWidgetItem(fullText);
        listItem->setData(Qt::UserRole, static_cast<int>(i));
        listItem->setToolTip(item.isImage ? item.content : item.content.left(500));
        
        historyList->addItem(listItem);
    }
    
    statusLabel->setText(QString("Items: %1 / %2").arg(history.size()).arg(MAX_HISTORY_SIZE));
    
    // Disable buttons if no items
    bool hasItems = !history.empty();
    clearBtn->setEnabled(hasItems);
}

QString MainWindow::truncateText(const QString &text, int maxLength)
{
    QString singleLine = text.simplified();
    
    if (singleLine.length() <= maxLength) {
        return singleLine;
    }
    
    return singleLine.left(maxLength - 3) + "...";
}

void MainWindow::onItemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    bool hasSelection = historyList->currentItem() != nullptr;
    copyBtn->setEnabled(hasSelection);
    deleteBtn->setEnabled(hasSelection);
}

void MainWindow::onItemDoubleClicked(QListWidgetItem *item)
{
    if (!item) return;
    
    int index = item->data(Qt::UserRole).toInt();
    if (index >= 0 && index < static_cast<int>(history.size())) {
        ignoreNextChange = true;
        
        const ClipboardItem &clipItem = history[index];
        if (clipItem.isImage) {
            clipboard->setPixmap(clipItem.imageData);
        } else {
            clipboard->setText(clipItem.content);
        }
        
        trayIcon->showMessage("Clipboard Manager", 
                              "Copied to clipboard!", 
                              QSystemTrayIcon::Information, 
                              1500);
    }
}

void MainWindow::copySelectedItem()
{
    QListWidgetItem *item = historyList->currentItem();
    if (item) {
        onItemDoubleClicked(item);
    }
}

void MainWindow::deleteSelectedItem()
{
    QListWidgetItem *item = historyList->currentItem();
    if (!item) return;
    
    int index = item->data(Qt::UserRole).toInt();
    if (index >= 0 && index < static_cast<int>(history.size())) {
        history.erase(history.begin() + index);
        historyModified = true;
        updateHistoryDisplay();
        
        copyBtn->setEnabled(false);
        deleteBtn->setEnabled(false);
    }
}

void MainWindow::clearHistory()
{
    if (history.empty()) return;
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "Clear History", 
        "Are you sure you want to clear all clipboard history?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        history.clear();
        historyModified = true;
        updateHistoryDisplay();
        saveHistory(); // Save immediately after clearing
        copyBtn->setEnabled(false);
        deleteBtn->setEnabled(false);
    }
}

void MainWindow::toggleWindow()
{
    if (isVisible() && !isMinimized()) {
        hide();
    } else {
        show();
        raise();
        activateWindow();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Always hide to tray, never actually close
    hide();
    
    // Save history when minimizing to tray
    saveHistory();
    
    trayIcon->showMessage("Clipboard Manager",
                          "Running in background. Use Task Manager to exit completely.",
                          QSystemTrayIcon::Information,
                          2000);
    event->ignore();
}

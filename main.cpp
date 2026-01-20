#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Set application metadata
    a.setApplicationName("Clipboard History Manager");
    a.setApplicationVersion("2.0.0");
    a.setOrganizationName("ClipboardManager");
    a.setQuitOnLastWindowClosed(false); // Keep running in system tray
    
    MainWindow w;
    w.show();
    
    return a.exec();
}

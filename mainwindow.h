#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QLabel>

class QAction;
class QMenu;
class ImgPro;
class showLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

signals:
    void pixmapChanged(QPixmap);
    void mouseMove();
    void seedChanged();

private slots:
    void open();
    void openLenna();
    void openAvatar();
    bool save();
    bool saveAs();
    bool zoomIn();
    bool zoomOut();
    bool undo();
    bool contour();
    bool mask();
    bool debug();
    void about();
    void newSeed(int, int);
    void routing(int, int);
    void routingOver(int, int);
    void debugMode(int);
    void showOriginalImg(QPixmap);
    void showDebugMinPath();
    void showDebugPathGraph();

private:
    void initLogo();
    void initDisable(bool val);
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    ImgPro* img_pro;

    showLabel* picLabel;
    showLabel* debLabel;
    QLabel* pixelNodeLabel;
    QLabel* costGraphLabel;
    QLabel* pathTreeLabel;
    QLabel* minPathLabel;

    QPixmap* img;
    QPixmap* debugImg;
    QPixmap* pixelNode;
    QPixmap* costGraph;
    QPixmap* pathTree;
    QPixmap* minPath;

    QString curFile;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *scissorMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *scissorToolBar;
    QDockWidget* debugWin;
    QTabWidget* debugTab;
    QAction *openAct;
    QAction *openLennaAct;
    QAction *openAvatarAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *zoominAct;
    QAction *zoomoutAct;
    QAction *undoAct;
    QAction *contourAct;
    QAction *maskAct;
    QAction *debugAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
};

#endif

#include <QtWidgets>

#include "mainwindow.h"
#include "imagepro.h"
#include "showlabel.h"

MainWindow::MainWindow()
{
    img_pro = NULL;
    picLabel = new showLabel(this);
    picLabel->setAlignment(Qt::AlignBaseline);
    setCentralWidget(picLabel);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    initLogo();
    setCurrentFile("");
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::initLogo(){
   img=new QPixmap(":/images/logo_eye.gif");
   picLabel->setPixmap(*img);
   debugImg=img;
   pixelNode=img;
   pathTree=img;
   costGraph=img;
   minPath=img;
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, "", "", "images"
                                                        "(*.png, *.jpg *.bmp)");
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

void MainWindow::openLenna(){
    return loadFile(":/images/lenna.tif");
}

void MainWindow::openAvatar(){
    return loadFile(":/images/avatar.jpg");
}

bool MainWindow::save()
{
    return saveAs();
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("jpg");
    dialog.setNameFilter("jpg");
    QStringList files;
    if (dialog.exec())
        files = dialog.selectedFiles();
    else
        return false;

    return saveFile(files.at(0));
}

bool MainWindow::cut(){
    return true;
}

bool MainWindow::paste(){
    return true;
}

bool MainWindow::copy(){
    return true;
}

bool MainWindow::zoomIn(){
    return true;
}

bool MainWindow::zoomOut(){
    return true;
}

bool MainWindow::rotate(){
    return true;
}

bool MainWindow::undo(){
    if(img_pro->curr_mode == 0){
        img_pro->dropLastSeed();
        picLabel->setPixmap(img_pro->toPixmap());
    }
    return true;
}

bool MainWindow::redo(){
    return true;
}

bool MainWindow::contour(){
    if(img_pro->curr_mode == 1){
        contourAct->setEnabled(true);
        img_pro->curr_mode = 0;
        picLabel->setPixmap(img_pro->toPixmap());
        maskAct->setEnabled(true);
    }
    else{
        maskAct->setEnabled(false);
        contourAct->setEnabled(false);
        picLabel->setMouseTracking(true);
        connect(picLabel, SIGNAL(mouseClick(int, int)), this, SLOT(newSeed(int, int)));
        connect(picLabel, SIGNAL(mouseMove(int, int)), this, SLOT(routing(int, int)));
        connect(picLabel, SIGNAL(mouseDClick(int, int)), this, SLOT(routingOver(int, int)));
    }
    return true;
}

bool MainWindow::mask(){
    maskAct->setEnabled(false);
    contourAct->setEnabled(true);
    picLabel->setPixmap(img_pro->toMaskPixmap());
    return true;
}

bool MainWindow::debug(){
    debugWin=new QDockWidget(tr("Debug Windows"), this);
    debugTab=new QTabWidget(debugWin);

    debLabel=new showLabel(debugTab);
    pixelNodeLabel=new QLabel(debugTab);
    costGraphLabel=new QLabel(debugTab);
    pathTreeLabel=new QLabel(debugTab);
    minPathLabel=new QLabel(debugTab);

    debLabel->setAlignment(Qt::AlignBaseline);
    pathTreeLabel->setAlignment(Qt::AlignBaseline);
    costGraphLabel->setAlignment(Qt::AlignBaseline);
    pixelNodeLabel->setAlignment(Qt::AlignBaseline);
    minPathLabel->setAlignment(Qt::AlignBaseline);

    debLabel->setPixmap(*debugImg);
    pixelNodeLabel->setPixmap(*pixelNode);
    costGraphLabel->setPixmap(*costGraph);
    pathTreeLabel->setPixmap(*pathTree);
    minPathLabel->setPixmap(*minPath);
    debLabel->setPixmap(*debugImg);

    debugTab->addTab(debLabel, tr("Original Image"));
    debugTab->addTab(pixelNodeLabel, tr("Pixel Node"));
    debugTab->addTab(costGraphLabel, tr("Cost Graph"));
    debugTab->addTab(pathTreeLabel, tr("Path Tree"));
    debugTab->addTab(minPathLabel, tr("Minimum Path"));

    addDockWidget(Qt::RightDockWidgetArea, debugWin);
    debugWin->setWidget(debugTab);
    debugTab->show();


    connect(debugTab,SIGNAL(currentChanged(int)), this, SLOT(debugMode(int)));

    if(debugWin != NULL)
        return true;
    else
        return false;
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About COMP 5421 Proj"),
            tr("In this project, you can create a tool that allows a user"
               "to cut an object out of one image and paste it into another."
               "The tool helps the user trace the object by providing a <em>live wire</em> "
               "that automatically snaps to and wraps around the object of interest."
               "You will then use your tool to create a composite image."));
}

void MainWindow::newSeed(int i, int j)
{
    img_pro->drawSeed(i, j);
    picLabel->setPixmap(img_pro->toPixmap());
    emit seedChanged();
}

void MainWindow::routing(int i, int j)
{
    img_pro->currPos(i, j);
    picLabel->setPixmap(img_pro->toPixmap());
    emit mouseMove();
}

void MainWindow::routingOver(int i, int j)
{
    picLabel->setMouseTracking(false);
    contourAct->setEnabled(false);
    maskAct->setEnabled(true);
    disconnect(picLabel, SIGNAL(mouseClick(int, int)), this, SLOT(newSeed(int, int)));
    disconnect(picLabel, SIGNAL(mouseMove(int, int)), this, SLOT(routing(int, int)));
    disconnect(picLabel, SIGNAL(mouseDClick(int, int)), this, SLOT(routingOver(int, int)));
    img_pro->closureSeed(i, j);
    picLabel->setPixmap(img_pro->toPixmap());
}

void MainWindow::debugMode(int ind)
{
    if(ind == -1 || ind == 0){
        disconnect(this, SIGNAL(mouseMove()), this, SLOT(showDebugMinPath()));
        disconnect(this, SIGNAL(seedChanged()), this, SLOT(showDebugPathGraph()));
        return;
    }
    if(ind == 1){
        pixelNodeLabel->setPixmap(img_pro->toPixelNodePixmap());
        disconnect(this, SIGNAL(mouseMove()), this, SLOT(showDebugMinPath()));
        disconnect(this, SIGNAL(seedChanged()), this, SLOT(showDebugPathGraph()));
    }
    if(ind == 2){
        costGraphLabel->setPixmap(img_pro->toCostGraphPixmap());
        disconnect(this, SIGNAL(mouseMove()), this, SLOT(showDebugMinPath()));
        disconnect(this, SIGNAL(seedChanged()), this, SLOT(showDebugPathGraph()));
    }
    if(ind == 3){
        pathTreeLabel->setPixmap(img_pro->toPathTreePixmap());
        connect(this, SIGNAL(seedChanged()), this, SLOT(showDebugPathGraph()));
        disconnect(this, SIGNAL(mouseMove()), this, SLOT(showDebugMinPath()));
    }
    if(ind == 4){
        minPathLabel->setPixmap(img_pro->toMinPathPixmap());
        connect(this, SIGNAL(mouseMove()), this, SLOT(showDebugMinPath()));
        disconnect(this, SIGNAL(seedChanged()), this, SLOT(showDebugPathGraph()));
    }
}

void MainWindow::showDebugMinPath()
{
    minPathLabel->setPixmap(img_pro->toMinPathPixmap());
}

void MainWindow::showDebugPathGraph()
{
    pathTreeLabel->setPixmap(img_pro->toPathTreePixmap());
}

void MainWindow::createActions()
{
    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing image"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    openLennaAct = new QAction(QIcon(":/images/lenna_icon.png"), tr("Open &Lenna"), this);
    openLennaAct->setStatusTip(tr("Load Lenna.bmp"));
    connect(openLennaAct, SIGNAL(triggered()), this, SLOT(openLenna()));

    openAvatarAct = new QAction(QIcon(":/images/avatar_icon.jpg"), tr("Open &Avatar"), this);
    openAvatarAct->setStatusTip(tr("Load Avatar.jpg"));
    connect(openAvatarAct, SIGNAL(triggered()), this, SLOT(openAvatar()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the image to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the image under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the COMP 5421 Proj"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the selected area to the clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the selected area to the clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the selected area into the current selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

    zoominAct = new QAction(QIcon(":/images/zoom-in.png"), tr("Zoom &In"), this);
    zoominAct->setShortcuts(QKeySequence::ZoomIn);
    zoominAct->setStatusTip(tr("Zoom in the image"));
    connect(zoominAct, SIGNAL(triggered()), this , SLOT(zoomIn()));

    zoomoutAct = new QAction(QIcon(":/images/zoom-out.png"), tr("Zoom &Out"), this);
    zoomoutAct->setShortcuts(QKeySequence::ZoomOut);
    zoomoutAct->setStatusTip(tr("Zoom out the image"));
    connect(zoomoutAct, SIGNAL(triggered()), this , SLOT(zoomOut()));

    rotateAct = new QAction(QIcon(":/images/rotate.png"), tr("&Rotate"), this);
    rotateAct->setStatusTip(tr("Rotate the image"));
    connect(rotateAct, SIGNAL(triggered()), this, SLOT(rotate()));

    undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the latest strike"));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    redoAct = new QAction(QIcon(":/images/redo.png"), tr("Red&o"), this);
    redoAct->setStatusTip(tr("Redo the latest strike"));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

    contourAct = new QAction(QIcon(":/images/contour.png"), tr("&Contour"), this);
    contourAct->setStatusTip(tr("Draw the contour"));
    connect(contourAct, SIGNAL(triggered()), this, SLOT(contour()));

    maskAct = new QAction(QIcon(":/images/mask.png"), tr("&Mask"), this);
    maskAct->setStatusTip(tr("Generate the mask"));
    connect(maskAct, SIGNAL(triggered()), this, SLOT(mask()));

    debugAct = new QAction(QIcon(":/images/bug.png"), tr("&Debug"), this);
    debugAct->setStatusTip(tr("Switch into debug mode"));
    connect(debugAct, SIGNAL(triggered()), this, SLOT(debug()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(openLennaAct);
    fileMenu->addAction(openAvatarAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(zoominAct);
    editMenu->addAction(zoomoutAct);
    editMenu->addAction(rotateAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);

    scissorMenu = menuBar()->addMenu(tr("&Scissor"));
    scissorMenu->addAction(contourAct);
    scissorMenu->addAction(maskAct);
    scissorMenu->addAction(debugAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    setIconSize(QSize(32,32));
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(openLennaAct);
    fileToolBar->addAction(openAvatarAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(zoominAct);
    editToolBar->addAction(zoomoutAct);
    editToolBar->addAction(rotateAct);
    editToolBar->addSeparator();
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addSeparator();
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);

    scissorToolBar=addToolBar(tr("Scissor"));
    addToolBar(Qt::LeftToolBarArea, scissorToolBar);
    scissorToolBar->addAction(contourAct);
    scissorToolBar->addSeparator();
    scissorToolBar->addAction(maskAct);
    scissorToolBar->addSeparator();
    scissorToolBar->addAction(debugAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Welcome to COMP 5421"));
}

void MainWindow::readSettings()
{
    QSettings settings("Mingfei SUN", "COMP 5421 Project");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("QtProject", "Application Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
    if (isWindowModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Save Image?"),
                     tr("The image has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    if (!img->load(fileName)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot open image %1.")
                             .arg(fileName));
        return;
    }
    picLabel->setPixmap(*img);
    emit pixmapChanged(*img);
    setCurrentFile(fileName);
    if(img_pro != NULL){
        delete img_pro;
        img_pro = NULL;
    }
    img_pro = new ImgPro(img);
    //picLabel->setPixmap(img_pro->toPixmap());
    statusBar()->showMessage(tr("Image %1 loaded")
                             .arg(fileName), 4000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QPixmap new_save;
    if(img_pro->curr_mode == 0){
        new_save = img_pro->toPixmap();
    }
    if(img_pro->curr_mode == 1){
        new_save = img_pro->toMaskPixmap();
    }
    if(!new_save.save(fileName)) {
        QMessageBox::warning(this, tr("Save Image"),
                             tr("Cannot save image %1.")
                             .arg(fileName));
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Image saved as %1").arg(fileName), 4000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "COMP 5421";
    setWindowFilePath(shownName);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

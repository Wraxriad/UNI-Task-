#include "MainWindow.h"
#include "Logger.h"
#include "ui_MainWindow.h"
#include "Renderer.h"
#include <QKeyEvent>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mLogger(Logger::getInstance())
{
    qDebug() << "=== MainWindow constructor started ===";

    ui->setupUi(this);
    qDebug() << "UI setup complete";

    mLogger.setMainWindow(this);
    qDebug() << "Logger set";

    resize(1300, 850);
    setWindowTitle("Vulkan 26");
    qDebug() << "Window sized";

    mVulkanWindow = new Renderer();
    qDebug() << "Renderer created";

    mVulkanWindow->setTitle("Renderer");
    mVulkanWindow->setMainWindow(this);
    qDebug() << "MainWindow set on renderer";

    mVulkanWindow->setWidth(1000);
    mVulkanWindow->setHeight(800);
    qDebug() << "About to call initVulkan()";

    mVulkanWindow->initVulkan();
    qDebug() << "initVulkan() completed";

    QWidget* vulkanWidget = QWidget::createWindowContainer(mVulkanWindow, this);
    vulkanWidget->setMinimumSize(1000, 600);
    vulkanWidget->setFocusPolicy(Qt::StrongFocus);
    vulkanWidget->setFocus();
    ui->VulkanLayout->addWidget(vulkanWidget);
    qDebug() << "Vulkan widget added";

    mCamera = mVulkanWindow->getCamera();
    qDebug() << "Camera obtained";

    // === SPAWN OBJECTS HERE ===
    spawnTestObjects();
    qDebug() << "Objects spawned";

    statusBar()->showMessage(" Everybody loves Vulkan! ");

    mUpdateTimer = new QTimer(this);
    connect(mUpdateTimer, &QTimer::timeout, this, &MainWindow::updateFrame);
    mUpdateTimer->start(16);

    qDebug() << "=== MainWindow constructor finished ===";
}

void MainWindow::updateFrame()
{
    handleInput();
    mVulkanWindow->requestUpdate();
}

MainWindow::~MainWindow()
{
    if (mUpdateTimer) {
        mUpdateTimer->stop();
        delete mUpdateTimer;
    }

    if(mVulkanWindow)
    {
        delete mVulkanWindow;
        mVulkanWindow = nullptr;
    }
    delete ui;
}

void MainWindow::start()
{
    LOGW("Start is called");
    // This starts the update and rendering in the VulkanWindow
    mVulkanWindow->requestUpdate();
}

void MainWindow::spawnTestObjects()
{
    GameObject* pointsObj = mVulkanWindow->spawnPointsObject("Castle_2_Points.txt");
    if (pointsObj) {
        pointsObj->setPosition(glm::vec3(13, -10, 0));
        pointsObj->setScale(glm::vec3(1, 1, 1));

        pointsObj->setRotation(glm::vec3(-90.0f, 90.0f, 0.0f));
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        quitApp();
    }
    if (event->key() == Qt::Key_Space) {
        start();
    }
    if (event->key() == Qt::Key_W) mInput.W = true;
    if (event->key() == Qt::Key_S) mInput.S = true;
    if (event->key() == Qt::Key_A) mInput.A = true;
    if (event->key() == Qt::Key_D) mInput.D = true;
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_W) mInput.W = false;
    if (event->key() == Qt::Key_S) mInput.S = false;
    if (event->key() == Qt::Key_A) mInput.A = false;
    if (event->key() == Qt::Key_D) mInput.D = false;
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        mInput.RMB = true;
        mMouseXlast = event->pos().x();
        mMouseYlast = event->pos().y();
    }

    if (event->button() == Qt::MiddleButton) {
        mInput.MMB = true;

        // RESET LOGIC:
        mCameraSpeed = mDefaultCameraSpeed;

        float multiplier = mCameraSpeed / mDefaultCameraSpeed;

        QString resetText = QString("Camera Speed reset to default: %1x (%2)")
                                .arg(double(multiplier), 0, 'f', 1)
                                .arg(double(mCameraSpeed), 0, 'f', 6);

        LOGH(resetText.toStdString());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) mInput.RMB = false;

    if (event->button() == Qt::MiddleButton) mInput.MMB = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (mInput.RMB) {
        int deltaX = event->pos().x() - mMouseXlast;
        int deltaY = event->pos().y() - mMouseYlast;

        mCamera->mYaw   -= deltaX * mCameraRotationSpeed;
        mCamera->mPitch -= deltaY * mCameraRotationSpeed;


        if (mCamera->mPitch > 89.0f)  mCamera->mPitch = 89.0f;
        if (mCamera->mPitch < -89.0f) mCamera->mPitch = -89.0f;


        QString rotText = QString("Camera Rot: [Yaw: %1, Pitch: %2]")
                              .arg(mCamera->mYaw, 0, 'f', 1)
                              .arg(mCamera->mPitch, 0, 'f', 1);


        LOGH(rotText.toStdString());
    }

    mMouseXlast = event->pos().x();
    mMouseYlast = event->pos().y();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;

    if (mInput.RMB)
    {

        float speedStep = mDefaultCameraSpeed * 0.3f;

        if (numDegrees.y() > 0)
        {
            mCameraSpeed += speedStep;


            if (mCameraSpeed > (mDefaultCameraSpeed * 20.0f))
                mCameraSpeed = mDefaultCameraSpeed * 20.0f;
        }
        else if (numDegrees.y() < 0)
        {
            mCameraSpeed -= speedStep;


            if (mCameraSpeed < (mDefaultCameraSpeed * 0.1f))
                mCameraSpeed = mDefaultCameraSpeed * 0.1f;
        }

        // Calculate the multiplier (Current / Default)
        float multiplier = mCameraSpeed / mDefaultCameraSpeed;

        // Create the formatted string
        QString speedText = QString("Camera Speed updated: %1x (%2)")
                                .arg(double(multiplier), 0, 'f', 1)
                                .arg(double(mCameraSpeed), 0, 'f', 6);

        LOGH(speedText.toStdString());
    }
    event->accept();
}


void MainWindow::handleInput() {
    if (!mCamera) {
        LOGE("Camera is null!");
        return;
    }

    static int frameCount = 0;
    frameCount++;

    if (mInput.W || mInput.S || mInput.A || mInput.D || mInput.RMB) {
        if (frameCount % 30 == 0) {
            LOGH(QString("Input state - W:%1 S:%2 A:%3 D:%4 RMB:%5")
                     .arg(mInput.W)
                     .arg(mInput.S)
                     .arg(mInput.A)
                     .arg(mInput.D)
                     .arg(mInput.RMB)
                     .toStdString());
        }
    }

    mCamera->resetCamera();

    bool isMoving = false;

    if (mInput.RMB) {
        if (frameCount % 30 == 0) {
            LOGH("RMB is pressed - movement enabled");
        }

        if (mInput.W) {
            mCamera->mCameraMovement.z += mCameraSpeed;
            isMoving = true;
        }
        if (mInput.S) {
            mCamera->mCameraMovement.z -= mCameraSpeed;
            isMoving = true;
        }
        if (mInput.D) {
            mCamera->mCameraMovement.x += mCameraSpeed;
            isMoving = true;
        }
        if (mInput.A) {
            mCamera->mCameraMovement.x -= mCameraSpeed;
            isMoving = true;
        }
    } else {
        // If keys are pressed but RMB isn't held, show warning occasionally
        if ((mInput.W || mInput.S || mInput.A || mInput.D) && frameCount % 30 == 0) {
            LOGW("Keys pressed but RMB not held - movement disabled");
        }
    }

    // Log movement values before update
    if (isMoving && frameCount % 30 == 0) {
        LOGH(QString("CameraMovement before update: (%1, %2, %3)")
                 .arg(mCamera->mCameraMovement.x)
                 .arg(mCamera->mCameraMovement.y)
                 .arg(mCamera->mCameraMovement.z)
                 .toStdString());
    }

    mCamera->updateCamera();

    // Log new position after update
    if (isMoving && frameCount % 30 == 0) {
        LOGH(QString("New Camera Position: (%1, %2, %3)")
                 .arg(mCamera->mPosition.x, 0, 'f', 2)
                 .arg(mCamera->mPosition.y, 0, 'f', 2)
                 .arg(mCamera->mPosition.z, 0, 'f', 2)
                 .toStdString());
    }

    // Update status bar (keep your existing code)
    float multiplier = mCameraSpeed / mDefaultCameraSpeed;
    statusBar()->showMessage(QString("Camera position ") +
                             "x: " + QString::number(double(mCamera->mPosition.x), 'f', 2) + ", " +
                             "y: " + QString::number(double(mCamera->mPosition.y), 'f', 2) + ", " +
                             "z: " + QString::number(double(mCamera->mPosition.z), 'f', 2) + "     " +
                             "Camera speed: " + QString::number(double(multiplier), 'f', 1) + "x");
}
void MainWindow::on_action_BackgroundColor_triggered()
{
    // Here you should get the current color from the Renderer class
    QColor currentColor = QColor::fromRgbF(1.0, 0.0, 0.0); // Red as default for now

    //QColor has a strange format, so have to fix the string a bit for logging
    QString colorString = QString("currentColor: %1").arg(currentColor.name());
    LOGH(colorString.toStdString());

    // Open color dialog - with currentColor selected
    QColor color = QColorDialog::getColor(currentColor, this, "Choose Color");
    colorString = QString("new Color: %1").arg(color.name());
    LOGW(colorString.toStdString());

    //Now you can push this color to the Renderer, to use this as the VkClearValue clearColor!
}

void MainWindow::on_action_Quit_triggered()
{
    quitApp();
}

void MainWindow::quitApp()
{
    delete mVulkanWindow;
    mVulkanWindow = nullptr;
    close(); // calls Qt to close the app
}

void MainWindow::on_actionLog_to_Console_toggled(bool arg)
{
    mLogger.setPrintToConsole( arg );
}

void MainWindow::on_actionDelete_Logfile_at_start_toggled(bool arg)
{
    mLogger.setDeleteLogFileAtStart( arg );
}

void MainWindow::on_actionLog_to_File_toggled(bool arg)
{
    mLogger.setLogToFile( arg );
}


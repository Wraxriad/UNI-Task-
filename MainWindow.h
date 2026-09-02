#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "camera.h"
#include <QTimer>
#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void spawnTestObjects();
    void start();
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void handleInput();

private slots:
    void on_action_BackgroundColor_triggered();
    void on_action_Quit_triggered();
    void on_actionLog_to_Console_toggled(bool arg);
    void on_actionDelete_Logfile_at_start_toggled(bool arg);
    void on_actionLog_to_File_toggled(bool arg);

    void updateFrame();

private:
    Ui::MainWindow *ui{nullptr};

    Input mInput;

    const float mDefaultCameraSpeed{0.06f};
    float mCameraSpeed{mDefaultCameraSpeed};
    float mCameraRotationSpeed{0.15f};

    int mMouseXlast{0};
    int mMouseYlast{0};

    class Camera * mCamera{nullptr};

    class Renderer* mVulkanWindow{nullptr};

    void quitApp();

    class Logger& mLogger;

    friend class Logger;

    QTimer* mUpdateTimer{nullptr};

};
#endif // MAINWINDOW_H

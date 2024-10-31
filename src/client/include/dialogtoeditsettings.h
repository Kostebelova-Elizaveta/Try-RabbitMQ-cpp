#ifndef DIALOGTOEDITSETTINGS_H
#define DIALOGTOEDITSETTINGS_H

#include <QDialog>
#include <QDebug>
#include <QSettings>
#include <QIntValidator>
#include <QFileDialog>
#include <QMessageBox>
#include "logger.h"

namespace Ui {
class DialogToEditSettings;
}

class DialogToEditSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogToEditSettings(QWidget *parent = nullptr);
    ~DialogToEditSettings();

private slots:

    void on_DialogToEditSettings_rejected();

    void on_DialogToEditSettings_accepted();

    void on_logPathButton_clicked();

signals:
    void settingsChanged(QSettings *settings);

private:
    Ui::DialogToEditSettings *ui;
    QSettings *settings;
};

#endif // DIALOGTOEDITSETTINGS_H

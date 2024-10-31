#include "dialogtoeditsettings.h"
#include "ui_dialogtoeditsettings.h"

const QString configPath = CLIENT_CONFIG_FILE_PATH;

DialogToEditSettings::DialogToEditSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogToEditSettings)
{
    ui->setupUi(this);
    ui->portEdit->setValidator(new QIntValidator);
    ui->logPathEdit->setReadOnly(true);
    settings = new QSettings(configPath, QSettings::IniFormat);

    ui->hostEdit->setText(settings->value("Rabbit/Host").toString());
    ui->portEdit->setText(settings->value("Rabbit/Port").toString());
    ui->routingkeyEdit->setText(settings->value("Rabbit/Routingkey").toString());

    ui->logPathEdit->setText(settings->value("Logging/LogPath").toString());
    QString level = settings->value("Logging/LogLevel").toString();
    if (level == "DEBUG") {
        ui->logLevelComboBox->setCurrentIndex(0);
    } else if (level == "INFO") {
        ui->logLevelComboBox->setCurrentIndex(1);
    } else if (level == "WARNING") {
        ui->logLevelComboBox->setCurrentIndex(2);
    } else if (level == "ERROR") {
        ui->logLevelComboBox->setCurrentIndex(3);
    } else {
        Logger::log(WARNING, "Invalid log level: " + level + ", INFO logLevel selected");
        ui->logLevelComboBox->setCurrentIndex(1);
    }
}

DialogToEditSettings::~DialogToEditSettings() {
    Logger::log(DEBUG, "DialogToEditSettings is closed");
    delete ui;
    delete settings;
}

void DialogToEditSettings::on_DialogToEditSettings_rejected() {
    Logger::log(DEBUG, "Settings weren't changed");
    this->deleteLater();
}


void DialogToEditSettings::on_DialogToEditSettings_accepted() {
    settings->setValue("Rabbit/Host", ui->hostEdit->text());
    settings->setValue("Rabbit/Port", ui->portEdit->text());
    settings->setValue("Rabbit/Routingkey", ui->routingkeyEdit->text());
    settings->setValue("Logging/LogLevel", ui->logLevelComboBox->currentText());
    settings->setValue("Logging/LogPath", ui->logPathEdit->text());

    Logger::initFromConfig(CLIENT_CONFIG_FILE_PATH);
    Logger::log(INFO, "New settings saved");

    emit(settingsChanged(settings));

    this->deleteLater();

}


void DialogToEditSettings::on_logPathButton_clicked() {
    Logger::log(DEBUG, "LogPath choose button clicked");
    const QString filePath = QFileDialog::getSaveFileName(this, "Select a log text-file", QDir::currentPath(), "Text files (*.txt)");
    if (!filePath.isEmpty()) {
        ui->logPathEdit->setText(filePath);
    }
    else {
        QMessageBox::warning(this, "Warning", "You have to choose or create some text-file!");
        Logger::log(WARNING, "You have to choose or create some text-file!");
    }
}


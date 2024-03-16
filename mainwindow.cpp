#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::handleAction);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::handleAction()
{
    // Get the selected method
    QString method = ui->comboBox->currentText();

    qDebug() << "Starting network request:" << method << ui->lineEdit->text();

    // Perform the network request based on the method
    if (method == "GET") {
        QNetworkAccessManager* manager = new QNetworkAccessManager(this);
        QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(ui->lineEdit->text())));
        connect(reply, &QNetworkReply::finished, [=]() {
            // Check for errors
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << "Error:" << reply->errorString();
                reply->deleteLater();
                manager->deleteLater();
                return;
            }

            // Read the response data
            QByteArray responseData = reply->readAll();

            // Parse JSON data
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);

            // Check if JSON parsing was successful
            if (jsonResponse.isNull()) {
                qDebug() << "Error: Failed to parse JSON response";
                reply->deleteLater();
                manager->deleteLater();
                return;
            }

            // Display JSON data in QTextEdit
            ui->textBrowser->append("Response:");
            ui->textBrowser->append(jsonResponse.toJson(QJsonDocument::Indented));

            // Cleanup
            reply->deleteLater();
            manager->deleteLater();
        });
    }

    // For demonstration purposes, let's display the method and input text in the text edit
    ui->textBrowser->append("Method: " + method);
    ui->textBrowser->append("Input: " + ui->lineEdit->text());
}

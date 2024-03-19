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
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::moreActions);
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
                ui->textBrowser_2->append("Error: " + reply->errorString());
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
    else if(method == "POST"){
        QString jsonDataString = ui->textEdit->toPlainText();

        QJsonParseError parseError;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonDataString.toUtf8(), &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qDebug() << "Error parsing JSON:" << parseError.errorString();
            return;
        }

        QNetworkAccessManager* manager = new QNetworkAccessManager(this);
        QNetworkRequest request(QUrl(ui->lineEdit->text()));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkReply* reply = manager->post(request, jsonDocument.toJson());
        connect(reply, &QNetworkReply::finished, [=]() {
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << "Error:" << reply->errorString();
                ui->textBrowser_2->append("Error: " + reply->errorString());
            } else {
                // Read the response data
                QByteArray responseData = reply->readAll();

                // Parse JSON data
                QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);

                // Check if JSON parsing was successful
                if (jsonResponse.isNull()) {
                    qDebug() << "Error: Failed to parse JSON response";
                } else {
                    // Print the JSON data
                    qDebug().noquote() << "Response:" << jsonResponse.toJson(QJsonDocument::Indented);
                    ui->textBrowser->append("Json :\n" + QString(jsonResponse.toJson(QJsonDocument::Indented)));
                }
            }

            // Clean up
            reply->deleteLater();
            manager->deleteLater();
        });
    }
    else if(method == "PUT"){
        QNetworkAccessManager* manager = new QNetworkAccessManager(this);

        // Get the request URL and data
        QUrl url(ui->lineEdit->text());
        QByteArray data = ui->textEdit->toPlainText().toUtf8();

        // Create the PUT request
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        // Send the PUT request
        QNetworkReply* reply = manager->put(request, data);
        connect(reply, &QNetworkReply::finished, [=](){
            if(reply->error() != QNetworkReply::NoError){
                qDebug() << "Error:" << reply->errorString();
                ui->textBrowser_2->append("Error: " + reply->errorString());
            }else{
                QByteArray responseData = reply->readAll();

                ui->textBrowser->append("Response; ");
                ui->textBrowser->append(responseData);
            }
            reply->deleteLater();
            reply->deleteLater();
        });
    }
    else if (method == "PATCH") {
        // Create the network manager
        QNetworkAccessManager* manager = new QNetworkAccessManager(this);

        // Get the request URL and data
        QUrl url(ui->lineEdit->text());
        QByteArray data = ui->textEdit->toPlainText().toUtf8();

        // Create the PATCH request
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        // Send the PATCH request
        QNetworkReply* reply = manager->sendCustomRequest(request, "PATCH", data);
        connect(reply, &QNetworkReply::finished, [=]() {
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << "Error:" << reply->errorString();
                ui->textBrowser_2->append("Error: " + reply->errorString());
            } else {
                // Read the response data
                QByteArray responseData = reply->readAll();

                // Display the response
                ui->textBrowser->append("Response:");
                ui->textBrowser->append(responseData);
            }

            // Clean up
            reply->deleteLater();
            manager->deleteLater();
        });
    }
    else if(method == "DELETE"){
        QNetworkAccessManager* manager = new  QNetworkAccessManager(this);
        QNetworkRequest request(QUrl(ui->lineEdit->text()));

        QNetworkReply* reply = manager->deleteResource(request);
        connect(reply, &QNetworkReply::finished, [=](){
            if(reply->error() != QNetworkReply::NoError){
                qDebug() << "Error:" << reply->errorString();
                ui->textBrowser_2->append("Error: " + reply->errorString());
            }else{
                QByteArray responseData = reply->readAll();

                ui->textBrowser->append("Response; ");
                ui->textBrowser->append(responseData);
            }
            reply->deleteLater();
            reply->deleteLater();
        });
    }


    // For demonstration purposes, let's display the method and input text in the text edit

    ui->textBrowser_2->append("Method: " + method);
    ui->textBrowser_2->append("Input: " + ui->lineEdit->text());
}

void MainWindow::moreActions(){
    // Create new widgets
    QLineEdit *lineEdit = new QLineEdit;
    QComboBox *comboBox = new QComboBox;
    QPushButton *actionButton = new QPushButton("Action");

    // Populate combobox with methods
    comboBox->addItems({"GET", "POST", "PUT", "PATCH", "DELETE"});

    // Create a new QHBoxLayout and add widgets to it
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(lineEdit);
    horizontalLayout->addWidget(comboBox);
    horizontalLayout->addWidget(actionButton);
    ui->verticalLayout->addLayout(horizontalLayout);
}


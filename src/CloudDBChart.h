/*
 * Copyright (c) 2015 Joern Rischmueller (joern.rm@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef CLOUDDBCHART_H
#define CLOUDDBCHART_H

#include "LTMSettings.h"
#include "Settings.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QSslSocket>
#include <QUuid>
#include <QUrl>
#include <QTableWidget>


// API Structure V1 (flattened) must be in sync with the Structure used for the V1
// but uses the correct QT datatypes

struct ChartAPIv1 {
    quint64 Id;
    QString Name;
    QString Description;
    QString Language;
    QString GcVersion;
    QString ChartXML;
    QByteArray Image;
    QDateTime LastChanged;
    QString CreatorId;
    QString CreatorNick;
    QString CreatorEmail;
    bool    Curated;
    bool    Deleted;
};

struct ChartAPIHeaderV1 {
    quint64 Id;
    QString Name;
    QString GcVersion;
    QDateTime LastChanged;
    bool    Curated;
    bool    Deleted;
};


class CloudDBChartClient : public QObject
{
    Q_OBJECT

public:

    CloudDBChartClient();
    ~CloudDBChartClient();

    bool postChart(ChartAPIv1 chart);
    bool getChartByID(qint64 , ChartAPIv1*);
    bool getAllChartHeader(QList<ChartAPIHeaderV1>*);

    bool sslLibMissing() { return noSSLlib; }

public slots:

    // Catch NAM signals ...
    void sslErrors(QNetworkReply*,QList<QSslError>);

private:
    bool noSSLlib;

    QNetworkAccessManager* g_nam;
    QNetworkDiskCache* g_cache;
    QNetworkReply *g_reply;

    QString  g_chart_url_base;
    QString  g_chart_url_header;
    QVariant g_header_content_type;
    QByteArray g_header_basic_auth;

    static bool unmarshallAPIv1(QByteArray , QList<ChartAPIv1>* );
    static void unmarshallAPIv1Object(QJsonObject* , ChartAPIv1* );

    static bool unmarshallAPIHeaderV1(QByteArray , QList<ChartAPIHeaderV1>* );
    static void unmarshallAPIHeaderV1Object(QJsonObject* , ChartAPIHeaderV1* chart);

};

struct ChartImportUIStructure {
    QString name;
    QString description;
    QString creatorNick;
    QPixmap image;
    LTMSettings ltmSettings;
};

class CloudDBChartImportDialog : public QDialog
{
    Q_OBJECT

public:

    CloudDBChartImportDialog();
    ~CloudDBChartImportDialog();

    void initialize();
    LTMSettings getSelectedSettings() {return g_selected; }

private slots:

    void addAndCloseClicked();
    void closeClicked();
    void resetToStartClicked();
    void nextSetClicked();
    void prevSetClicked();
    void curatedToggled(bool);


private:

    LTMSettings g_selected;

    CloudDBChartClient* g_client;
    QList<ChartImportUIStructure> *g_currentPresets;
    int g_currentIndex;
    int g_stepSize;
    QList<ChartAPIHeaderV1>* g_currentHeaderList;
    QList<ChartAPIHeaderV1>* g_fullHeaderList;
    bool g_filterActive;


    // UI elements
    QLabel *showing;
    QString showingTextTemplate;
    QPushButton *resetToStart;
    QPushButton *nextSet;
    QPushButton *prevSet;
    QCheckBox *curatedOnly;
    QLabel *filterLabel;
    QLineEdit *filter;
    QTableWidget *tableWidget;
    QPushButton *addAndCloseButton, *closeButton;

    // helper methods
    void getCurrentPresets(int, int);
    void updateDialogWithCurrentPresets();
    void applyFilterAndCurated(bool, QString);
    QString encodeHTML ( const QString& );

};


class CloudDBChartPublishDialog : public QDialog
{
    Q_OBJECT

public:

    CloudDBChartPublishDialog(ChartAPIv1 data);
    ~CloudDBChartPublishDialog();

    ChartAPIv1 getData() { return data; }

private slots:
    void publishClicked();
    void cancelClicked();


private:

    ChartAPIv1 data;

    QPushButton *publishButton, *cancelButton;
    QLineEdit *name;
    QLabel *image;
    QTextEdit *description;
    QLineEdit *nickName;
    QLineEdit *email;
    //QComboBox *language;
    QLabel *gcVersionString;
    QLabel *creatorId;

};


#endif // CLOUDDBCHART_H

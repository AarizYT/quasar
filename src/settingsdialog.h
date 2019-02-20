#pragma once

#include <atomic>

#include <QWidget>
#include <QtWebEngineWidgets/QWebEngineProfile>

class DataServer;

class SettingsDialog : public QWidget
{
public:
    SettingsDialog(DataServer* server, QWidget* parent = nullptr);
    ~SettingsDialog();

private:
    QWebEngineProfile* profile;

    Q_DISABLE_COPY(SettingsDialog);
};

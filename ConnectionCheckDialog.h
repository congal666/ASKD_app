#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ConnectionCheckDialog; }
QT_END_NAMESPACE

class ConnectionCheckDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectionCheckDialog(QWidget *parent = nullptr);
    ~ConnectionCheckDialog();

private:
    void startOneShotCheck();
    bool sendAndWait10s(QString portName, QString *err);
    bool autoDetectPort(QString *foundPort, QString *err);
    bool probePort(const QString &portName, QString *err);

private:
    Ui::ConnectionCheckDialog *ui = nullptr;
};

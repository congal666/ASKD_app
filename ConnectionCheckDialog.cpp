#include "ConnectionCheckDialog.h"
#include "ui_ConnectionCheckDialog.h"

#include <QSerialPort>
#include <QElapsedTimer>
#include <QTimer>
#include <QSerialPortInfo>

static constexpr char KT = 0x03;

ConnectionCheckDialog::ConnectionCheckDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::ConnectionCheckDialog)
{
    ui->setupUi(this);

    // чтобы окно просто показывалось без кнопок
    setModal(true);

    ui->lblStatus->setText("Проверяю устройство...");

    // запускаем проверку сразу после показа
    QTimer::singleShot(0, this, &ConnectionCheckDialog::startOneShotCheck);
}

ConnectionCheckDialog::~ConnectionCheckDialog()
{
    delete ui;
}

void ConnectionCheckDialog::startOneShotCheck()
{
    ui->lblStatus->setText("Ищу устройство (COM)...");

    QString foundPort;
    QString err;

    const bool ok = autoDetectPort(&foundPort, &err);

    if (ok) {
        ui->lblStatus->setText("Найдено на " + foundPort + " ✅");
        // тут можно сохранить foundPort в поле класса/настройки
    } else {
        ui->lblStatus->setText("Нет ответа: " + err + " ❌");
    }

    QTimer::singleShot(1000, this, [this]{ accept(); });
}

bool ConnectionCheckDialog::sendAndWait10s(QString portName, QString *err)
{
    QSerialPort sp;
    sp.setPortName(portName);
    sp.setBaudRate(QSerialPort::Baud115200);
    sp.setDataBits(QSerialPort::Data8);
    sp.setParity(QSerialPort::EvenParity);
    sp.setStopBits(QSerialPort::OneStop);
    sp.setFlowControl(QSerialPort::NoFlowControl);

    if (!sp.open(QIODevice::ReadWrite)) {
        if (err) *err = "Не открылся порт: " + sp.errorString();
        return false;
    }

    // команда как в паскале: 'A' + 0x03
    const QByteArray cmd = QByteArray() + char(0x41) + char(KT);
    if (sp.write(cmd) != cmd.size() || !sp.waitForBytesWritten(500)) {
        if (err) *err = "Не отправилась команда";
        return false;
    }

    // ждём до 10 секунд любой ответ, где встретится KT
    QByteArray rx;
    QElapsedTimer t; t.start();
    while (t.elapsed() < 10'000) {
        if (sp.waitForReadyRead(100)) {
            rx += sp.readAll();
            if (rx.contains(char(KT))) {
                return true;
            }
        }
    }

    if (err) *err = "Нет ответа за 10 секунд";
    return false;
}


bool ConnectionCheckDialog::autoDetectPort(QString *foundPort, QString *err)
{
    const auto ports = QSerialPortInfo::availablePorts();
    if (ports.isEmpty()) {
        if (err) *err = "COM-порты не найдены";
        return false;
    }

    QElapsedTimer total;
    total.start();

    QString lastError;

    // можно сначала попробовать порты с нормальными описаниями
    for (const QSerialPortInfo &pi : ports) {
        if (total.elapsed() > 10'000) break;

        const QString name = pi.portName(); // "COM3" на Windows
        QString e;
        if (probePort(name, &e)) {
            if (foundPort) *foundPort = name;
            return true;
        }
        lastError = name + ": " + e;
    }

    if (err) {
        *err = "Устройство не найдено. Последняя ошибка: " + lastError;
    }
    return false;
}

// проверка конкретного порта: открыть -> отправить -> ждать ответ
bool ConnectionCheckDialog::probePort(const QString &portName, QString *err)
{

    QSerialPort sp;
    sp.setPortName(portName);
    sp.setBaudRate(QSerialPort::Baud115200);
    sp.setDataBits(QSerialPort::Data8);
    sp.setParity(QSerialPort::EvenParity);
    sp.setStopBits(QSerialPort::OneStop);
    sp.setFlowControl(QSerialPort::NoFlowControl);

    if (!sp.open(QIODevice::ReadWrite)) {
        if (err) *err = "не открылся: " + sp.errorString();
        return false;
    }

    const QByteArray cmd = QByteArray() + char(0x41) + char(KT); // 'A' + 0x03
    if (sp.write(cmd) != cmd.size() || !sp.waitForBytesWritten(200)) {
        if (err) *err = "не отправилась команда";
        return false;
    }

    QByteArray rx;
    QElapsedTimer t; t.start();
    while (t.elapsed() < 10'000) {
        if (sp.waitForReadyRead(100)) {
            rx += sp.readAll();
            if (rx.contains(char(KT))) {
                return true;
            }
        }
    }

    if (err) *err = "нет ответа";
    return false;
}

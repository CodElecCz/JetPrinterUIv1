#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDialog>

class TcpServer : public QDialog
{
    Q_OBJECT

public:
    explicit TcpServer(int port, QWidget *parent = nullptr):
           QDialog(parent)
    {
        this->server = new QTcpServer(this);
        connect(server, SIGNAL(newConnection()), this, SLOT(on_newConnection()));
        server->listen(QHostAddress::Any, port);
    }

signals:
    void response(QString cmd, QString *response);

private slots:
    void on_newConnection()
    {
        socket= server->nextPendingConnection();
        if(socket->state() == QTcpSocket::ConnectedState)
        {
            qDebug()<<"New connection established.";
            qDebug()<<socket->peerAddress();
        }

        connect(socket, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
        connect(socket, SIGNAL(readyRead()), this, SLOT(on_readyReadData()));
    }

    void on_disconnected()
    {
        qDebug()<<"Connection disconnected.";
        disconnect(socket, SIGNAL(disconnected()));
        disconnect(socket, SIGNAL(readyRead()));
        socket->deleteLater();
    }

    void on_readyReadLine()
    {
        while(socket->canReadLine())
        {
            QByteArray ba = socket->readLine();
            QString cmd(ba.constData());
            //qDebug(ba.constData());

            if(cmd.contains("exit"))
            {
                socket->disconnectFromHost();
                break;
            }
            else
            {
                QString* resp = new QString();
                response(cmd, resp);
                if(resp->length()>0)
                {
                    socket->write(resp->toUtf8());
                    socket->waitForBytesWritten();
                }
            }
        }
    }

    void on_readyReadData()
    {
        QByteArray ba = socket->readAll();
        QString cmd(ba.constData());
        qDebug(ba.constData());

        if(cmd.contains("exit"))
        {
            socket->disconnectFromHost();
        }
        else
        {
            QString* resp = new QString();
            response(cmd, resp);
            if(resp->length()>0)
            {
                socket->write(resp->toUtf8());
                socket->waitForBytesWritten();
            }
        }
    }

private:
    QTcpServer      *server;
    QTcpSocket      *socket;
};

#endif // TCPSERVER_H

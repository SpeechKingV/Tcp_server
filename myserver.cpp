#include "myserver.h"

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QObject>
#include <QDataStream>
#include <QTime>

MyServer::MyServer(int nPort, QWidget *parent) : QWidget(parent) , m_nNextBlockSize(0)
{
    m_ptcpServer = new QTcpServer(this);
    if(!m_ptcpServer->listen(QHostAddress::Any, nPort))
    {
        QMessageBox::critical(0,
                              "Server Error",
                              "Unabel to start the server:"
                              + m_ptcpServer->errorString()
                              );
        m_ptcpServer->close();
        return;
    }

    connect(m_ptcpServer, SIGNAL(newConnection()),this,SLOT(SNewConnection()));

    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);

    //                                          Layout setup

    QVBoxLayout* pvbxLayot = new QVBoxLayout;

    pvbxLayot->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayot->addWidget(m_ptxt);

    setLayout(pvbxLayot);

}

void MyServer::SNewConnection()
{
    QTcpSocket* pClientSoket = m_ptcpServer->nextPendingConnection();
    connect(pClientSoket, SIGNAL(disconnected()), pClientSoket, SLOT(deleteLater()));
    connect(pClientSoket, SIGNAL(readyRead()), this , SLOT(SReadClient()));

    sendToClient(pClientSoket, "Server Response: Connected!");
}

void MyServer::SReadClient()
{
    QTcpSocket* pClientSoket = m_ptcpServer->nextPendingConnection();
    QDataStream in(pClientSoket);

    in.setVersion(QDataStream::Qt_5_3);

    for (; ; )
    {
        if(!m_nNextBlockSize)
        {
            if(pClientSoket->bytesAvailable() < sizeof (quint16))
            {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if(pClientSoket->bytesAvailable() < m_nNextBlockSize)
        {
            break;
        }

        QTime time;
        QString str;

        in >> time >> str;

        QString strMessage = time.toString() + " Client has sent - " + str;
        m_ptxt->append(strMessage);

        m_nNextBlockSize = 0;

        sendToClient(pClientSoket,"Server Response: Received \"" + str + "\"");

    }
}

void MyServer::sendToClient(QTcpSocket *pSocet, const QString &str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_5_3);
    out << quint16(0) << QTime::currentTime() << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocet->write(arrBlock);
}

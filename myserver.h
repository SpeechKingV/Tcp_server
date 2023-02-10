#ifndef MYSERVER_H
#define MYSERVER_H

#include <QMainWindow>
#include <QObject>


class QTcpServer;
class QTextEdit;
class QTcpSocket;

class MyServer : public QWidget
{
    Q_OBJECT

private:

    QTcpServer* m_ptcpServer;
    QTextEdit* m_ptxt;
    quint16 m_nNextBlockSize;

    void sendToClient(QTcpSocket* pSocet,const QString& str);

public:

    MyServer(int nPort,QWidget *parent = nullptr);

public slots:

    virtual void SNewConnection();
    void SReadClient   ();
};

#endif // MYSERVER_H


#ifndef REQUESTDISPATCHERTHREAD_H
#define REQUESTDISPATCHERTHREAD_H
#include <QThread>
#include <QLinkedList>
#include "requesthandler.h"
#include "abstractbuffer.h"
#include "threadpool.h"
#include "readerwritercache.h"

#define INVALIDATION_PERIOD 60 // periode de verification toute les minutes
#define STALE_DELAY 120      // donnée obsolète apres 2 minutes

/** \class      requestdispatcherthread
*   \authors    Adam Zouari et Oussama Lagha
*   \date       15 Mai 2018 (update 15 juin pour etape 3)
*   \brief      pour chaque request dans le buffer
*               on lance un requestHandler qui va s'occuper
*               de traiter le request
*/
class requestdispatcherthread : public QThread
{

public:
    requestdispatcherthread(AbstractBuffer<Request>* requests,AbstractBuffer<Response>* responses,bool hasDebugLog)
        : hasDebugLog(hasDebugLog),responses(responses),requests(requests)
    {
        // creation du thread pool avec comme taille le nombre de processeurs logiques de la machine
        int sizePool = QThread::idealThreadCount();
        threadpool = new ThreadPool(sizePool);

        // creation du cache
        cache = new ReaderWriterCache(INVALIDATION_PERIOD,STALE_DELAY);

        if (hasDebugLog)
            qDebug() << "Created request dispatcher thread";

    }

    void run(){

        while(true){

            if (hasDebugLog)
                qDebug() << "Waiting for request...";

            //on récupére la premiere requete dans le tampon
            Request request = requests->get();

            if (hasDebugLog)
                qDebug() << "Got a request " << request.getFilePath();

            RequestHandler* handler = new RequestHandler(request,responses,hasDebugLog,cache);

            // Donne le handler au thread pool afin qu'il cree un thread ou qu'il le recycle
            threadpool->start(handler);

        }
    }
private:
    ReaderWriterCache *cache;
    ThreadPool *threadpool;                 //une piscine des threads
    AbstractBuffer<Request>* requests;      //tampon qui contient tt les requetes a traiter
    AbstractBuffer<Response>* responses;    //tampon qui contient tt les reponses traiter par le serveur
    bool hasDebugLog;                       // permet de tester si on peut affché un msg
};
#endif // REQUESTDISPATCHERTHREAD_H

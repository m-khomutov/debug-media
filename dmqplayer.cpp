//
// Created by mkh on 16.11.2020.
//

#include "dmqplayer.h"

#include <QtGui/QPainter>
#include <QtCore/QThread>
#include <QtGui/QApplication>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QLineEdit>
#include <QtCore/QDebug>

dm::q::Player::Player( int width, int height, int ask_position_sec )
: m_image(new QImage() ),m_rgbframe( rgb::Geometry( width, height, AV_PIX_FMT_RGB24 ) ) {
    setAutoFillBackground(false);
    setAttribute( Qt::WA_NoSystemBackground, true );
    setAttribute( Qt::WA_PaintOnScreen,      true );

    QPalette palette = this->palette ();
    palette.setColor(QPalette::Background, Qt::black);
    setPalette( palette );

    resize( width, height );
    /*if( ask_position_sec) {
        m_position_timer = new QTimer( this );
        QObject::connect( m_position_timer, SIGNAL(timeout()), this, SLOT(f_ask_position()) );
        m_position_timer->start( ask_position_sec * 1000 );
    }*/
    //QThread* thread = new QThread;
    //m_receiver->moveToThread( thread );

    //connect(thread, SIGNAL(started ()), m_receiver, SLOT(run()));
    //connect(thread, SIGNAL(finished()), thread,     SLOT(deleteLater ()));
    //thread->start ();
}

void dm::q::Player::run() {
    show();
}

void dm::q::Player::onFrame( AVFrame *avframe ) {
    if( avframe ) {
        if( avframe->width && avframe->height ) {
            std::lock_guard< std::mutex > lk( m_frame_mutex );

            m_rgbframe.copy( (const uint8_t**)avframe->data, avframe->linesize, avframe->width, avframe->height );
            m_image.reset( new QImage( m_rgbframe.data(), m_rgbframe.width(), m_rgbframe.height(), QImage::Format_RGB888 ) );
        }
        if( m_ready_to_update )
            update();
    }
}

void dm::q::Player::f_ask_position() {
    //m_receiver->askPosition();
}

void dm::q::Player::f_show_keyusage_box() {
    QMessageBox msgBox;
    msgBox.setText("F1 - вывод справки\n"
                   "F2 - запрос позиции\n"
                   "F3 - установка позиции\n"
                   "F4 - установка параметра\n"
                   "F5 - установка скорости\n"
                   "F6 - пауза в воспроизведении\n"
                   "F7 - восстановление воспроизведения после паузы\n"
                   "Left - смещение позиции на 10 сек. вперед\n"
                   "Right - смещение позиции на 10 сек. назад\n"
                   "F10 - выход");
    msgBox.exec();
}

void dm::q::Player::showEvent( QShowEvent *event ) {
    m_ready_to_update.store( true );
    event->accept();
}

void dm::q::Player::paintEvent( QPaintEvent *event ) {
    std::lock_guard< std::mutex > lk( m_frame_mutex );

    QPainter painter( this );
    painter.drawImage(0, 0, m_image->scaled(this->size()));
    event->accept();
}

void dm::q::Player::keyReleaseEvent( QKeyEvent * event ) {
    switch( event->key()  ) {
        case Qt::Key_F1:
            f_show_keyusage_box();
            break;
        case Qt::Key_F2:
            //m_receiver->askPosition();
            break;
        case Qt::Key_F3:
            //m_receiver->setPosition();
            break;
        case Qt::Key_F4:
            //m_receiver->setParameter();
            break;
        case Qt::Key_F5:
            //m_receiver->setScale();
            break;
        case Qt::Key_F6:
            //m_receiver->pause();
            break;
        case Qt::Key_F7:
            //m_receiver->resume();
            break;
        case Qt::Key_Right:
            //m_receiver->moveForward();
            break;
        case Qt::Key_Left:
            //m_receiver->moveBackward();
            break;
        case Qt::Key_F10:
            //m_receiver->stop();
            qApp->quit();
            break;

    }
    event->accept();
}
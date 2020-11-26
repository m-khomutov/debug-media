//
// Created by mkh on 16.11.2020.
//

#include "dmqplayer.h"
#include "rtsp/dmmediasession.h"
#include "rtsp/dmconnection.h"

#include <QtGui/QPainter>
#include <QtCore/QThread>
#include <QtGui/QApplication>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QLineEdit>
#include <QtCore/QDebug>

dm::q::Player::Player( rtsp::MediaSession * session, int width, int height, int ask_position_sec )
: m_session( session ),m_image(new QImage() ),m_rgbframe( rgb::Geometry( width, height, AV_PIX_FMT_RGB24 ) ) {
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

void dm::q::Player::onFrame( int len, AVFrame *avframe ) {
    if( len != -1 ) {
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
    m_session->connection()->getParameter( "position" );
}
void dm::q::Player::f_set_position() {
    bool ok;
    double pos = m_session->position();
    double value = QInputDialog::getDouble(nullptr, QString::fromUtf8("позиционирование"),
                                            QString::fromUtf8("значение"), pos, 0, pos+1000, 1, &ok );
    if( ok ) {
        m_position = value;
        m_session->connection()->pause();
        QTimer::singleShot( 500, this, SLOT( f_resume()) );
    }
}
void dm::q::Player::f_ask_parameter() {
    bool ok;
    QString text = QInputDialog::getText(nullptr, QString::fromUtf8("запросить значение параметра"),
                                         QString::fromUtf8("название"),
                                         QLineEdit::Normal,
                                          "", &ok);
    if( ok && !text.isEmpty() )
        m_session->connection()->getParameter( std::string(text.toLocal8Bit()) );
}
void dm::q::Player::f_set_parameter() {
    bool ok;
    QString text = QInputDialog::getText(nullptr, QString::fromUtf8("установить значение параметра"),
                                         QString::fromUtf8("название: значение"),
                                         QLineEdit::Normal,
                                         ": ", &ok);
    if( ok && !text.isEmpty() )
        m_session->connection()->setParameter( std::string(text.toLocal8Bit()) );
}
void dm::q::Player::f_scale() {
    bool ok;
    double value = QInputDialog::getDouble(nullptr, QString::fromUtf8("скорость"),
                                                QString::fromUtf8("значение"), 1.0, -32, 32, 2, &ok );
    if( ok && value != .0 )
        m_session->connection()->scale( value );
}
void dm::q::Player::f_pause() {
    m_session->connection()->pause();

}
void dm::q::Player::f_resume() {
    m_session->connection()->resume( m_position );
}
void dm::q::Player::f_set_seek_step() {
    bool ok;
    double value = QInputDialog::getDouble( nullptr, QString::fromUtf8("шаг смещения"),
                                       QString::fromUtf8("значение"), m_seek_step, 0., m_seek_step+1000., 1, &ok );
    if( ok && value >=1. )
        m_seek_step = value;
}
void dm::q::Player::f_seek_forward() {
    m_session->connection()->pause();
    QTimer::singleShot( 500, this, SLOT( f_resume()) );
}
void dm::q::Player::f_seek_backward() {
    m_session->connection()->pause();
    QTimer::singleShot( 500, this, SLOT( f_resume()) );
}

void dm::q::Player::f_show_keyusage_box() {
    QMessageBox msgBox;
    msgBox.setText("F1 - вывод справки\n"
                   "F2 - запрос позиции\n"
                   "F3 - установка позиции\n"
                   "F4 - запрос параметра\n"
                   "F5 - установка параметра\n"
                   "F6 - установка скорости\n"
                   "F7 - пауза в воспроизведении\n"
                   "F8 - восстановление воспроизведения после паузы\n"
                   "F9 - установка шага смещения (по умолчанию 10 сек.)"
                   "Left - смещение позиции на шаг вперед\n"
                   "Right - смещение позиции на шаг назад\n"
                   "q - выход");
    msgBox.exec();
}

void dm::q::Player::showEvent( QShowEvent *event ) {
    m_ready_to_update.store( true );
    event->accept();
}

void dm::q::Player::paintEvent( QPaintEvent *event ) {
    std::lock_guard< std::mutex > lk( m_frame_mutex );

    if( !m_image->isNull() ) {
        QPainter painter( this );
        painter.drawImage(0, 0, m_image->scaled(this->size()));
    }
    event->accept();
}

void dm::q::Player::keyReleaseEvent( QKeyEvent * event ) {
    switch( event->key()  ) {
        case Qt::Key_F1:
            f_show_keyusage_box();
            break;
        case Qt::Key_F2:
            f_ask_position();
            break;
        case Qt::Key_F3:
            f_set_position();
            break;
        case Qt::Key_F4:
            f_ask_parameter();
            break;
        case Qt::Key_F5:
            f_set_parameter();
            break;
        case Qt::Key_F6:
            f_scale();
            break;
        case Qt::Key_F7:
            f_pause();
            break;
        case Qt::Key_F8:
            m_position = 0.;
            f_resume();
            break;
        case Qt::Key_F9:
            f_set_seek_step();
            break;
        case Qt::Key_Right:
            m_position = m_session->position() + m_seek_step;
            f_seek_forward();
            break;
        case Qt::Key_Left:
            m_position = m_session->position() - m_seek_step;
            f_seek_backward();
            break;
        case Qt::Key_Q:
            qApp->quit();
            break;
    }
    event->accept();
}
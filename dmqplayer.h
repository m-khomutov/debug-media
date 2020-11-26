//
// Created by mkh on 16.11.2020.
//
#pragma once

#include "core/dmbasereceiver.h"
#include "core/dmbaseplayer.h"
#include "dmrgbframe.h"

#include <QtGui/QMainWindow>
#include <QtGui/QCloseEvent>
#include <QtCore/QTimer>

#include <memory>
#include <atomic>
#include <mutex>

namespace dm {
    namespace rtsp {
        class MediaSession;
    }
    namespace q {
        class Player : public QMainWindow, public BasePlayer {
            Q_OBJECT
        public:
            Player( rtsp::MediaSession * session, int width, int height, int ask_position_sec );

            void run() override;
            void onFrame( int len, AVFrame * frame ) override;

        protected:
            virtual void showEvent( QShowEvent * event );
            virtual void paintEvent( QPaintEvent * event );
            virtual void keyReleaseEvent( QKeyEvent * event );

        private:
            std::shared_ptr< QImage > m_image;
            rtsp::MediaSession * m_session;
            QTimer* m_position_timer;

            std::atomic_bool m_ready_to_update{false};
            rgb::Frame m_rgbframe;
            std::mutex m_frame_mutex;

            double m_seek_step{10.};
            double m_position{0.};

        private slots:
            void f_show_keyusage_box();
            void f_ask_position();
            void f_set_position();
            void f_ask_parameter();
            void f_set_parameter();
            void f_scale();
            void f_pause();
            void f_resume();
            void f_set_seek_step();
            void f_seek_forward();
            void f_seek_backward();
        };
    }  // namespace q
}  // namespace dm

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
    namespace q {
        class Player : public QMainWindow, public BasePlayer {
            Q_OBJECT
        public:
            Player( int width, int height, int ask_position_sec );

            void run() override;
            void onFrame( AVFrame * frame ) override;

        protected:
            virtual void showEvent( QShowEvent * event );
            virtual void paintEvent( QPaintEvent * event );
            virtual void keyReleaseEvent( QKeyEvent * event );

        private:
            std::shared_ptr< QImage > m_image;
            std::unique_ptr< dm::BaseReceiver > m_receiver;
            QTimer* m_position_timer;

            std::atomic_bool m_ready_to_update{false};
            rgb::Frame m_rgbframe;
            std::mutex m_frame_mutex;

        private slots:
            void f_ask_position();
            void f_show_keyusage_box();
        };
    }  // namespace q
}  // namespace dm

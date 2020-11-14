#include <unistd.h>
#include <signal.h>
#include <locale.h>
#include <sys/time.h>

//#include "basic/seagull_viewer.h"
//#include "mp2ts/seagull_file.h"

#include <QtGui/QApplication>
#include <QtCore/QTextCodec>
#include <iostream>
#include <iomanip>

namespace {
    void catchsig( int nonsence ) {
        qApp->quit();
    }
    void showopts() {
        std::cout << "options: -r|-c|-f|-w|-p|-h\n"
                  << "         -r path to source (required)\n"
                  << "         -c path to cert (default nil)\n"
                  << "         -f .ts file co compare contents with\n"
                  << "         -w PID to watch (default 0 - no pid to watch)\n"
                  << "         -p period to ask position\n"
                  << "         -h this help\n";
    }
    void showtime( std::ostream & out ) {
        timeval tv;
        gettimeofday( &tv, 0 );
        tm *tp = localtime( &tv.tv_sec );
        out << std::setw( 2) << std::setfill( '0') << tp->tm_hour << ":" << tp->tm_min << ":" << tp->tm_sec << ".";
        out << std::setw( 4) << tv.tv_usec;
    }
}



int main( int argc, char* argv[] ) {
    int c;
    const char * source = nullptr;
    const char * cert = nullptr;
    const char * tsfname  = nullptr;
    uint16_t pid_to_watch = 0;
    int ask_position_sec = 0;

    while( (c = getopt( argc, argv, "r:c:f:w:p:h" )) != -1 ) {
        switch( c ) {
            case 'r':
                source = optarg;
                break;
            case 'c':
                cert = optarg;
                break;
            case 'f':
                tsfname = optarg;
                break;
            case 'w':
                pid_to_watch = strtol( optarg, 0, 10 );
                break;
            case 'p':
                ask_position_sec = strtol(optarg, 0, 10);
                break;
            case '?':
            case 'h':
            default:
                showopts();
                return -1;
        }
    }
    if( !source ) {
        showopts();
        return -1;
    }

    signal( SIGCHLD, SIG_IGN  ); //ignore child
    signal( SIGTSTP, SIG_IGN  ); //ignore tty signals
    signal( SIGTTOU, SIG_IGN  );
    signal( SIGTTIN, SIG_IGN  );
    signal( SIGHUP,  SIG_IGN  ); //ignore hangup signal
    signal( SIGTERM, catchsig ); //catch kill signal
    signal( SIGINT,  catchsig ); //catch interrupt signal

    int rc{0};
    try {
        //std::unique_ptr<seagull::mp2ts::File> tsfile;
        //if( tsfn ) tsfile.reset( new seagull::mp2ts::File( tsfn, watch ) );

        QApplication app( argc, argv );
                     app.setApplicationName( "dmplayer" );
        QTextCodec *utfcodec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForTr      (utfcodec);
        QTextCodec::setCodecForCStrings(utfcodec);

        setlocale( LC_ALL, "POSIX" );
        //std::unique_ptr< seagull::basic::Viewer > viewer = seagull::basic::Viewer::Make( res, cert, ask_position_sec, tsfile.get() );
        //viewer->view();
        rc = app.exec();
    }
    catch( const std::logic_error& err ) {
        std::cerr << err.what() << std::endl;
        rc = 1;
    }
    return rc;
}

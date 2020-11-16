#include "core/dmbasereceiver.h"

#include <unistd.h>
#include <signal.h>
#include <locale.h>
#include <sys/time.h>

#include <QtGui/QApplication>
#include <QtCore/QTextCodec>
#include <iostream>
#include <iomanip>

namespace {
    std::unique_ptr< dm::BaseReceiver > receiver;

    void catchsig( int nonsence ) {
        qApp->quit();
    }
    void showopts() {
        std::cout << "options: -s|-c|-f|-w|-p|-h\n"
                  << "         -s source url (required)\n"
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
}  // namespace



int main( int argc, char* argv[] ) {
    int c;
    const char * source = nullptr;
    const char * cert = nullptr;
    int ask_position_sec = 0;

    while( (c = getopt( argc, argv, "s:c:p:h" )) != -1 ) {
        switch( c ) {
            case 's':
                source = optarg;
                break;
            case 'c':
                cert = optarg;
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
        QApplication app( argc, argv );
                     app.setApplicationName( "dmplayer" );
        QTextCodec *utfcodec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForTr      (utfcodec);
        QTextCodec::setCodecForCStrings(utfcodec);
        setlocale( LC_ALL, "POSIX" );
        receiver.reset( dm::BaseReceiver::create( source, cert ) );
        receiver->run();

        rc = app.exec();
    }
    catch( const std::logic_error& err ) {
        std::cerr << err.what() << std::endl;
        rc = 1;
    }
    return rc;
}

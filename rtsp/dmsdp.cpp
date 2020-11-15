#include "dmsdp.h"

#include <string.h>

namespace dm {
    namespace rtsp {
        std::ostream& operator <<( std::ostream& out, const dm::rtsp::SessionOriginator & originator ) {
            for( size_t i(dm::rtsp::SessionOriginator::kUsername); i <= dm::rtsp::SessionOriginator::kUnicastAddress; ++i ) {
                if( originator.fields.size() < i+1 )
                    break;
                switch( i ) {
                    case dm::rtsp::SessionOriginator::kUsername:
                        out << "username=";
                        break;
                    case dm::rtsp::SessionOriginator::kSessionId:
                        out << "session id=";
                        break;
                    case dm::rtsp::SessionOriginator::kSessionVersion:
                        out << "session version=";
                        break;
                    case dm::rtsp::SessionOriginator::kNetworkType:
                        out << " network type=";
                        break;
                    case dm::rtsp::SessionOriginator::kAddressType:
                        out << " address type=";
                        break;
                    case dm::rtsp::SessionOriginator::kUnicastAddress:
                        out << " unicast address=";
                        break;
                }
                out << originator.fields[ i ] << " ";
            }
            return out;
        }
        std::ostream& operator <<( std::ostream& out, const dm::rtsp::SessionDescription & description ) {
            out << "*** session *** ";
            out << "\n\tprotocol version=" << description.protocol_version
                << "\n\t" << description.session_originator;
            if( !description.name.empty() )
                out << "\n\tsession name=" << description.name;
            if( !description.information.empty() )
                out << "\n\tsession information=" << description.information;
            if( !description.uri.empty() )
                out << "\n\tdescription uri=" << description.uri;
            if( !description.email.empty() )
                out << "\n\temail address=" << description.email;
            if( !description.phone.empty() )
                out << "\n\tphone number=" << description.phone;
            if( !description.connection.empty() )
                out << "\n\tconnection information=" << description.connection;
            if( !description.bandwidth.empty() ) {
                out << "\n\tbandwidth information=";
                for( auto bw : description.bandwidth )
                    out << bw << " ";
            }
            if( !description.active_time.empty() ) {
                out << "\n\ttime the session is active=";
                for( auto at : description.active_time )
                    out << at << " ";
            }
            if( !description.repeat_time.empty() ) {
                out << "\n\trepeat times=";
                for( auto rt : description.repeat_time )
                    out << rt << " ";
            }
            if( !description.tz_adjustment.empty() )
                out << "\n\ttime zone adjustments=" << description.tz_adjustment;
            if( !description.encryption_key.empty() )
                out << "\n\tencryption key=" << description.encryption_key;
            if( !description.attributes.empty() ) {
                out << "\n\tattributes=" << std::endl;
                for( auto a : description.attributes )
                    out << "\t\t" << a << std::endl;
            }
            return out;
        }
        std::ostream& operator <<( std::ostream& out, const dm::rtsp::MediaName & name ) {
            for( size_t i(dm::rtsp::MediaName::kType); i <= dm::rtsp::MediaName::kFormat; ++i ) {
                if( name.fields.size() < i+1 )
                    break;
                switch( i ) {
                    case dm::rtsp::MediaName::kType:
                        out << "type=";
                        break;
                    case dm::rtsp::MediaName::kPort:
                        out << "port=";
                        break;
                    case dm::rtsp::MediaName::kProtocol:
                        out << "protocol=";
                        break;
                    case dm::rtsp::MediaName::kFormat:
                        out << "format=";
                        break;
                }
                out << name.fields[ i ] << " ";
            }
            return out;
        }
        std::ostream& operator <<( std::ostream& out, const dm::rtsp::MediaDescription & description ) {
            out << "*** media *** \n\t";
            out << description.name;
            if( !description.title.empty() )
                out << "\n\tmedia title=" << description.title << std::endl;
            if( !description.connection.empty() )
                out << "\n\tconnection information=" << description.connection << std::endl;
            if( !description.bandwidth.empty() ) {
                out << "\n\tbandwidth information=\n";
                for( auto bw : description.bandwidth )
                    out << "\t\t" << bw << std::endl;
            }
            if( !description.encryption_key.empty() )
                out << "'\n\tencryption key=" << description.encryption_key;
            if( !description.attributes.empty() ) {
                out << "\n\tattributes=\n";
                for( auto a : description.attributes )
                    out << "\t\t" << a << std::endl;
            }
            out << "CONTROL : " << description.control << std::endl;
            return out;
        }

    }  // namespace rtsp
}  // namespace dm

namespace {
    std::string assign( const std::string& line ) {
        std::string ret = line.substr(2);
        if( !std::isgraph( ret.back() ) )
            ret.pop_back();
        return ret;
    }
}  // namespace


void dm::rtsp::SessionDescription::parse( const std::string& line ) {
    switch( line[0] ) {
        case 'v':
            protocol_version = std::stoi( assign( line ) );
            break;
        case 'o':
            session_originator.parse( line.substr(2) );
            break;
        case 's':
            name = assign( line );
            break;
        case 'i':
            information = assign( line );
            break;
        case 'u':
            uri = assign( line );
            break;
        case 'e':
            email = assign( line );
            break;
        case 'p':
            phone = assign( line );
            break;
        case 'c':
            connection = assign( line );
            break;
        case 'b':
            bandwidth.push_back( assign( line ) );
            break;
        case 't':
            active_time.push_back( assign( line ) );
            break;
        case 'r':
            repeat_time.push_back( assign( line ) );
            break;
        case 'z':
            tz_adjustment = assign( line );
            break;
        case 'k':
            encryption_key = assign( line );
            break;
        case 'a':
            addAttribute( assign( line ) );
           break;
    }
}

void dm::rtsp::MediaDescription::parse( const std::string & line ) {
    switch( line[0] ) {
        case 'm':
            name.parse( line.substr(2) );
            break;
        case 'i':
            title = assign( line );
            break;
        case 'c':
            connection = assign( line );
            break;
        case 'b':
            bandwidth.push_back( assign( line ) );
            break;
        case 'k':
            encryption_key = assign( line );
            break;
        case 'a':
            addAttribute( assign( line ) );
            break;
    }
}

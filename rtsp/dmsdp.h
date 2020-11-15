#pragma once
#include <iostream>
#include <cstring>
#include <locale>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <iterator>

namespace dm {
    namespace rtsp {
        // Класс определения разделителя строки на маркеры
        using ctype_char = std::ctype< char >;
        struct div_facet : ctype_char {
            static std::ctype_base::mask const* get_table( uint8_t dividing_char ) {
                static const ctype_char::mask *clt = ctype_char::classic_table();
                static ctype_char::mask ret[ ctype_char::table_size ];

                std::memcpy( ret, clt, ctype_char::table_size * sizeof(ctype_char::mask) );
                ret[dividing_char] = std::ctype_base::space;
                ret[' '] = std::ctype_base::space;
                return &ret[ 0 ];
            }
            div_facet(uint8_t dividing_char): ctype_char( get_table(dividing_char) )
            {}
        };
        // Класс разделения строки на маркеры
        class StringToTokens {
        public:
            StringToTokens( const std::string& string_to_divide, uint8_t dividing_char ) {
                std::stringstream ss( string_to_divide );
                ss.imbue( std::locale(std::locale(), new div_facet(dividing_char)) );
                m_tokens = std::vector< std::string >( std::istream_iterator<std::string>(ss), std::istream_iterator<std::string>() );
            }
            const std::string& operator[]( std::vector<std::string>::size_type at ) const {
                return m_tokens[ at ];
            }

            std::vector<std::string>::size_type size() const {
                return m_tokens.size();
            }
            const std::string* find( const char * what ) {
                for( auto & t : m_tokens ) {
                    if( t.find(what) != std::string::npos )
                        return &t;
                }
                return nullptr;
            }

        private:
            std::vector< std::string > m_tokens;
        };

        struct SDP {
            virtual ~SDP() = default;

            bool media() {
                return control.find( "rtsp://" ) == std::string::npos;
            }
            const std::string* find( const char* what ) {
                for( auto & att : attributes )
                    if( att.find( what ) != std::string::npos )
                        return &att;
                return nullptr;
            }
            void addAttribute( const std::string & line ) {
                attributes.push_back( line );
                if( line.find( "control:" ) == 0 ) {
                    control = line.substr(8);
                    if( !std::isgraph( control.back()) )
                        control.pop_back();
                }
                else if( line.find( "fmtp:" ) == 0 ) {
                    fmtp = line.substr(5);
                }
                else if( line.find( "rtpmap:" ) == 0 ) {
                    rtpmap = line.substr( 7 );
                }
            }

            virtual void parse(const std::string& line) = 0;

            std::string control;
            std::string fmtp;
            std::string rtpmap;
            std::string connection;
            std::string encryption_key;

            std::vector< std::string > bandwidth;
            std::vector< std::string > attributes;
        };

        struct SessionOriginator {
            void parse( const std::string& line ) {
                size_t p1 = 0, p2;
                while( (p2 = line.find( ' ', p1 )) != std::string::npos ) {
                    fields.push_back( line.substr( p1, p2-p1 ) );
                    p1 = p2+1;
                }
                fields.push_back( line.substr( p1 ) );
                fields.back().pop_back(); /*0x0d */
            }

            enum { kUsername, kSessionId, kSessionVersion, kNetworkType, kAddressType, kUnicastAddress };
            std::vector< std::string > fields;

            friend std::ostream & operator<<( std::ostream & out, const SessionOriginator & originator );
        };

        struct SessionDescription: public SDP {
            void parse( const std::string& line ) override;

            int protocol_version;
            SessionOriginator session_originator;
            std::string name;
            std::string information;
            std::string uri;
            std::string email;
            std::string phone;

            std::vector<std::string> active_time;
            std::vector<std::string> repeat_time;

            std::string tz_adjustment;

            friend std::ostream & operator<<( std::ostream & out, const SessionDescription & description );
        };

        struct MediaName {
            void parse( const std::string& line ) {
                size_t p1 = 0, p2;
                while( (p2 = line.find( ' ', p1 )) != std::string::npos ) {
                    fields.push_back( line.substr( p1, p2-p1 ) );
                    p1 = p2+1;
                }
                fields.push_back( line.substr( p1 ) );
                fields.back().pop_back(); /*0x0d */
            }
            const std::string & protocol() {
                if( fields.size() > kProtocol )
                    return fields[kProtocol];
                return defprotocol;
            }
            enum {kType, kPort, kProtocol, kFormat};
            std::vector< std::string > fields;
            std::string defprotocol = "RTP/AVP";

            friend std::ostream & operator<<( std::ostream & out, const MediaName & name );
        };

        struct MediaDescription: SDP {
            void parse(const std::string& line) override;

            std::string title;
            MediaName name;

            friend std::ostream & operator<<( std::ostream & out, const MediaDescription & description );
        };
    }  // namespace rtsp
}  // namespace dm

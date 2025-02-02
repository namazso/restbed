//System Includes
#include <chrono>
#include <thread>
#include <string>
#include <memory>
#include <vector>
#include <ciso646>
#include <stdexcept>
#include <functional>
#include <system_error>

//Project Includes
#include <restbed>

//External Includes
#include <boost/asio.hpp>
#include <catch2/catch.hpp>

//System Namespaces
using std::thread;
using std::string;
using std::vector;
using boost::system::error_code;
using std::shared_ptr;
using std::make_shared;
using std::chrono::seconds;

//Project Namespaces
using namespace restbed;

//External Namespaces
using boost::asio::ip::tcp;
using boost::asio::connect;
using boost::asio::io_service;
using boost::asio::socket_base;

void get_handler( const shared_ptr< Session > session )
{
    session->close( 200 );
}

SCENARIO( "validate connection timeout after connect", "[socket]" )
{
    auto resource = make_shared< Resource >( );
    resource->set_path( "/resource" );
    resource->set_method_handler( "GET", get_handler );
    
    auto settings = make_shared< Settings >( );
    settings->set_port( 1984 );
    settings->set_connection_timeout( seconds( 2 ) );
    
    shared_ptr< thread > worker = nullptr;
    
    Service service;
    service.publish( resource );
    service.set_ready_handler( [ &worker ]( Service & service )
    {
        worker = make_shared< thread >( [ &service ] ( )
        {
            GIVEN( "I create a service with a socket connection timeout of '2' seconds" )
            {
                WHEN( "I establish a network connection and wait '4' seconds" )
                {
                    io_service io_service;
                    boost::asio::ip::tcp::socket socket( io_service );
                    boost::asio::ip::tcp::endpoint endpoint( boost::asio::ip::address::from_string( "127.0.0.1" ), 1984 );

                    boost::system::error_code error;
                    socket.connect( endpoint, error );
                    
                    REQUIRE( true == socket.is_open( ) );
                    
                    std::this_thread::sleep_for( seconds( 4 ) );
                    
                    THEN( "I should see the peer close the socket" )
                    {
                        uint8_t data;
                        socket.receive( boost::asio::buffer( &data, 1 ), socket_base::message_peek, error );
                        
                        REQUIRE( ( error == boost::asio::error::eof or error == boost::asio::error::connection_reset ) );
                    }
                    
                    socket.close( );
                }
                
                service.stop( );
            }
        } );
    } );
    
    service.start( settings );
    worker->join( );
}

SCENARIO( "validate connection timeout after partial request", "[socket]" )
{
    auto resource = make_shared< Resource >( );
    resource->set_path( "/resource" );
    resource->set_method_handler( "GET", get_handler );
    
    auto settings = make_shared< Settings >( );
    settings->set_port( 1984 );
    settings->set_connection_timeout( seconds( 2 ) );
    
    shared_ptr< thread > worker = nullptr;
    
    Service service;
    service.publish( resource );
    service.set_ready_handler( [ &worker ]( Service & service )
    {
        worker = make_shared< thread >( [ &service ] ( )
        {
            GIVEN( "I create a service with a socket connection timeout of '2' seconds" )
            {
                AND_THEN( "I perform a partial HTTP request" )
                {
                    io_service io_service;
                    boost::asio::ip::tcp::socket socket( io_service );
                    boost::asio::ip::tcp::endpoint endpoint( boost::asio::ip::address::from_string( "127.0.0.1" ), 1984 );

                    boost::system::error_code error;
                    socket.connect( endpoint, error );
                    
                    REQUIRE( true == socket.is_open( ) );
                    
                    string request = "GET /resource HTTP/1.1\r\n";
                    socket.send( boost::asio::buffer( request, request.length( ) ), 0, error );
                    
                    WHEN( "I wait '4' seconds" )
                    {
                        std::this_thread::sleep_for( seconds( 4 ) );
                        
                        THEN( "I should see the peer close the socket" )
                        {
                            uint8_t data;
                            socket.receive( boost::asio::buffer( &data, 1 ), socket_base::message_peek, error );
                            
                            REQUIRE( ( error == boost::asio::error::eof or error == boost::asio::error::connection_reset ) );
                        }
                        
                        socket.close( );
                    }
                }
                
                service.stop( );
            }
        } );
    } );
    
    service.start( settings );
    worker->join( );
}

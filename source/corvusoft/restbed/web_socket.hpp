/*
 * Copyright 2013-2020, Corvusoft Ltd, All Rights Reserved.
 */

#pragma once

//System Includes
#include <string>
#include <memory>
#include <functional>
#include <system_error>

//Project Includes
#include <corvusoft/restbed/web_socket_message.hpp>

//External Includes
#include <boost/system/error_code.hpp>

//Windows DLL Exports
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64)) && !defined(RESTBED_BUILD_STATIC)
	#ifdef WIN_DLL_EXPORT
		#define WEB_SOCKET_EXPORT __declspec(dllexport)
	#else
		#define WEB_SOCKET_EXPORT __declspec(dllimport)
	#endif
#else
	#define WEB_SOCKET_EXPORT
#endif

//System Namespaces

//Project Namespaces

//External Namespaces

namespace restbed
{
    //Forward Declarations
    class Logger;
    class Session;
    
    namespace detail
    {
        class SocketImpl;
        class WebSocketImpl;
        class WebSocketManagerImpl;
    }
    
    class WEB_SOCKET_EXPORT WebSocket : public std::enable_shared_from_this< WebSocket >
    {
        public:
            //Friends
            
            //Definitions
            
            //Constructors
            WebSocket( void );
            
            ~WebSocket( void );
            
            //Functionality
            bool is_open( void ) const;
            
            bool is_closed( void ) const;
            
            void close( void );
            
            void send( const Bytes& body, const std::function< void ( const std::shared_ptr< WebSocket > ) > callback = nullptr );
            
            void send( const std::string& body, const std::function< void ( const std::shared_ptr< WebSocket > ) > callback = nullptr );
            
            void send( const WebSocketMessage::OpCode opcode, const std::function< void ( const std::shared_ptr< WebSocket > ) > callback = nullptr );
            
            void send( const std::shared_ptr< WebSocketMessage > message, const std::function< void ( const std::shared_ptr< WebSocket > ) > callback = nullptr );
            
            //Getters
            std::string get_key( void ) const;
            
            std::shared_ptr< Logger > get_logger( void ) const;
            
            std::shared_ptr< detail::SocketImpl > get_socket( void ) const;
            
            std::function< void ( const std::shared_ptr< WebSocket > ) > get_open_handler( void ) const;
            
            std::function< void ( const std::shared_ptr< WebSocket > ) > get_close_handler( void ) const;
            
            std::function< void ( const std::shared_ptr< WebSocket >, const boost::system::error_code ) > get_error_handler( void ) const;
            
            std::function< void ( const std::shared_ptr< WebSocket >, const std::shared_ptr< WebSocketMessage > ) > get_message_handler( void ) const;
            
            //Setters
            void set_key( const std::string& value );
            
            void set_logger( const std::shared_ptr< Logger >& value );
            
            void set_socket( const std::shared_ptr< detail::SocketImpl >& value );
            
            void set_open_handler( const std::function< void ( const std::shared_ptr< WebSocket > ) >& value );
            
            void set_close_handler( const std::function< void ( const std::shared_ptr< WebSocket > ) >& value );
            
            void set_error_handler( const std::function< void ( const std::shared_ptr< WebSocket >, const boost::system::error_code ) >& value );
            
            void set_message_handler( const std::function< void ( const std::shared_ptr< WebSocket >, const std::shared_ptr< WebSocketMessage > ) >& value );
            
            //Operators
            
            //Properties
            
        protected:
            //Friends
            
            //Definitions
            
            //Constructors
            
            //Functionality
            
            //Getters
            
            //Setters
            
            //Operators
            
            //Properties
            
        private:
            //Friends
            friend Session;
            friend detail::WebSocketManagerImpl;
            
            //Definitions
            
            //Constructors
            WebSocket( const WebSocket& original ) = delete;
            
            //Functionality
            
            //Getters
            
            //Setters
            
            //Operators
            WebSocket& operator =( const WebSocket& value ) = delete;
            
            //Properties
            std::unique_ptr< detail::WebSocketImpl > m_pimpl;
    };
}

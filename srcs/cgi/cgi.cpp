#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <vector>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <sys/select.h>
#include <signal.h>
#include <errno.h>
#include "../../includes/response/ResponseMaker.hpp"
#include <sys/time.h>

std::string	we_readWithTimeout( int fd, pid_t child_pid, int timeout_sec )
{
	std::string		result;
	char			buffer[1024];
	fd_set			readfds;
	struct timeval	start, now, timeout;
	long			elapsed_sec;

	gettimeofday( &start, NULL );

	while ( true )
	{
		gettimeofday( &now, NULL );
		elapsed_sec = now.tv_sec - start.tv_sec;

		if ( elapsed_sec >= timeout_sec )
		{
			kill( child_pid, SIGKILL );
			waitpid( child_pid, NULL, 0 );
			throw std::runtime_error( "Timeout dépassé pour l'exécution CGI" );
		}

		FD_ZERO( &readfds );
		FD_SET( fd, &readfds );

		timeout.tv_sec = timeout_sec - elapsed_sec;
		timeout.tv_usec = 0;

		int	activity = select( fd + 1, &readfds, NULL, NULL, &timeout );

		if ( activity < 0 && errno != EINTR )
			throw std::runtime_error( "Erreur avec select()" );

		if ( activity == 0 )
			continue;

		if ( FD_ISSET( fd, &readfds ) )
		{
			ssize_t	bytes_read = read( fd, buffer, sizeof( buffer ) - 1 );
			if ( bytes_read < 0 )
				throw std::runtime_error( "Erreur lors de la lecture du pipe" );

			if ( bytes_read == 0 )
				break;

			buffer[bytes_read] = '\0';
			result += buffer;
		}

		int	status;
		if ( waitpid( child_pid, &status, WNOHANG ) > 0 )
			break;
	}

	close( fd );
	return ( result );
}

std::string	we_executeCGI( const std::string &binary, const std::string &scriptPath , ErrorManagement &err )
{
	char	*argv[] = { ( char * )binary.c_str(), ( char * )scriptPath.c_str(), NULL };

	int	pipefd[2];

	if ( pipe( pipefd ) == -1 )
	{
		if ( err.getErrorCode() == 0 )
			err.setErrorCode( 500 );
		throw ResponseMaker::ResponseException( "Erreur : Impossible de créer le pipe\n" );
	}

	pid_t	pid = fork( );

	if ( pid < 0 )
	{
		if ( err.getErrorCode() == 0 )
			err.setErrorCode( 500 );
		throw ResponseMaker::ResponseException( "Erreur : Échec du fork()\n" );
	}

	if ( pid == 0 )
	{
		if ( dup2( pipefd[1], STDOUT_FILENO ) == -1 )
		{
			std::cerr << "Erreur : Impossible de rediriger stdout ( " << strerror( errno ) << " )\n";
			_exit(1);
		}
		close( pipefd[0] );
		close( pipefd[1] );

		execve( ( char * )binary.c_str(), argv, NULL );

		std::cerr << "Erreur : Échec de l'exécution de " << scriptPath << " ( " << strerror( errno ) << " )\n";
		std::cout << "Status: 500 Internal Server Error\n"
				<< "Content-Type: text/plain\n\n"
				<< "500 Internal Server Error: CGI execution failed.\n";
		_exit( 1 );
	}
	else
	{
		close( pipefd[1] );

		try
		{
			std::string output = we_readWithTimeout(pipefd[0], pid, 5);
			return ( output );
		}
		catch ( const std::exception &e )
		{
			std::cerr << "Erreur : " << e.what() << std::endl;
			if (err.getErrorCode() == 0)
				err.setErrorCode(504);
			throw ResponseMaker::ResponseException("");
		}
	}
}

std::string	we_checkCGI( const std::string& binary, const std::string& file, ErrorManagement &err )
{
	std::string	scriptPath;
	scriptPath += file;

	if ( access( scriptPath.c_str( ), F_OK ) == -1 )
	{
		std::cerr << "Erreur : Fichier CGI introuvable ( " << scriptPath << " )\n";
		if (err.getErrorCode() == 0)
			err.setErrorCode(404);
		throw ResponseMaker::ResponseException("");
	}
	if ( access( scriptPath.c_str( ), X_OK ) == -1 )
	{
		std::cerr << "Erreur : Fichier CGI non exécutable ( " << scriptPath << " )\n";
		if (err.getErrorCode() == 0)
			err.setErrorCode(403);
		throw ResponseMaker::ResponseException("");	}
	if ( access( binary.c_str( ), F_OK ) == -1 )
	{
		std::cerr << "Erreur : Binaire CGI introuvable ( " << binary << " )\n";
		if (err.getErrorCode() == 0)
			err.setErrorCode(404);
		throw ResponseMaker::ResponseException("");
	}
	if ( access( binary.c_str( ), X_OK ) == -1 )
	{
		std::cerr << "Erreur : binaire CGI non exécutable ( " << binary << " )\n";
		if (err.getErrorCode() == 0)
			err.setErrorCode(403);
		throw ResponseMaker::ResponseException("");
	}
	return ( we_executeCGI( binary, scriptPath, err ) );
}
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


std::string	we_readWithTimeout( int fd, pid_t child_pid, int timeout_sec )
{
	std::string		output;
	char			buffer[1024];
	fd_set			readfds;
	struct timeval	timeout;
	int				max_fd = fd;
	int				elapsed = 0;
	int				status;
	bool			finished = false;

	while ( elapsed < timeout_sec * 10 )
	{
		FD_ZERO( &readfds );
		FD_SET( fd, &readfds );

		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;

		int	ready = select( max_fd + 1, &readfds, NULL, NULL, &timeout );
		if ( ready == -1 )
		{
			perror( "select" );
			break;
		}
		else if ( ready == 0 )
		{
			elapsed++;
			pid_t	result = waitpid(child_pid, &status, WNOHANG);
			if (result == child_pid)
			{
				finished = true;
				break;
			}
			continue;
		}
		else
		{
			if ( FD_ISSET( fd, &readfds ) )
			{
				ssize_t	bytesRead = read( fd, buffer, sizeof( buffer ) );
				if (bytesRead > 0)
					output.append( buffer, bytesRead );
				else
					break;
			}
		}
	}

	if ( !finished )
	{
		std::cerr << "CGI Timeout: killing process " << child_pid << "\n";
		kill( child_pid, SIGKILL );
		waitpid( child_pid, NULL, 0 );
		throw std::runtime_error( "CGI Timeout exceeded" );
	}

	close(fd);

	if ( WIFEXITED( status ) && WEXITSTATUS( status ) != 0 )
	{
		std::cerr << "CGI exited with status: " << WEXITSTATUS( status ) << "\n";
		throw std::runtime_error( "CGI returned error" );
	}

	return ( output );
}

std::string	we_executeCGI( const std::string &binary, const std::string &scriptPath , ErrorManagement &err )
{
	char	*argv[] = {( char * )binary.c_str(), ( char * )scriptPath.c_str(), NULL };

	int	pipefd[2];

	if ( pipe( pipefd ) == -1 )
	{
		if (err.getErrorCode() == 0)
			err.setErrorCode(500);
		throw ResponseMaker::ResponseException("Erreur : Impossible de créer le pipe\n");
	}

	pid_t	pid = fork( );

	if ( pid < 0 )
	{
		if (err.getErrorCode() == 0)
			err.setErrorCode(500);
		throw ResponseMaker::ResponseException("Erreur : Échec du fork()\n");
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

		/*	exeption	*/

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

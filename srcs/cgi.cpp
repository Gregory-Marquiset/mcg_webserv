#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <vector>
#include <cerrno>
#include <cstring>
#include <fstream>

std::string	we_executeCGI( const std::string &scriptPath )
{
	char		*argv[] = { (char *)scriptPath.c_str(), ( char * )scriptPath.c_str( ), NULL };

	int	pipefd[2];

	if ( pipe( pipefd ) == -1 )
	{
		std::cerr << "Erreur : Impossible de créer le pipe\n";
		return ( "500 Internal Server Error" );
	}

	pid_t	pid = fork( );

	if ( pid < 0 )
	{
		std::cerr << "Erreur : Échec du fork()\n";
		return ( "500 Internal Server Error" );
	}

	if ( pid == 0 )
	{
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
			std::cerr << "Erreur : Impossible de rediriger stdout ( " << strerror( errno ) << " )\n";
			_exit(1);
		}
		close( pipefd[0] );
		close( pipefd[1] );

		execve( argv[0], argv, NULL );

		std::cerr << "Erreur : Échec de l'exécution de " << scriptPath << " ( " << strerror( errno ) << " )\n";
		std::cout << "Status: 500 Internal Server Error\n"
				<< "Content-Type: text/plain\n\n"
				<< "500 Internal Server Error: CGI execution failed.\n";

		_exit( 1 );
	}
	else
	{
		close( pipefd[1] );

		std::string	output;
		char		buffer[1024];
		ssize_t		bytesRead;

		while ( ( bytesRead = read( pipefd[0], buffer, sizeof( buffer ) ) ) > 0 )
			output.append( buffer, bytesRead );
		if ( bytesRead == -1 )
		{
			std::cerr << "Erreur : Problème lors de la lecture du pipe ( " << strerror( errno ) << " )\n";
			close( pipefd[0] );
			return ( "500 Internal Server Error" );
		}
		close ( pipefd[0] );

		int	status;

		waitpid( pid, &status, 0 );
		if (WIFEXITED( status ) && WEXITSTATUS( status ) != 0 )
		{
			std::cerr << "Erreur : Le CGI a retourné un code d'erreur ( " << WEXITSTATUS( status ) << " )\n";
			return ( "500 Internal Server Error" );
		}

		return ( output );
	}
}

std::string	we_checkCGI( const std::string &file )
{
	std::string	scriptPath = ".";
	scriptPath += file;

	if ( access( scriptPath.c_str( ), F_OK ) == -1 )
	{
		std::cerr << "Erreur : Fichier CGI introuvable ( " << scriptPath << " )\n";
		return ( "404 Not Found" );
	}
	if ( access( scriptPath.c_str( ), X_OK ) == -1 )
	{
		std::cerr << "Erreur : Fichier CGI non exécutable ( " << scriptPath << " )\n";
		return ( "403 Forbidden" );
	}

	return ( we_executeCGI( scriptPath ) );
}

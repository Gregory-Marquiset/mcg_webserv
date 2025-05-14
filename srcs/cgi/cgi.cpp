#include <sys/wait.h>
#include <cstring>
#include <sys/select.h>
#include <sys/time.h>
#include "../../includes/response/ResponseMaker.hpp"

std::string	we_readWithTimeout(int fd, pid_t child_pid, int timeout_sec)
{
	struct timeval	timeout;
	std::string		result;
	fd_set			readfds;
	char			buffer[1024];
	int				status;

	while (true)
	{
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);

		timeout.tv_sec = timeout_sec;
		timeout.tv_usec = 0;

		int	activity = select(fd + 1, &readfds, NULL, NULL, &timeout);

		if (activity < 0)
			throw std::runtime_error("select() Error");

		if (activity == 0)
		{
			kill(child_pid, SIGKILL);
			waitpid(child_pid, NULL, 0);
			throw std::runtime_error("Timeout dépassé pour l'exécution CGI");
		}

		if (FD_ISSET(fd, &readfds))
		{
			ssize_t	bytes_read = read(fd, buffer, sizeof(buffer) - 1);

			if (bytes_read < 0)
				throw std::runtime_error("Erreur lors de la lecture du pipe");

			if (bytes_read == 0)
				break;

			buffer[bytes_read] = '\0';
			result += buffer;
		}
		if (waitpid(child_pid, &status, WNOHANG) > 0)
			break;
	}

	close(fd);
	return (result);
}

std::string	we_executeCGI(const std::string &binary, const std::string &scriptPath,
	const std::string &cookieHeader, const std::string &body, ErrorManagement &err)
{
	char	*argv[] = { (char *)binary.c_str(), (char *)scriptPath.c_str(), NULL };
	int		pipefd_out[2];
	int		pipefd_in[2];

	if (pipe(pipefd_out) == -1 || pipe(pipefd_in) == -1)
	{
		if (err.getErrorCode() == 0)
			err.setErrorCode(500);
		throw ResponseMaker::ResponseException("Erreur : Impossible de créer le pipe");
	}

	pid_t	pid = fork();

	if (pid < 0)
	{
		if (err.getErrorCode() == 0)
			err.setErrorCode(500);
		throw ResponseMaker::ResponseException("Erreur : Échec du fork()");
	}

	if (pid == 0)
	{
		if (dup2(pipefd_out[1], STDOUT_FILENO) == -1)
		{
			std::cerr << "Erreur : dup2 stdout" << std::endl;
			_exit(1);
		}

		if (dup2(pipefd_in[0], STDIN_FILENO) == -1)
		{
			std::cerr << "Erreur : dup2 stdin" << std::endl;
			_exit(1);
		}

		close(pipefd_out[0]);
		close(pipefd_out[1]);
		close(pipefd_in[0]);
		close(pipefd_in[1]);

		std::vector<std::string>	envStrings;
		std::vector<char *>			envp;

		if (!cookieHeader.empty())
			envStrings.push_back("HTTP_COOKIE=" + cookieHeader);

		envStrings.push_back("REQUEST_METHOD=POST");

		std::stringstream	ss;
		ss << body.length();
		envStrings.push_back("CONTENT_LENGTH=" + ss.str());

		envStrings.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");

		for (size_t i = 0; i < envStrings.size(); ++i)
			envp.push_back(const_cast<char *>(envStrings[i].c_str()));
		envp.push_back(NULL);

		execve((char *)binary.c_str(), argv, &envp[0]);

		std::cerr << "Erreur : execve a échoué" << std::endl;
		_exit(1);
	}
	else
	{
		close(pipefd_out[1]);
		close(pipefd_in[0]);

		if (!body.empty())
		{
			ssize_t bytes_written = write(pipefd_in[1], body.c_str(), body.length());
			if (bytes_written == -1)
				std::cerr << "Erreur lors de l'envoi du body au CGI" << std::endl;
		}
		close(pipefd_in[1]);

		try
		{
			std::string output = we_readWithTimeout(pipefd_out[0], pid, 5);
			return (output);
		}
		catch (const std::exception &e)
		{
			std::cerr << "Erreur : " << e.what() << std::endl;
			if (err.getErrorCode() == 0)
				err.setErrorCode(504);
			throw ResponseMaker::ResponseException("");
		}
	}
}

std::string	we_checkCGI(const std::string &binary, const std::string &file,
	const std::string &cookieHeader, const std::string &body, ErrorManagement &err)
{
	std::string	scriptPath = file;

	if (access(scriptPath.c_str(), F_OK) == -1)
	{
		std::cerr << "Erreur : Fichier CGI introuvable (" << scriptPath << ")" << std::endl;
		if (err.getErrorCode() == 0)
			err.setErrorCode(404);
		throw ResponseMaker::ResponseException("");
	}
	if (access(scriptPath.c_str(), X_OK) == -1)
	{
		std::cerr << "Erreur : Fichier CGI non exécutable (" << scriptPath << ")" << std::endl;
		if (err.getErrorCode() == 0)
			err.setErrorCode(403);
		throw ResponseMaker::ResponseException("");
	}
	if (access(binary.c_str(), F_OK) == -1)
	{
		std::cerr << "Erreur : Binaire CGI introuvable (" << binary << ")" << std::endl;
		if (err.getErrorCode() == 0)
			err.setErrorCode(404);
		throw ResponseMaker::ResponseException("");
	}
	if (access(binary.c_str(), X_OK) == -1)
	{
		std::cerr << "Erreur : Binaire CGI non exécutable (" << binary << ")" << std::endl;
		if (err.getErrorCode() == 0)
			err.setErrorCode(403);
		throw ResponseMaker::ResponseException("");
	}
	return (we_executeCGI(binary, scriptPath, cookieHeader, body, err));
}
